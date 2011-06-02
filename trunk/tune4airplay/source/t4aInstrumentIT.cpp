#include <IwTextParserITX.h>
#include "t4aInstrumentIT.h"
#include "TuneMixer.h"
#include "TuneChannel.h"
#include "t4aTune.h"

using namespace Tune4Airplay;

IW_CLASS_FACTORY(Ct4aInstrumentIT);
IW_MANAGED_IMPLEMENT(Ct4aInstrumentIT)

namespace Tune4Airplay
{
	struct InstrumentITContext
	{
		ISampler* sampler;
		iwfixed	tick;
	};
}
Ct4aInstrumentIT::Ct4aInstrumentIT()
{
	for(uint32 i=0; i<120; ++i)
		sample[i] = 0xFFFF;
	fadeout = 0;
	useVolumeLoop = T4A_LOOP_NONE;
	useSustainLoop = T4A_LOOP_NONE;
	volumeLoopNodeStart=0;
	volumeLoopNodeEnd=0;
	sustainLoopNodeStart=0;
	sustainLoopNodeEnd=0;
}

Ct4aInstrumentIT::~Ct4aInstrumentIT()
{
}

void Ct4aInstrumentIT::SetTune(Ct4aTune*t){tune=t;}

void Ct4aInstrumentIT::Serialise()
{
	CIwResource::Serialise();

	IwSerialiseInt32(fadeout);

	int16 en = useVolumeLoop;
	IwSerialiseInt16(en);
	useVolumeLoop = (SampleLoop)en;
	IwSerialiseUInt16(volumeLoopNodeStart);
	IwSerialiseUInt16(volumeLoopNodeEnd);

	en = useSustainLoop;
	IwSerialiseInt16(en);
	useSustainLoop = (SampleLoop)en;
	IwSerialiseUInt16(sustainLoopNodeStart);
	IwSerialiseUInt16(sustainLoopNodeEnd);

	for(uint32 i=0; i<120; ++i)
		IwSerialiseUInt16(sample[i]);

	volumeEnv.Serialise();
	panningEnv.Serialise();
	pitchEnv.Serialise();
}
void Ct4aInstrumentIT::SetNoteSample(uint8 note, uint16 s)
{
	this->sample[note] = s;
}
iwfixed Ct4aInstrumentIT::GetDefaultVolume(uint8 baseNote) const
{
	uint16 s = sample[baseNote];
	if (s != 0xFFFF)
			return tune->GetSample(s)->GetDefaultVolume(baseNote);
	return IW_GEOM_ONE;
}
//Required context size
uint32 Ct4aInstrumentIT::GetContextSize(uint8 baseNote) const
{
	uint16 s = sample[baseNote];
	if (s != 0xFFFF)
			return sizeof(InstrumentITContext)+tune->GetSample(s)->GetContextSize(baseNote);
	return sizeof(InstrumentITContext);
}
//Init sampler context
void Ct4aInstrumentIT::InitContext(uint8 baseNote, uint8* samplerContext) const
{
	uint16 s = sample[baseNote];
	InstrumentITContext* ic = ((InstrumentITContext*)samplerContext);
	ic->sampler = 0;
	ic->tick = 0;
	if (s != 0xFFFF)
	{
			ic->sampler = tune->GetSample(s);
			if (ic->sampler)
				tune->GetSample(s)->InitContext(baseNote,samplerContext+sizeof(InstrumentITContext));
	}
	if (volumeEnv.HasNodes())
	{
		volumeEnv.GetValue(ic->tick);
	}
}
//Release sampler context
void Ct4aInstrumentIT::ReleaseContext(uint8 baseNote, uint8* samplerContext) const
{
	InstrumentITContext* ic = ((InstrumentITContext*)samplerContext);
	if (ic->sampler)
	{
			ic->sampler->ReleaseContext(baseNote, samplerContext+sizeof(InstrumentITContext));
			ic->sampler = 0;
	}
}
//Render sound to output buffer
bool Ct4aInstrumentIT::RenderSample(SamplerRenderContext* context, uint8* samplerContext, int32* output) const
{
	InstrumentITContext* ic = ((InstrumentITContext*)samplerContext);
	if (!ic->sampler)
		return false;

	((InstrumentITContext*)samplerContext)->tick += context->mixer->GetTicksPerSample();
	iwfixed tick = ((InstrumentITContext*)samplerContext)->tick;
	iwfixed volume = IW_GEOM_ONE;
	if (volumeEnv.HasNodes())
	{
		if (tick >= volumeEnv.GetLastTick())
			return false;
		volume = volumeEnv.GetValue(tick);
	}
	int32 buf[2] = {0,0};
	ic->sampler->RenderSample(context,samplerContext+sizeof(InstrumentITContext), buf);
	output[0] += IW_FIXED_MUL(volume,buf[0]);
	if (context->numChannels > 1)
		output[1] += IW_FIXED_MUL(volume,buf[1]);
	return true;
}

#ifdef IW_BUILD_RESOURCES
/// <summary>
/// Parse from text file: parse attribute/value pair, return true only if attribute parsed.
/// </summary>
bool Ct4aInstrumentIT::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
	return CIwManaged::ParseAttribute(pParser,pAttrName);
}
/// <summary>
/// Окончание разбора текстового файла
/// </summary>
void Ct4aInstrumentIT::ParseClose(CIwTextParserITX* pParser)
{
	Ct4aTune *tune = dynamic_cast<Ct4aTune*>(pParser->GetObject(-1));
	if (tune)
		tune->PushInstrument(this);

	// Return value to resource Build() method
	pParser->SetReturnValue(this);
}
#endif