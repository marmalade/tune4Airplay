#pragma once
#include <IwArray.h>
#include <IwGeomCore.h>

namespace Tune4Airplay
{
	class ISampler;

	class CTuneChannel
	{
		bool hasSamplerContext;
		CIwArray<uint8> samplerContext;
	public:
		iwfixed position;
		iwfixed volume;
		iwfixed tick;
		iwfixed c5pitch;
		uint8 baseNote;
		bool note;
		bool sustain;
		const ISampler* sampler;
		volatile bool active;

		void RequestContext(uint32 bytes);
		inline uint8* GetSamplerContext() { return samplerContext.data();}

		void SetSampler(const ISampler* sampler);
		void StopSampler();
		void SetNote(uint8 baseNote);
		void SetNotePitch(uint8 baseNote);
		void StartSampler();
		CTuneChannel();
		~CTuneChannel();
	};
}