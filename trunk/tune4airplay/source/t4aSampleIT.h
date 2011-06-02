#pragma once
#include <IwResource.h>
#include <IwGeomCore.h>
#include "ISampler.h"
#define TUNE4AIRPLAY_RESTYPE_SAMPLEIT	"Ct4aSampleIT"

namespace Tune4Airplay
{
	struct SampleITContext;

	//Music composition
	class Ct4aSampleIT : public CIwResource, public ISampler
	{
		CIwArray<CIwArray<int16> > sampleChannels;
		uint32 c5speed;
		iwsfixed volume;
		iwsfixed defaultVolume;
		iwfixed maxPosition;

		SampleLoop useVolumeLoop;
		iwfixed volumeLoopSampleStart;
		iwfixed volumeLoopSampleEnd;
		SampleLoop useSustainLoop;
		iwfixed sustainLoopSampleStart;
		iwfixed sustainLoopSampleEnd;

	public:
		//Declare managed class
		IW_MANAGED_DECLARE(Ct4aSampleIT);

		//Constructor
		Ct4aSampleIT();
		//Desctructor
		virtual ~Ct4aSampleIT();

		inline void SetVolumeLoop(SampleLoop t,uint32 from,uint32 to) { useVolumeLoop=t; volumeLoopSampleStart=IW_FIXED(from); volumeLoopSampleEnd=IW_FIXED(to); }
		inline void SetSustainLoop(SampleLoop t,uint32 from,uint32 to) { useSustainLoop=t; sustainLoopSampleStart=IW_FIXED(from); sustainLoopSampleEnd=IW_FIXED(to); }

		//Serialise to binary
		virtual void Serialise();
		
		inline int16* GetData (uint32 i) {return &(sampleChannels[i][0]); }
		inline const int16* GetData (uint32 i) const {return &(sampleChannels[i][0]); }
		inline uint32 GetChannels () const {return sampleChannels.size(); }
		inline uint32 GetSamples () const { if (sampleChannels.size() == 0) return 0; return sampleChannels[0].size(); }

		void SetC5Speed(uint32 speed);
		inline uint32 GetC5Speed() const {return c5speed; };

		void SetVolume(iwsfixed _volume);
		inline iwsfixed GetVolume() const {return volume; };
		void SetDefaultVolume(iwsfixed _dvolume);
		inline iwsfixed GetDefaultVolume() const {return defaultVolume; };

		void LoadSamples(void* data, uint32 size, bool sample16Bit, bool sampleSigned, bool sampleStereo);

		// ---- ISampler interface implementatin ----
		iwfixed GetDefaultVolume(uint8 note) const;
		//Required context size
		virtual uint32 GetContextSize(uint8 note) const;
		//Init sampler context
		virtual void InitContext(uint8 note, uint8* samplerContext) const;
		//Release sampler context
		virtual void ReleaseContext(uint8 note, uint8* samplerContext) const;
		//Evaluate single sample value
		virtual bool RenderSample(SamplerRenderContext* context, uint8* samplerContext, int32* output) const;

		// ---- Text resources ----
#ifdef IW_BUILD_RESOURCES
		// function invoked by the text parser when parsing attributes for objects of this type
	    virtual bool ParseAttribute(CIwTextParserITX *pParser, const char *pAttrName);

		// function invoked by the text parser when the object definition end is encountered
		virtual void ParseClose(CIwTextParserITX* pParser);
#endif

	private:
		bool ChooseRenderMethod(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const;
		void RenderMonoSample(SamplerRenderContext* context, int32* buf) const;
		bool RenderForward(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const;
		bool RenderForwardInVolumeLoop(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const;
		bool RenderBackwardInVolumeLoop(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const;
	};
}