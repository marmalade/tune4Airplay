#pragma once
#include <IwArray.h>
#include <IwResManager.h>

namespace Tune4Airplay
{
#ifdef IW_BUILD_RESOURCES
	struct FmtHeaderWAV;
	struct SmplHeaderWAV;
	class Ct4aSampleIT;
	class CResHandlerWAV : public CIwResHandler
	{
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(CResHandlerWAV);

		//Constructor
		CResHandlerWAV();
		//Desctructor
		virtual ~CResHandlerWAV();

		//Buid resource method
		virtual CIwResource*	Build(const CIwStringL& pathname);
		static bool Build(s3eFile* pFile, Ct4aSampleIT* sample);
		static CIwResource* ReadIEEE(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample);
		static CIwResource* ReadUncompressed(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample);
		static CIwResource* ReadUncompressed8(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample);
		static CIwResource* ReadUncompressed16(FmtHeaderWAV* fmt, SmplHeaderWAV* smpl, uint32 size, s3eFile* pFile, Ct4aSampleIT* sample);
	};
#endif
}