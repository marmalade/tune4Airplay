#include <IwFile.h>
#include "ResHandlerMOD.h"
#include "t4aSampleIT.h"

using namespace Tune4Airplay;

#ifdef IW_BUILD_RESOURCES
IW_MANAGED_IMPLEMENT(CResHandlerMOD)

#pragma pack(push)
#pragma pack(1)
struct SampleHeaderMOD
{
	char samplename[22];		/* 22 in module, 23 in memory */
	uint16 length;
	uint8 finetune;
	uint8 volume;
	uint16 reppos;
	uint16 replen;
};
struct FileHeaderMOD
{
	char songname[20];			/* the songname.. 20 in module, 21 in memory */
	SampleHeaderMOD samples[31];		/* all sampleinfo */
	uint8 songlength;			/* number of patterns used */
	uint8 magic1;				/* should be 127 */
	uint8 positions[128];		/* which pattern to play at pos */
	uint8 magic2[4];			/* string "M.K." or "FLT4" or "FLT8" */
};
struct NoteHeaderMOD {
	uint8 a, b, c, d;
};

#pragma pack(pop)

CResHandlerMOD::CResHandlerMOD() : CIwResHandler("mod", TUNE4AIRPLAY_RESTYPE_TUNE)
{
	SetName("MOD");
}

CResHandlerMOD::~CResHandlerMOD()
{
}

CIwResource* CResHandlerMOD::Build(const CIwStringL& pathname)
{
	IW_CALLSTACK("CResHandlerMOD::Build")

	uint16 npertab[7 * 12] = {
		/* Octaves 6 -> 0 */
		/* C    C#     D    D#     E     F    F#     G    G#     A    A#     B */
		0x6b0,0x650,0x5f4,0x5a0,0x54c,0x500,0x4b8,0x474,0x434,0x3f8,0x3c0,0x38a,
		0x358,0x328,0x2fa,0x2d0,0x2a6,0x280,0x25c,0x23a,0x21a,0x1fc,0x1e0,0x1c5,
		0x1ac,0x194,0x17d,0x168,0x153,0x140,0x12e,0x11d,0x10d,0x0fe,0x0f0,0x0e2,
		0x0d6,0x0ca,0x0be,0x0b4,0x0aa,0x0a0,0x097,0x08f,0x087,0x07f,0x078,0x071,
		0x06b,0x065,0x05f,0x05a,0x055,0x050,0x04b,0x047,0x043,0x03f,0x03c,0x038,
		0x035,0x032,0x02f,0x02d,0x02a,0x028,0x025,0x023,0x021,0x01f,0x01e,0x01c,
		0x01b,0x019,0x018,0x016,0x015,0x014,0x013,0x012,0x011,0x010,0x00f,0x00e
	};

	Ct4aTune* tune = 0;	// Object to return

	s3eFile* pFile = IwFileOpenPrefixed(pathname.c_str(), "rb");
	if (!pFile)
	{
		IwAssertMsg(SOUND, false, ("Invalid file %s", pathname.c_str()));
		return 0;
	}

	FileHeaderMOD header;
	if (s3eFileRead(&header,sizeof(header),1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid header %s", pathname.c_str()));
		return 0;
	}
	if (header.songlength > 128) { header.songlength = 128; }

	tune = new Ct4aTune();
	uint8 maxPat=0;
	uint32 samplesPresent = 31;
	for (uint32 i=0; i<header.songlength; ++i)
	{
		if (maxPat <= header.positions[i])
			maxPat = header.positions[i]+1;
		tune->PushPatternIntex(header.positions[i]);
	}
	bool modtype, trekker = 0;
	uint16 channels = 4;
	if ((header.magic2[0] == 'M') && 
		(header.magic2[1] == '.') && 
		(header.magic2[2] == 'K') && 
		(header.magic2[3] == '.'))
	{
		modtype = 0;
		trekker = 0;
		channels = 4;
	}
	else if (((header.magic2[0] == 'F') && 
		(header.magic2[1] == 'L') && 
		(header.magic2[2] == 'T') && 
		(header.magic2[3] == '4')) ||
		((header.magic2[0] == 'E') && 
		(header.magic2[1] == 'X') && 
		(header.magic2[2] == 'O') && 
		(header.magic2[3] == '4')))
	{
		modtype = 1;
		trekker = 1;
		channels = 4;
	}
	else if (((header.magic2[0] == 'F') && 
		(header.magic2[1] == 'L') && 
		(header.magic2[2] == 'T') && 
		(header.magic2[3] == '8')) ||
		((header.magic2[0] == 'E') && 
		(header.magic2[1] == 'X') && 
		(header.magic2[2] == 'O') && 
		(header.magic2[3] == '8')))
	{
		modtype = 1;
		trekker = 1;
		channels = 8;
	}
	else if ((header.magic2[0] == 'M') && 
		(header.magic2[1] == '!') && 
		(header.magic2[2] == 'K') && 
		(header.magic2[3] == '!'))
	{
		modtype = 0;
		trekker = 0;
		channels = 4;
	}
	else if ((header.magic2[1] == 'C') && 
		(header.magic2[2] == 'H') && 
		(header.magic2[3] == 'N'))
	{
		modtype = 1;
		trekker = 0;
		channels = header.magic2[0]-'0';
	}
	else
	{
		samplesPresent = 15;
		int32 pos = s3eFileTell(pFile);
		s3eFileSeek(pFile,pos-4,S3E_FILESEEK_SET);
	}

	for (uint32 patternIndex=0; patternIndex<maxPat; ++patternIndex)
	{
		uint8 lasteffect = 0x10;

		Ct4aPattern* pat = new Ct4aPattern();
		pat->SetNumRows(64);
		int lastnote=0;
		for (uint32 row=0; row<64; ++row)
		{
			for (uint32 col=0; col<channels; ++col)
			{
				NoteHeaderMOD noteHeader;
				if (s3eFileRead(&noteHeader,sizeof(noteHeader),1,pFile) != 1)
				{
					IwAssertMsg(SOUND, false, ("Invalid MOD note %d at row %d at pattern %d", col,row,patternIndex));
					delete tune;
					return 0;
				}
				uint16 instrument = (noteHeader.a & 0x10) | (noteHeader.c >> 4);
				uint16 period = (((uint16)noteHeader.a & 0xf) << 8) + noteHeader.b;
				uint16 effect = noteHeader.c & 0xf;
				uint16 effdat = noteHeader.d;

				int16 note = 0;
				if (period) {
					for (note = 0; note < 7 * 12; note++)
						if (period >= npertab[note])
							break;
					if (note == 7 * 12)
						note = 0;
					else
						note++;
				}

				if (instrument) {
					/* if instrument does not exist, note cut */
					if ((instrument > 31) || (!header.samples[instrument - 1].length)) {
						pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_NOTEOFF));
						if (effect == 0xc)
							effect = effdat = 0;
					} else {
						/* Protracker handling */
						if (!modtype) {
							/* if we had a note, then change instrument... */
							if (note)
							{
								pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_SAMPLE,instrument-1));
							}
							/* ...otherwise, only adjust volume... */
							else {
								/* ...unless an effect was specified, which forces a new
								   note to be played */
								if (effect || effdat) {
									pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_SAMPLE,instrument-1));
									note = lastnote;
								} 
								else
								{
									pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_VOLUME,IW_FIXED( (header.samples[instrument -1].volume & 0x7f)/127.0f )));
									//UniPTEffect(0xc,
									//			header.samples[instrument -
									//						1].volume & 0x7f);
								}
							}
						} else {
							/* Fasttracker handling */
							pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_SAMPLE,instrument-1));
							if (!note)
								note = lastnote;
						}
					}
				}
				if (note) {
					pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_NOTE,note+2*12-1));
					//UniNote(note + 2 * OCTAVE - 1);
					lastnote = note;
				}

				/* Convert pattern jump from Dec to Hex */
				if (effect == 0xd)
					effdat = (((effdat & 0xf0) >> 4) * 10) + (effdat & 0xf);

				/* Volume slide, up has priority */
				if ((effect == 0xa) && (effdat & 0xf) && (effdat & 0xf0))
					effdat &= 0xf0;

				/* Handle ``heavy'' volumes correctly */
				if ((effect == 0xc) && (effdat > 0x40))
					effdat = 0x40;
	
				/* An isolated 100, 200 or 300 effect should be ignored (no
				   "standalone" porta memory in mod files). However, a sequence such
				   as 1XX, 100, 100, 100 is fine. */
				if ((!effdat) && ((effect == 1)||(effect == 2)||(effect ==3)) &&
					(lasteffect < 0x10) && (effect != lasteffect))
					effect = 0;

				if (effect || effdat)
				{
					switch (effect)
					{
					case 0: //arpeggio
						break;
					case 1: //porta up
						break;
					case 2: //porta down
						break;
					case 3: //porta to note
						break;
					case 4: //vibrato
						break;
					case 5: //dual effect 3+A
						break;
					case 6: //dual effect 4+A
						break;
					case 7: //tremolo
						break;
					case 8: //pan
						break;
					case 9: //sample offset
						break;
					case 10: //volume slide
						{
						iwfixed up = effdat>>4;
						iwfixed down = effdat&0x0F;
						if (up && !down)
							pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_VOLUME_SLIDE, IW_FIXED( up/64.0f )));
						if (!up && down)
							pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_VOLUME_SLIDE, IW_FIXED( -down/64.0f )));
						}
						break;
					case 11: //pattern jump
						pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_JUMP_TO_ORDER,effdat));
						break;
					case 12: //set volume
						pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_VOLUME, IW_FIXED( effdat/64.0f )));
						break;
					case 13: //pattern break
						pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_JUMP_TO_ROW,(effdat>>4)*10+(effdat&0x0F) ));
						break;
					case 14: //extended effects
						break;
					case 15: //set speed
						pat->GetRow(row)->PushCommand(CCommand(col,T4A_CMD_TEMPO,effdat));
						break;
					}
				}
			}
		}
		tune->PushPattern(pat);
	}
	for (uint32 i=0; i<samplesPresent; ++i)
	{
		Ct4aSampleIT* smp = new Ct4aSampleIT();
		if (header.samples[i].length > 0)
		{
			CIwArray<int8> buf;
			uint16 l= header.samples[i].length;
			l = (l>>8)|(l<<8);
			buf.resize(l*2);
			if (s3eFileRead(&buf[0],buf.size(),1,pFile) != 1)
			{
				IwAssertMsg(SOUND, false, ("Invalid MOD sample %d", i));
				delete tune;
				return 0;
			}
			smp->LoadSamples(&buf[2],buf.size()-2,false,true,false);
		}
		tune->PushSample(smp);
	}
	s3eFileClose(pFile);
	return tune;
}
#endif