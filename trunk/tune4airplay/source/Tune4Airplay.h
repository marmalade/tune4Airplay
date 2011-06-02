#pragma once
#include <s3eTypes.h>

#include "ChannelManager.h"

namespace Tune4Airplay
{
	void Tune4AirplayInit();
	void Tune4AirplayTerminate();
	CChannelManager* GetTune4AirplayChannelManager();

	int8 GetNoteByName(const char* name);
	int8 GetNoteByNameHash(uint32 hash);
}