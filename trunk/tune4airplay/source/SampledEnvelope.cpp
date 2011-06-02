#include "SampledEnvelope.h"

using namespace Tune4Airplay;

CSampledEnvelope::CSampledEnvelope()
{
	lastTick = 0;
}
CSampledEnvelope::~CSampledEnvelope()
{
}
void CSampledEnvelope::Serialise()
{
	nodes.SerialiseHeader();
	IwSerialiseInt32(lastTick);
	for(uint32 i=0; i<nodes.size(); ++i)
		IwSerialiseInt32(nodes[i]);
	values.SerialiseHeader();
	for(uint32 i=0; i<values.size(); ++i)
		IwSerialiseInt32(values[i]);
}
void CSampledEnvelope::AddNode(int32 tick,iwfixed val)
{
	int32 prevTick = 0;
	iwfixed prevValue = 0;
	if (tick == 0)
	{
		nodes.push_back(tick);
		values.push_back(val);
		return;
	}
	if (nodes.size() > 0)
	{
		prevTick = nodes.back();
		prevValue = values.back();
	}
	for (int i=prevTick+1; i<=tick;++i)
	{
		values.push_back(prevValue+(val-prevValue)*(i-prevTick)/(tick-prevTick));
	}
	lastTick = IW_FIXED(tick);
	nodes.push_back(tick);
}
void CSampledEnvelope::SetVolumeLoop(int32 s,int32 e)
{
}
void CSampledEnvelope::SetSustainLoop(int32 s,int32 e)
{
}
