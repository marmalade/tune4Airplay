#pragma once

#include "TuneChannel.h"

namespace Tune4Airplay
{
	class Ct4aTune;
	class CTuneMixer;

	//Sound render environment: buffers and etc
	struct SamplerRenderContext
	{
		uint32	outputFrequency;
		uint32 numChannels;
		CTuneChannel* channel;
		CTuneMixer* mixer;
		//Per-channel volume
		iwfixed volume[2];
	};

	//Sampler interface
	class ISampler
	{
	public:
		virtual void SetTune(Ct4aTune*);

		//Default note volume
		virtual iwfixed GetDefaultVolume(uint8 note) const;

		//Required context size
		virtual uint32 GetContextSize(uint8 note) const;
		//Init sampler context
		virtual void InitContext(uint8 note, uint8* samplerContext) const;
		//Release sampler context
		virtual void ReleaseContext(uint8 note, uint8* samplerContext) const;

		//Evaluate single sample value
		virtual bool RenderSample(SamplerRenderContext* context, uint8* samplerContext, int32* output) const;
		//Render sound to output buffer
		//virtual bool Render(SamplerRenderContext* context, uint8* samplerContext) const;

		//Virtual destructor for dynamic casts and safe delete operator
		virtual ~ISampler();
	};

	enum SampleLoop
	{
		T4A_LOOP_NONE,
		T4A_LOOP_FORWARDS,
		T4A_LOOP_PINGPONG,
	};
}