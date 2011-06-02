#include "t4aPattern.h"

using namespace Tune4Airplay;


CPatternRow::CPatternRow()
{
}

CPatternRow::~CPatternRow()
{
}
void CPatternRow::PushCommand(const CCommand& cmd)
{
	commands.push_back(cmd);
}

void CPatternRow::Serialise()
{
	commands.SerialiseHeader();
	for (uint32 i=0; i<commands.size(); ++i)
	{
		commands[i].Serialise();
		/*if (commands[i].GetChannel() >= 32)
			IwAssertMsg(SOUND,false,("Wrong channel"));*/
	}
}
