#include <IwFile.h>
#include "ResHandlerWAV.h"
#include "t4aSampleIT.h"

using namespace Tune4Airplay;

#ifdef IW_BUILD_RESOURCES
IW_MANAGED_IMPLEMENT(CResHandlerWAV)

#pragma pack(push)
#pragma pack(1)
namespace Tune4Airplay {
	struct ChunkHeader
	{
		uint32 ChunkID;
		uint32 ChunkSize;
	};
	struct FileHeaderWAV
	{
		ChunkHeader	RIFF;
		uint32	WAVE;
	};
	struct LoopHeaderWAV
	{
		uint32 CuePointID;
		uint32 Type;
		uint32 Start;
		uint32 End;
		uint32 Fraction;
		uint32 PlayCount;

	};
	struct SmplHeaderWAV
	{
		uint32 Manufacturer;
		uint32 Product;
		uint32 SamplePeriod;
		uint32 MIDIUnityNote;
		uint32 MIDIPitchFraction;
		uint32 SMPTEFormat;
		uint32 SMPTEOffset;
		uint32 NumSampleLoops;
		uint32 SamplerData;
		LoopHeaderWAV Loops [1];
	};
	struct FmtHeaderWAV
	{
		uint16 CompressionCode;
		uint16 NumberOfChannels;
		uint32 SampleRate;
		uint32 AverageBytesPerSecond;
		uint16 BlockAlign;
		uint16 SignificantBitsPerSample;
		uint16 ExtraFormatBytes;
	};
}
#pragma pack(pop)

//Constructor
CResHandlerWAV::CResHandlerWAV() : CIwResHandler("wav", TUNE4AIRPLAY_RESTYPE_SAMPLEIT)
{
	SetName("WAV");
}

//Destructor
CResHandlerWAV::~CResHandlerWAV()
{
}
bool CResHandlerWAV::Build(s3eFile* pFile, Ct4aSampleIT* sample)
{
	FileHeaderWAV header;

	if (s3eFileRead(&header,sizeof(header),1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid WAV header"));
		return false;
	}
	if (header.RIFF.ChunkID != 0x46464952)
	{
		IwAssertMsg(SOUND, false, ("Invalid magic 'RIFF' %08X", header.RIFF.ChunkID));
		return false;
	}
	if (header.WAVE != 0x45564157)
	{
		IwAssertMsg(SOUND, false, ("Invalid magic 'WAVE' %08X", header.RIFF.ChunkID));
		return false;
	}
	uint32 end = s3eFileTell(pFile)-4+header.RIFF.ChunkSize;
	uint32 dataPos = 0;
	uint32 dataSize = 0;
	ChunkHeader chunk;
	CIwArray<uint8> fmtBuf;
	FmtHeaderWAV* fmt = 0;
	CIwArray<uint8> smplBuf;
	SmplHeaderWAV* smpl = 0;
	for (;end>(uint32)s3eFileTell(pFile);)
	{
		if (s3eFileRead(&chunk,sizeof(chunk),1,pFile) != 1)
		{
			IwAssertMsg(SOUND, false, ("Invalid WAV chunk header"));
			return false;
		}
		if (chunk.ChunkID == 0x20746d66) //fmt
		{
			fmtBuf.resize(chunk.ChunkSize);
			if (s3eFileRead(&fmtBuf[0],fmtBuf.size(),1,pFile) != 1)
			{
				IwAssertMsg(SOUND, false, ("Invalid WAV fmt"));
				return false;
			}
			fmt = (FmtHeaderWAV*)&fmtBuf[0];
		}
		else if (chunk.ChunkID == 0x6C706D73) //smpl
		{
			smplBuf.resize(chunk.ChunkSize);
			if (s3eFileRead(&smplBuf[0],smplBuf.size(),1,pFile) != 1)
			{
				IwAssertMsg(SOUND, false, ("Invalid WAV smpl"));
				return false;
			}
			smpl = (SmplHeaderWAV*)&smplBuf[0];
		}
		else if (chunk.ChunkID == 0x61746164) //data
		{
			dataPos = s3eFileTell(pFile);
			dataSize = chunk.ChunkSize;
			s3eFileSeek(pFile,chunk.ChunkSize,S3E_FILESEEK_CUR);
		}
		else
		{
			s3eFileSeek(pFile,chunk.ChunkSize,S3E_FILESEEK_CUR);
		}
	}

	switch (fmt->CompressionCode)
	{
	case 0:
	default:
		IwAssertMsg(SOUND, false, ("WAV compression %d not supported", fmt->CompressionCode));
		return false;
	case 1:
		s3eFileSeek(pFile, dataPos, S3E_FILESEEK_SET);
		ReadUncompressed(fmt, smpl, dataSize,pFile, sample);
		s3eFileClose(pFile);
		return true;
	case 3:
		s3eFileSeek(pFile, dataPos, S3E_FILESEEK_SET);
		ReadIEEE(fmt, smpl, dataSize,pFile, sample);
		s3eFileClose(pFile);
		return true;
	}
	return false;
}
CIwResource* CResHandlerWAV::Build(const CIwStringL& pathname)
{
	IW_CALLSTACK("CResHandlerWAV::Build")

	s3eFile* pFile = IwFileOpenPrefixed(pathname.c_str(), "rb");
	if (!pFile)
	{
		IwAssertMsg(SOUND, false, ("Invalid file %s", pathname.c_str()));
		return 0;
	}

	Ct4aSampleIT* sample = new Ct4aSampleIT();	// Object to return
	if (!Build(pFile,sample))
	{
		s3eFileClose(pFile);
		delete sample;
		return 0;
	}
	s3eFileClose(pFile);
	return sample;
}
CIwResource* CResHandlerWAV::ReadIEEE(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample)
{
	CIwArray<float> buf;
	buf.resize(size/4);
	if (s3eFileRead(&buf[0],size,1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid WAV data"));
		return 0;
	}
	CIwArray<int16> int16buf;
	for (uint32 i=0; i<buf.size();++i)
	{
		int16buf.push_back((int16)(buf[i]*32767));
	}
	sample->SetC5Speed(fmt->SampleRate);
	sample->LoadSamples(&int16buf[0],int16buf.size()/fmt->NumberOfChannels,true,true,fmt->NumberOfChannels>1);
	return sample;
}
CIwResource* CResHandlerWAV::ReadUncompressed8(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample)
{
	CIwArray<int8> buf;
	buf.resize(size);
	if (s3eFileRead(&buf[0],size,1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid WAV data"));
		return 0;
	}
	CIwArray<int16> int16buf;
	for (uint32 i=0; i<buf.size();++i)
	{
		int16buf.push_back(((int16)buf[i])*256);
	}
	sample->SetC5Speed(fmt->SampleRate);
	sample->LoadSamples(&int16buf[0],int16buf.size()/fmt->NumberOfChannels,true,true,fmt->NumberOfChannels>1);
	return sample;
}
CIwResource* CResHandlerWAV::ReadUncompressed16(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample)
{
	CIwArray<int16> int16buf;
	int16buf.resize(size/2);
	if (s3eFileRead(&int16buf[0],size,1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid WAV data"));
		return 0;
	}
	
	sample->SetC5Speed(fmt->SampleRate);
	sample->LoadSamples(&int16buf[0],int16buf.size()/fmt->NumberOfChannels,true,true,fmt->NumberOfChannels>1);
	return sample;
}
CIwResource* CResHandlerWAV::ReadUncompressed(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample)
{
	if (fmt->SignificantBitsPerSample == 8)
		return CResHandlerWAV::ReadUncompressed8(fmt, smpl, size, pFile, sample);
	if (fmt->SignificantBitsPerSample == 16)
		return CResHandlerWAV::ReadUncompressed16(fmt, smpl, size, pFile, sample);
	IwAssertMsg(SOUND, false, ("WAV SignificantBitsPerSample %d not supported", fmt->SignificantBitsPerSample));
	return 0;
}
#endif