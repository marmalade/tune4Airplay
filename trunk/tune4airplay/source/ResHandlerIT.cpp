#include <IwFile.h>
#include "ResHandlerIT.h"
#include "t4aSampleIT.h"
#include "t4aInstrumentIT.h"

using namespace Tune4Airplay;

#ifdef IW_BUILD_RESOURCES
IW_MANAGED_IMPLEMENT(CResHandlerIT)

#pragma pack(push)
#pragma pack(1)
struct FileHeaderIT
{
	int32 IMPM;
	char name [26];
	uint16 PHiligt;
	uint16 OrdNum;
	uint16 InsNum;
	uint16 SmpNum;
	uint16 PatNum;
	uint16 Cwt;
	uint16 Cmwt;
	uint16 Flags;
	uint16 Special;

	int8 GV;
	int8 MV;
	int8 IS;
	int8 IT;
	int8 Sep;
	int8 PWD;

	int16 MsgLgth;
	int32 MessageOffset;
	int32 Reserved;
	int8 ChnlPan[64];
	int8 ChnlVol[64];
};
struct InstrumentHeaderOldIT
{
	int32 IMPI;
	char DOSName [12];
	uint8 zero;
	uint8 Flg;
	uint8 VLS;
	uint8 VLE;
	uint8 SLS;
	uint8 SLE;
	uint8  x0;
	uint8  x1;
	uint16 FadeOut;
	uint8 NNA;
	uint8 DNC;
	uint16 TrkVers;
	uint8 NoS;
	uint8  x2;
	char Name [26];
	uint8  x3;
	uint8  x4;
	uint8  x5;
	uint8  x6;
	uint8  x7;
	uint8  x8;
	uint8 NoteSample [240];
	uint8 VolumeEnvelope [200];
	uint16 NodePoints [25];
};
struct NodePointIT
{
	uint8 Y;
	uint16 tickNumber;
};
struct EnvelopeHeaderIT
{
	uint8 Flg;
	uint8 Num;
	uint8 LpB;
	uint8 LpE;
	uint8 SLB;
	uint8 SLE;
	NodePointIT NodePoints[25];
};
struct InstrumentHeaderIT
{
	int32 IMPI;
	char DOSName [12];
	uint8 zero;
	uint8 NNA;
	uint8 DCT;
	uint8 DCA;
	uint16 FadeOut;
	uint8 PPS;
	uint8 PPC;
	uint8 GbV;
	uint8 DfP;
	uint8 RV ;
	uint8 RP ;
	uint16 TrkVers;
	uint8 NoS;
	uint8 x0;
	char Name [26];
	uint8 IFC;
	uint8 IFR;
	uint8 MCh;
	uint8 MPr;
	uint16 MIDIBnk;
	uint8 NoteSample [240];
	EnvelopeHeaderIT volume;
	EnvelopeHeaderIT panning;
	EnvelopeHeaderIT pitch;
	uint8  x[7];
};
struct SampleHeaderIT
{
	int32 IMPS;
	char DOSName [12];
	uint8 zero;
	uint8 GvL;
	uint8 Flg;
	uint8 Vol;
	char Name [26];
	uint8 Convert;
	uint8 DfP;
	uint32  	Length        ;
	uint32  LoopBegin    ;
	uint32  LoopEnd      ;
	uint32 C5Speed       ;
	uint32 SusLoopBegin ;
	uint32  SusLoopEnd   ;
	uint32  SamplePointer ;
	uint8 ViS;
	uint8 ViD;
	uint8 ViR;
	uint8 ViT;
};
struct SampleFileHeaderIT
{
	uint32 filesize;
	uint16 date;
	uint16 time;
	uint8 fmt;
};
struct NoteIT
{
	uint8 note,ins,volpan,cmd,inf;
};
#pragma pack(pop)

//Constructor
CResHandlerIT::CResHandlerIT() : CIwResHandler("it", TUNE4AIRPLAY_RESTYPE_TUNE)
{
	SetName("IT");
}

//Destructor
CResHandlerIT::~CResHandlerIT()
{
}
CIwArray<int16> DecompressIT16(s3eFile* pFile, uint32 dstSize)
{
	CIwArray<int16> res;
	int16 last = 0;
	int32 x,y,needbits,havebits,new_count=0;
	uint16 bits = 17;
	uint16 bufbits = 0;
	uint8 buf;
	uint16 incnt;
	s3eFileRead(&incnt,sizeof(incnt),1,pFile);
	
	
	while (res.size() < dstSize)
	{
		needbits=new_count?4:bits;
		x=havebits=0;

		while (needbits) {
			// feed buffer 
			if (!bufbits) {
				if(incnt)
				{
					if (s3eFileRead(&buf,sizeof(buf),1,pFile) != 1)
						buf = 0;
					incnt--;
				}
				else
				{
					buf = 0;
					return res;
				}
				bufbits=8;
			}
			// get as many bits as necessary 
			y = needbits<bufbits?needbits:bufbits;
			x|= (buf & ((1<<y)- 1))<<havebits;
			buf>>=y;
			bufbits-=(uint16)y;
			needbits-=(uint16)y;
			havebits+=(uint16)y;
		}

		if (new_count) {
			new_count = 0;
			if (++x >= bits)
				x++;
			bits = (uint16)x;
			continue;
		}
		if (bits<7) {
			if (x==(1<<(bits-1))) {
				new_count = 1;
				continue;
			}
		}
		
		else if (bits<17) {
			y=(0xffff>>(17-bits))-8;
			if ((x>y)&&(x<=y+16)) {
				if ((x-=y)>=bits)
					x++;
				bits = (uint16)x;
				continue;
			}
		}
		else if (bits<18) {
			if (x>=0x10000) {
				bits=(uint16)(x-0x10000+1);
				continue;
			}

		} else {
			// error in compressed data... 
			IwAssertMsg(SOUND,false,("MMERR_ITPACK_INVALID_DATA"));
			//_mm_errno=MMERR_ITPACK_INVALID_DATA;
			return res;
		}

		if (bits<16) // extend sign 
			x = ((int16)(x <<(16-bits))) >> (16-bits);
		res.push_back( (last+=x)); // convert to 16 bit 
	}

	return res;
}
CIwArray<int16> DecompressIT8(s3eFile* pFile, uint32 dstSize)
{
	CIwArray<int16> res;
	uint16 x,y,needbits,havebits,new_count=0;
	uint16 bits = 9;
	uint16 bufbits = 0;
	uint8 buf;
	int16 last = 0;
	uint16 incnt;
	s3eFileRead(&incnt,sizeof(incnt),1,pFile);
	
	while (res.size() < dstSize)
	{
		needbits=new_count?3:bits;
		x=havebits=0;

		while (needbits) {
			// feed buffer 
			if (!bufbits) {
				if(incnt)
				{
					if (s3eFileRead(&buf,sizeof(buf),1,pFile) != 1)
						buf = 0;
					incnt--;
				}
				else
				{
					buf = 0;
					return res;
				}
				bufbits=8;
			}
			// get as many bits as necessary 
			y = needbits<bufbits?needbits:bufbits;
			x|= (buf & ((1<<y)- 1))<<havebits;
			buf>>=y;
			bufbits-=y;
			needbits-=y;
			havebits+=y;
		}

		if (new_count) {
			new_count = 0;
			if (++x >= bits)
				x++;
			bits = x;
			continue;
		}
		if (bits<7) {
			if (x==(1<<(bits-1))) {
				new_count = 1;
				continue;
			}
		}
		else if (bits<9) {
			y = (0xff >> (9-bits)) - 4;
			if ((x>y)&&(x<=y+8)) {
				if ((x-=y)>=bits)
					x++;
				bits = x;
				continue;
			}
		}
		else if (bits<10) {
			if (x>=0x100) {
				bits=x-0x100+1;
				continue;
			}
		} else {
			// error in compressed data... 
			IwAssertMsg(SOUND,false,("MMERR_ITPACK_INVALID_DATA"));
			//_mm_errno=MMERR_ITPACK_INVALID_DATA;
			return res;
		}

		if (bits<8) // extend sign 
			x = ((int8)(x <<(8-bits))) >> (8-bits);
		res.push_back( (last+=x) << 8 ); // convert to 16 bit 
	}


	return res;
}

CIwResource* CResHandlerIT::Build(const CIwStringL& pathname)
{
	IW_CALLSTACK("CResHandlerIT::Build")

	Ct4aTune* tune = 0;	// Object to return

	s3eFile* pFile = IwFileOpenPrefixed(pathname.c_str(), "rb");
	if (!pFile)
	{
		IwAssertMsg(SOUND, false, ("Invalid file %s", pathname.c_str()));
		return 0;
	}

	FileHeaderIT header;
	if (s3eFileRead(&header,sizeof(header),1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid header %s", pathname.c_str()));
		return 0;
	}
	if (header.IMPM != 0x4D504D49)
	{
		IwAssertMsg(SOUND, false, ("Invalid magic %08X", header.IMPM));
		return 0;
	}
	bool useInstruments = 0!=(header.Flags&4);
	tune = new Ct4aTune();
	tune->SetSpeed(header.IS);
	tune->SetTempo(header.IT);
	for (uint32 i=0; i<header.OrdNum; ++i)
	{
		uint8 order;
		if ((s3eFileRead(&order,sizeof(order),1,pFile) != 1) || (order >= 200 && order < 254))
		{
			IwAssertMsg(SOUND, false, ("Invalid order %d", order));
			delete tune;
			return 0;
		}
		if (order == 255)
			break;
		if (order != 254)
			tune->PushPatternIntex((uint32)order);
	}
	CIwArray<uint32> offsetInstr;
	if (header.InsNum > 0)
	{
		offsetInstr.resize(header.InsNum);
		if ((s3eFileRead(&offsetInstr[0],sizeof(uint32)*header.InsNum,1,pFile) != 1))
		{
			IwAssertMsg(SOUND, false, ("Invalid instruments offsets"));
			delete tune;
			return 0;
		}
	}

	CIwArray<uint32> offsetSamples;
	if (header.SmpNum > 0)
	{
		offsetSamples.resize(header.SmpNum);
		if ((s3eFileRead(&offsetSamples[0],sizeof(uint32)*header.SmpNum,1,pFile) != 1))
		{
			IwAssertMsg(SOUND, false, ("Invalid samples offsets"));
			delete tune;
			return 0;
		}
	}

	CIwArray<uint32> offsetPatterns;
	if (header.PatNum > 0)
	{
		offsetPatterns.resize(header.PatNum);
		if ((s3eFileRead(&offsetPatterns[0],sizeof(uint32)*header.PatNum,1,pFile) != 1))
		{
			IwAssertMsg(SOUND, false, ("Invalid patterns offsets"));
			delete tune;
			return 0;
		}
	}

	for (uint32 i=0; i<header.InsNum; ++i)
	{
		s3eFileSeek(pFile, (int32)offsetInstr[i], S3E_FILESEEK_SET);
		if (header.Cmwt < 0x200)
		{
			InstrumentHeaderOldIT ins;
			if ((s3eFileRead(&ins,sizeof(ins),1,pFile) != 1))
			{
				IwAssertMsg(SOUND, false, ("Invalid instrument"));
				delete tune;
				return 0;
			}
			if (ins.IMPI != 0x49504D49)
			{
				IwAssertMsg(SOUND, false, ("Invalid instrument magic 0x%08X", ins.IMPI));
				delete tune;
				return 0;
			}
			Ct4aInstrumentIT* instrument = new Ct4aInstrumentIT();
			instrument->SetFadeOut(IW_FIXED(ins.FadeOut));
			for (uint i=0; i<120; ++i)
			{
				uint16 s = ins.NoteSample[i*2+1];
				if (s > 0)
					instrument->SetNoteSample(ins.NoteSample[i*2],s-1);
			}
			tune->PushInstrument(instrument);
		}
		else
		{
			InstrumentHeaderIT ins;
			if ((s3eFileRead(&ins,sizeof(ins),1,pFile) != 1))
			{
				IwAssertMsg(SOUND, false, ("Invalid instrument"));
				delete tune;
				return 0;
			}
			if (ins.IMPI != 0x49504D49)
			{
				IwAssertMsg(SOUND, false, ("Invalid instrument magic 0x%08X", ins.IMPI));
				delete tune;
				return 0;
			}
			Ct4aInstrumentIT* instrument = new Ct4aInstrumentIT();
			instrument->SetFadeOut(IW_FIXED(ins.FadeOut));

			if (0 != (ins.volume.Flg & 1))
			{
				EnvelopeHeaderIT& srcEnv = ins.volume;
				CSampledEnvelope* e = instrument->GetVolumeEnvelope();
				for (uint i=0; i<srcEnv.Num; ++i)
					e->AddNode(srcEnv.NodePoints[i].tickNumber,IW_FIXED(srcEnv.NodePoints[i].Y/64.0f));
				if (0 != (srcEnv.Flg & 2))
					e->SetVolumeLoop(srcEnv.LpB,srcEnv.LpE);
				if (0 != (srcEnv.Flg & 4))
					e->SetSustainLoop(srcEnv.SLB,srcEnv.SLE);
			}
			if (0 != (ins.pitch.Flg & 1))
			{
				EnvelopeHeaderIT& srcEnv = ins.pitch;
				CSampledEnvelope* e = instrument->GetPitchEnvelope();
				for (uint i=0; i<srcEnv.Num; ++i)
					e->AddNode(srcEnv.NodePoints[i].tickNumber,IW_FIXED((float)srcEnv.NodePoints[i].Y/32.0f-1.0f));
				if (0 != (srcEnv.Flg & 2))
					e->SetVolumeLoop(srcEnv.LpB,srcEnv.LpE);
				if (0 != (srcEnv.Flg & 4))
					e->SetSustainLoop(srcEnv.SLB,srcEnv.SLE);
			}
			if (0 != (ins.panning.Flg & 1))
			{
				EnvelopeHeaderIT& srcEnv = ins.panning;
				CSampledEnvelope* e = instrument->GetPanningEnvelope();
				for (uint i=0; i<srcEnv.Num; ++i)
					e->AddNode(srcEnv.NodePoints[i].tickNumber,IW_FIXED((float)srcEnv.NodePoints[i].Y/32.0f-1.0f));
				if (0 != (srcEnv.Flg & 2))
					e->SetVolumeLoop(srcEnv.LpB,srcEnv.LpE);
				if (0 != (srcEnv.Flg & 4))
					e->SetSustainLoop(srcEnv.SLB,srcEnv.SLE);
			}
			for (uint i=0; i<120; ++i)
			{
				uint16 s = ins.NoteSample[i*2+1];
				if (s > 0)
					instrument->SetNoteSample(ins.NoteSample[i*2],s-1);
			}
			tune->PushInstrument(instrument);
		}
	}
	for (uint32 i=0; i<header.SmpNum; ++i)
	{
		s3eFileSeek(pFile, (int32)offsetSamples[i], S3E_FILESEEK_SET);
		SampleHeaderIT smp;
		if ((s3eFileRead(&smp,sizeof(smp),1,pFile) != 1))
		{
			IwAssertMsg(SOUND, false, ("Invalid sample"));
			delete tune;
			return 0;
		}
		if (smp.IMPS != 0x53504D49)
		{
			IwAssertMsg(SOUND, false, ("Invalid sample magic 0x%08X", smp.IMPS));
			delete tune;
			return 0;
		}
		//SampleFileHeaderIT fileHeader;
		bool smpFileHeader = 0 != (smp.Flg & (uint8)(1<<0));
		/*if (smpFileHeader)
		{
			if ((s3eFileRead(&fileHeader,sizeof(fileHeader),1,pFile) != 1))
			{
				IwAssertMsg(SOUND, false, ("Invalid sample"));
				delete tune;
				return 0;
			}
		}*/
		bool smp16 = 0 != (smp.Flg & (uint8)(1<<1));
		bool smpStereo = 0 != (smp.Flg & (1<<2));
		bool smpCompressed = 0 != (smp.Flg & (1<<3));
		bool smpUseLoop = 0 != (smp.Flg & (1<<4));
		bool smpUseSustainLoop = 0 != (smp.Flg & (1<<5));
		bool smpPingPongLoop = 0 != (smp.Flg & (1<<6));
		bool smpPingPongSusLoop = 0 != (smp.Flg & (1<<7));
		bool smpSigned = 0 != (smp.Convert & (1<<0));
		bool smpMotorola = 0 != (smp.Convert & (1<<1));
		bool smpDelta = 0 != (smp.Convert & (1<<2));
		bool smpByteDeltaValues = 0 != (smp.Convert & (1<<3));
		bool smpTxWave12Bit = 0 != (smp.Convert & (1<<4));
		bool smpLeftRightAll = 0 != (smp.Convert & (1<<5));

		s3eFileSeek(pFile, (int32)smp.SamplePointer, S3E_FILESEEK_SET);

		
		Ct4aSampleIT* sample = new Ct4aSampleIT();
		uint32 sampleSize = (smpStereo?2:1)*(smp16?2:1);
		sample->SetC5Speed(smp.C5Speed);// /sampleSize);
		sample->SetVolume(IW_SFIXED(smp.GvL/64.0f));

		if (smpUseLoop && smpPingPongLoop)
			sample->SetVolumeLoop(T4A_LOOP_PINGPONG,smp.LoopBegin,smp.LoopEnd);
		if (smpUseLoop && !smpPingPongLoop)
			sample->SetVolumeLoop(T4A_LOOP_FORWARDS,smp.LoopBegin,smp.LoopEnd);
		if (smpUseSustainLoop && smpPingPongSusLoop)
			sample->SetSustainLoop(T4A_LOOP_PINGPONG,smp.SusLoopBegin,smp.SusLoopEnd);
		if (smpUseSustainLoop && !smpPingPongSusLoop)
			sample->SetSustainLoop(T4A_LOOP_FORWARDS,smp.SusLoopBegin,smp.SusLoopEnd);

		if (smpCompressed)
		{
			if (smp16)
			{
				CIwArray<int16> samples = DecompressIT16(pFile, smp.Length);
				sample->LoadSamples(&samples[0],smp.Length,true,smpSigned,smpStereo);
			}
			else
			{
				CIwArray<int16> samples = DecompressIT8(pFile, smp.Length);
				sample->LoadSamples(&samples[0],smp.Length,true,smpSigned,smpStereo);
			}

		}
		else
		{
			CIwArray<uint8> samples;
			samples.resize(smp.Length*sampleSize);
			if (samples.size() > 0)
			{
				if (s3eFileRead(&samples[0],samples.size(),1,pFile) != 1)
				{
					s3eFileError err = s3eFileGetError();
					if (err != S3E_FILE_ERR_EOF)
					{
						IwAssertMsg(SOUND, false, ("Can't read sample: %d", err));
						delete tune;
						return 0;
					}
				}
			}
			if (samples.size() > 0)
				sample->LoadSamples(&samples[0],smp.Length,smp16,smpSigned,smpStereo);
		}

		tune->PushSample(sample);
	}
	for (uint32 i=0; i<header.PatNum; ++i)
	{
		s3eFileSeek(pFile, (int32)offsetPatterns[i], S3E_FILESEEK_SET);
		uint16 length;
		uint16 rows;
		uint32 buf;
		if ((s3eFileRead(&length,sizeof(length),1,pFile) != 1) ||
			(s3eFileRead(&rows,sizeof(rows),1,pFile) != 1) ||
			(s3eFileRead(&buf,sizeof(buf),1,pFile) != 1))
		{
			IwAssertMsg(SOUND, false, ("Invalid pattern"));
			delete tune;
			return 0;
		}

		int32 patternEnd = s3eFileTell(pFile)+(int32)length;
		Ct4aPattern* pat = new Ct4aPattern();
		tune->PushPattern(pat);
		pat->SetNumRows(rows);
		uint32 row = 0;


		uint8 channelvariable;
		uint8 maskvariable[256];
		NoteIT lastNotes[256];
		memset(&lastNotes[0],0,sizeof(NoteIT)*256);
		while ((s3eFileTell(pFile) < patternEnd) && (row < rows))
		{
			s3eFileRead(&channelvariable,sizeof(channelvariable),1,pFile);
			if (channelvariable == 0)
			{
				++row;
				continue;
			}
			//CCommand cmd;
			uint8 channel = (channelvariable-1) & 63;
			//cmd.SetChannel(channel);
			if(channelvariable & 128)
				s3eFileRead(&maskvariable[channel],sizeof(maskvariable[channel]),1,pFile);
			if (0 != (maskvariable[channel] & 1))
			{
				s3eFileRead(&lastNotes[channel].note,sizeof(lastNotes[channel].note),1,pFile);
			}
			if (0 != (maskvariable[channel] & 2))
			{
				s3eFileRead(&lastNotes[channel].ins,sizeof(lastNotes[channel].ins),1,pFile);
			}
			if (0 != (maskvariable[channel] & 4))
			{
				s3eFileRead(&lastNotes[channel].volpan,sizeof(lastNotes[channel].volpan),1,pFile);
			}
			if (0 != (maskvariable[channel] & 8))
			{
				s3eFileRead(&lastNotes[channel].cmd,sizeof(lastNotes[channel].cmd),1,pFile);
				s3eFileRead(&lastNotes[channel].inf,sizeof(lastNotes[channel].inf),1,pFile);
			}
			if ((0 != (maskvariable[channel] & 16)) || (0 != (maskvariable[channel] & 1)))
			{
				if (lastNotes[channel].note != 255)
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_NOTE,lastNotes[channel].note));
				else
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_NOTEOFF));
			}
			if ((0 != (maskvariable[channel] & 32)) || (0 != (maskvariable[channel] & 2)))
			{
				int32 ins = lastNotes[channel].ins-1;
				if (useInstruments)
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_INSTRUMENT,ins));
				else
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_SAMPLE,ins));
			}
			if ((0 != (maskvariable[channel] & 64)) || (0 != (maskvariable[channel] & 4)))
			{
				uint8 volpan = lastNotes[channel].volpan;
				CPatternRow* rowPtr = pat->GetRow(row);
				if ((volpan >=0) && (volpan <= 64))
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME,IW_FIXED((int32)volpan/64.0f)));
				else if(volpan==65) // fine volume slide up (65-74) - A0 case 
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME_SLIDE,IW_FIXED(0/64.0f)));
				else if(volpan<=74)	{ /* fine volume slide up (65-74) - general case */
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME_SLIDE,IW_FIXED((int8)( 0x0f+((volpan-65)<<4) )/64.0f)));
				} else if(volpan==75)	/* fine volume slide down (75-84) - B0 case */
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME_SLIDE,IW_FIXED(0/64.0f)));
				else if(volpan<=84) {	/* fine volume slide down (75-84) - general case*/
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME_SLIDE,IW_FIXED((int8)( (0xf0+(volpan-75)) )/64.0f)));
				} else if(volpan<=94)	/* volume slide up (85-94) */
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME_SLIDE,IW_FIXED((int8)( (((volpan-85)<<4)) )/64.0f)));
				else if(volpan<=104)/* volume slide down (95-104) */
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_VOLUME_SLIDE,IW_FIXED((int8)( ((volpan-95)) )/64.0f)));
				else if(volpan<=114)/* pitch slide down (105-114) */
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_PITCH_SLIDE,IW_FIXED( -((int32)volpan-105) )));
				else if(volpan<=124)/* pitch slide up (115-124) */
					rowPtr->PushCommand(CCommand(channel,T4A_CMD_PITCH_SLIDE,IW_FIXED( (int32)volpan-115 )));
				else if(volpan<=127) { /* crap */
					//IwAssertMsg(SOUND, false, ("Invalid pattern command"));
				} 
				else if ((volpan >=128) && (volpan <= 128+64))
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_PAN,IW_FIXED(((int)volpan-128-32)/32.0f)));
				//else if(volpan<=202)/* portamento to note */
				//	UniVolEffect(VOL_PORTAMENTO,portatable[volpan-193]);
				//else if(volpan<=212)/* vibrato */
				//	UniVolEffect(VOL_VIBRATO,(volpan-203));

				
			}
			if ((0 != (maskvariable[channel] & 128)) || (0 != (maskvariable[channel] & 8)))
			{
				uint8 cmd = lastNotes[channel].cmd;
				switch (cmd)
				{
				case 0x01:
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_SPEED, lastNotes[channel].inf));
					break;
				case 0x02:
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_JUMP_TO_ORDER, lastNotes[channel].inf));
					break;
				case 0x03:
					//if ((flags & S3MIT_OLDSTYLE) && !(flags & S3MIT_IT))
					//	UniPTEffect(0xd,(inf>>4)*10+(inf&0xf));
					//else
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_JUMP_TO_ROW, lastNotes[channel].inf));
					break;
				case 0x14:
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_TEMPO, lastNotes[channel].inf));
					break;
				case 0x11:
					pat->GetRow(row)->PushCommand(CCommand(channel,T4A_CMD_RETRIGGER, lastNotes[channel].inf));
					break;
				default:
					cmd = cmd;//for breakpoint
					break;
				};
				//cmd.SetCmd(lastNotes[channel].cmd,lastNotes[channel].inf);
			}
			//pat->GetRow(row)->PushCommand(cmd);
		}
	}
	s3eFileClose(pFile);
	return tune;
}
#endif