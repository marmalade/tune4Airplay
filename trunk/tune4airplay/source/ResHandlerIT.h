#pragma once
#include <IwArray.h>
#include <IwResManager.h>

#include "t4aTune.h"

namespace Tune4Airplay
{
#ifdef IW_BUILD_RESOURCES
	class CResHandlerIT : public CIwResHandler
	{
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(CResHandlerIT);

		//Constructor
		CResHandlerIT();
		//Desctructor
		virtual ~CResHandlerIT();

		//Buid resource method
		virtual CIwResource*	Build(const CIwStringL& pathname);
	};
#endif
}