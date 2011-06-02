#include "ResHandlerXM.h"
#include <IwFile.h>

using namespace Tune4Airplay;

#ifdef IW_BUILD_RESOURCES
IW_MANAGED_IMPLEMENT(CResHandlerXM)

#pragma pack(push)
#pragma pack(1)
struct FileHeaderXM
{
	char  id[17];          /* ID text: 'Extended module: ' */
	char  songname[21];    /* Module name */
	char  trackername[20]; /* Tracker name */
	uint16 version;         /* Version number */
	uint32 headersize;      /* Header size */
	uint16 songlength;      /* Song length (in patten order table) */
	uint16 restart;         /* Restart position */
	uint16 numchn;          /* Number of channels (2,4,6,8,10,...,32) */
	uint16 numpat;          /* Number of patterns (max 256) */
	uint16 numins;          /* Number of instruments (max 128) */
	uint16 flags;       
	uint16 tempo;           /* Default tempo */
	uint16 bpm;             /* Default BPM */
	uint8 orders[256];     /* Pattern order table  */
};
struct InstrumentHeaderXM
{
	uint32 size;     /* Instrument size */
	char  name[22]; /* Instrument name */
	uint8 type;     /* Instrument type (always 0) */
	uint16 numsmp;   /* Number of samples in instrument */
	uint32 ssize;
};
struct PatchHeaderXM
{
};
struct SampleHeaderXM
{
};
struct PatternHeaderXM
{
};

#pragma pack(pop)

CResHandlerXM::CResHandlerXM() : CIwResHandler("xm", TUNE4AIRPLAY_RESTYPE_TUNE)
{
	SetName("XM");
}

CResHandlerXM::~CResHandlerXM()
{
}
CIwResource* CResHandlerXM::Build(const CIwStringL& pathname)
{
	IW_CALLSTACK("CResHandlerXM::Build")

	Ct4aTune* tune = 0;	// Object to return

	s3eFile* pFile = IwFileOpenPrefixed(pathname.c_str(), "rb");
	if (!pFile)
	{
		IwAssertMsg(SOUND, false, ("Invalid file %s", pathname.c_str()));
		return 0;
	}

	FileHeaderXM header;
	if (s3eFileRead(&header,sizeof(header),1,pFile) != 1)
	{
		IwAssertMsg(SOUND, false, ("Invalid header %s", pathname.c_str()));
		return 0;
	}
	

	tune = new Ct4aTune();
	
	s3eFileClose(pFile);
	return tune;
}
#endif