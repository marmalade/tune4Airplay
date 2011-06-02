#pragma once
#include <IwResource.h>

#include "Command.h"

namespace Tune4Airplay
{
	//Music composition pattern
	class CPatternRow
	{
		CIwArray<CCommand> commands;
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(CPatternRow);

		//Constructor
		CPatternRow();
		//Desctructor
		virtual ~CPatternRow();

		//Serialise to binary
		virtual void Serialise();

		void PushCommand(const CCommand& cmd);
		inline uint32 GetNumCommands() const {return commands.size(); };
		inline CCommand* GetCommand(uint32 i) { return (&commands[i]);}
		inline const CCommand* GetCommand(uint32 i) const { return (&commands[i]);}
	};
}