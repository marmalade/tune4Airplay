#pragma once
#include <IwArray.h>
#include <IwResManager.h>

#include "t4aTune.h"

namespace Tune4Airplay
{
#ifdef IW_BUILD_RESOURCES
	class CResHandlerS3M : public CIwResHandler
	{
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(CResHandlerS3M);

		//Constructor
		CResHandlerS3M();
		//Desctructor
		virtual ~CResHandlerS3M();

		//Buid resource method
		virtual CIwResource*	Build(const CIwStringL& pathname);
	};
#endif
}