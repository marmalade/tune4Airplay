#pragma once

#include <IwArray.h>
#include <IwGeomCore.h>

namespace Tune4Airplay
{
	//Instrument envelope
	class CSampledEnvelope
	{
	private:
		CIwArray<iwfixed> values;
		CIwArray<int32> nodes;
		iwfixed lastTick;
	public:
		CSampledEnvelope();
		~CSampledEnvelope();

		void Serialise();

		inline bool HasNodes() const {return !nodes.empty();}
		inline iwfixed GetValue(iwfixed tick) const { return values[tick>>IW_GEOM_POINT]; }
		inline iwfixed GetLastTick() const { return lastTick; }

		void AddNode(int32 tick,iwfixed val);
		void SetVolumeLoop(int32 s,int32 e);
		void SetSustainLoop(int32 s,int32 e);
	};
}