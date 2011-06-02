#include "ResHandlerS3M.h"
#include <IwFile.h>

using namespace Tune4Airplay;

#ifdef IW_BUILD_RESOURCES
IW_MANAGED_IMPLEMENT(CResHandlerS3M)

#pragma pack(push)
#pragma pack(1)
struct FileHeaderS3M
{
	char  songname[28];
	uint8 t1a;
	uint8 type;
	uint8 unused1[2];
	uint16 ordnum;
	uint16 insnum;
	uint16 patnum;
	uint16 flags;
	uint16 tracker;
	uint16 fileformat;
	char  scrm[4];
	uint8 mastervol;
	uint8 initspeed;
	uint8 inittempo;
	uint8 mastermult;
	uint8 ultraclick;
	uint8 pantable;
	uint8 unused2[8];
	uint16 special;
	uint8 channels[32];

};
struct SampleHeaderS3M
{
	uint8 type;
	char  filename[12];
	uint8 memsegh;
	uint16 memsegl;
	uint32 length;
	uint32 loopbeg;
	uint32 loopend;
	uint8 volume;
	uint8 dsk;
	uint8 pack;
	uint8 flags;
	uint32 c2spd;
	uint8 unused[12];
	char  sampname[28];
	char  scrs[4];
};
struct NoteHeaderS3M
{
	uint8 note,ins,vol,cmd,inf;
};
#pragma pack(pop)

CResHandlerS3M::CResHandlerS3M() : CIwResHandler("s3m", TUNE4AIRPLAY_RESTYPE_TUNE)
{
	SetName("S3M");
}

CResHandlerS3M::~CResHandlerS3M()
{
}
CIwResource* CResHandlerS3M::Build(const CIwStringL& pathname)
{
	IW_CALLSTACK("CResHandlerS3M::Build")

	Ct4aTune* tune = 0;	// Object to return

	s3eFile* pFile = IwFileOpenPrefixed(pathname.c_str(), "rb");
	if (!pFile)
	{
		IwAssertMsg(SOUND, false, ("Invalid file %s", pathname.c_str()));
		return 0;
	}

	FileHeaderS3M header;
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