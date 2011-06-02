#pragma once
#include <IwResource.h>
#include "ISampler.h"
#include "SampledEnvelope.h"
#include "t4aTune.h"
#define TUNE4AIRPLAY_RESTYPE_INSTRUMENT	"Ct4aInstrumentIT"

namespace Tune4Airplay
{
	class CTune;
	//Music composition
	class Ct4aInstrumentIT : public CIwResource, public ISampler
	{
		uint16 sample[120];
		Ct4aTune* tune;

		iwfixed fadeout;

		SampleLoop useVolumeLoop;
		uint16 volumeLoopNodeStart;
		uint16 volumeLoopNodeEnd;

		SampleLoop useSustainLoop;
		uint16 sustainLoopNodeStart;
		uint16 sustainLoopNodeEnd;

		CSampledEnvelope volumeEnv;
		CSampledEnvelope pitchEnv;
		CSampledEnvelope panningEnv;
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(Ct4aInstrumentIT);

		//Constructor
		Ct4aInstrumentIT();
		//Desctructor
		virtual ~Ct4aInstrumentIT();

		inline void SetVolumeLoop(SampleLoop t,uint16 from,uint16 to) { useVolumeLoop=t; volumeLoopNodeStart=from; volumeLoopNodeEnd=to; }
		inline void SetSustainLoop(SampleLoop t,uint16 from,uint16 to) { useSustainLoop=t; sustainLoopNodeStart=from; sustainLoopNodeEnd=to; }

		virtual void SetTune(Ct4aTune*t);
		//Serialise to binary
		virtual void Serialise();

		inline void SetFadeOut(iwfixed _fadeout) {fadeout = _fadeout;}
		inline CSampledEnvelope* GetVolumeEnvelope() {return &volumeEnv;}
		inline CSampledEnvelope* GetPanningEnvelope() {return &panningEnv;}
		inline CSampledEnvelope* GetPitchEnvelope() {return &pitchEnv;}
		void SetNoteSample(uint8 note, uint16 sample);

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
	};
}