#include <s3eSound.h>
#include <s3eFile.h>
#include <IwFile.h>
#include <IwTextParserITX.h>
#include "ResHandlerWAV.h"
#include "t4aSampleIT.h"
#include "TuneMixer.h"

using namespace Tune4Airplay;

IW_CLASS_FACTORY(Ct4aSampleIT);
IW_MANAGED_IMPLEMENT(Ct4aSampleIT);

namespace Tune4Airplay
{
	struct SampleITContext
	{
		typedef bool (Ct4aSampleIT::* RenderMethodPointer) (SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const;
		RenderMethodPointer method;
	};

	bool RenderSample(SamplerRenderContext* context, uint8* samplerContext);
}

Ct4aSampleIT::Ct4aSampleIT()
{
	c5speed = 22050;
	maxPosition = 0;
	useVolumeLoop = T4A_LOOP_NONE;
	useSustainLoop = T4A_LOOP_NONE;
	volume = IW_FIXED(1);
	defaultVolume = IW_FIXED(1);
}

Ct4aSampleIT::~Ct4aSampleIT()
{
}

void Ct4aSampleIT::Serialise()
{
	CIwResource::Serialise();

	IwSerialiseUInt32(c5speed);
	IwSerialiseInt16(volume);
	IwSerialiseInt16(defaultVolume);
	IwSerialiseInt32(maxPosition);

	int16 en = useVolumeLoop;
	IwSerialiseInt16(en);
	useVolumeLoop = (SampleLoop)en;
	IwSerialiseInt32(volumeLoopSampleStart);
	IwSerialiseInt32(volumeLoopSampleEnd);
	
	en = useSustainLoop;
	IwSerialiseInt16(en);
	useSustainLoop = (SampleLoop)en;
	IwSerialiseInt32(sustainLoopSampleStart);
	IwSerialiseInt32(sustainLoopSampleEnd);

	sampleChannels.SerialiseHeader();
	for (uint32 ich=0; ich<sampleChannels.size(); ++ich)
	{
		sampleChannels[ich].SerialiseHeader();
		uint32 samples = sampleChannels[ich].size();
		for (uint32 ismp=0; ismp<samples; ++ismp)
		{
			IwSerialiseInt16(sampleChannels[ich][ismp]);
			//sampleChannels[ich][ismp] = (ismp & 255) << 6;
		}
	}
}
void Ct4aSampleIT::SetC5Speed(uint32 speed)
{
	c5speed = speed;
}
void Ct4aSampleIT::SetVolume(iwsfixed _volume)
{
	volume = _volume;
}
void Ct4aSampleIT::LoadSamples(void* data, uint32 size, bool sample16Bit, bool sampleSigned, bool sampleStereo)
{
	sampleChannels.resize((sampleStereo?2:1));
	maxPosition=IW_FIXED(size);
	for (uint32 ich=0; ich<sampleChannels.size(); ++ich)
		sampleChannels[ich].resize(size);

	if (sample16Bit && sampleSigned)
	{
		int16* data_int16 = (int16*)data;
		for (uint32 ismp=0; ismp<size; ++ismp)
			for (uint32 ich=0; ich<sampleChannels.size(); ++ich)
			{
				sampleChannels[ich][ismp] = *data_int16;
				++data_int16;
			}
	}
	else if (sample16Bit && !sampleSigned)
	{
		uint16* data_uint16 = (uint16*)data;
		for (uint32 ismp=0; ismp<size; ++ismp)
			for (uint32 ich=0; ich<sampleChannels.size(); ++ich)
			{
				sampleChannels[ich][ismp] = (int16)(((int32)*data_uint16)-0x8000);
				++data_uint16;
			}
	}
	else if (!sample16Bit && sampleSigned)
	{
		int8* data_int8 = (int8*)data;
		for (uint32 ismp=0; ismp<size; ++ismp)
			for (uint32 ich=0; ich<sampleChannels.size(); ++ich)
			{
				sampleChannels[ich][ismp] = ((int16)(*data_int8))<<8;
				++data_int8;
			}
	}
	else if (!sample16Bit && !sampleSigned)
	{
		uint8* data_uint8 = (uint8*)data;
		for (uint32 ismp=0; ismp<size; ++ismp)
			for (uint32 ich=0; ich<sampleChannels.size(); ++ich)
			{
				sampleChannels[ich][ismp] = (int16)((((int32)*data_uint8)-0x80) << 8);
				++data_uint8;
			}
	}
}
iwfixed Ct4aSampleIT::GetDefaultVolume(uint8 note) const
{
	return defaultVolume;
}
//Required context size
uint32 Ct4aSampleIT::GetContextSize(uint8 note) const
{
	return sizeof(SampleITContext);
}
//Init sampler context
void Ct4aSampleIT::InitContext(uint8 note, uint8* samplerContext) const
{
	register SampleITContext* ctx = (SampleITContext*)samplerContext;
	ctx->method = &Ct4aSampleIT::ChooseRenderMethod;
}
//Release sampler context
void Ct4aSampleIT::ReleaseContext(uint8 note, uint8* samplerContext) const
{
}
bool Ct4aSampleIT::ChooseRenderMethod(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const
{
	if (sampleChannels.empty())
		return false;

	if (useVolumeLoop)
	{
		samplerContext->method = &Ct4aSampleIT::RenderForwardInVolumeLoop;
	}
	else
	{
		samplerContext->method = &Ct4aSampleIT::RenderForward;
	}
	return (*this .* (samplerContext->method))(context,samplerContext,buf);
}
inline void Ct4aSampleIT::RenderMonoSample(SamplerRenderContext* context, int32* buf) const
{
	uint32 pos = context->channel->position >> IW_GEOM_POINT;
	int32 smpl = (int32)sampleChannels[0][pos];
	smpl = IW_FIXED_MUL(smpl, volume);
	*buf += IW_FIXED_MUL(smpl,context->volume[0]);
	++buf;
	if (context->numChannels > 1)
	{
		*buf += IW_FIXED_MUL(smpl,context->volume[1]);
		++buf;
	}
}
bool  Ct4aSampleIT::RenderForward(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const
{
	if (context->channel->position >= maxPosition)
		return false;

	RenderMonoSample(context,buf);
	iwfixed pitch = context->channel->c5pitch*c5speed/context->outputFrequency;
	context->channel->position += pitch;
	return true;
}
bool  Ct4aSampleIT::RenderForwardInVolumeLoop(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const
{
	if (context->channel->position >= maxPosition)
		return false;

	RenderMonoSample(context,buf);
	iwfixed pitch = context->channel->c5pitch*c5speed/context->outputFrequency;
	context->channel->position += pitch;
	if (!context->channel->note)
	{
		samplerContext->method = &Ct4aSampleIT::RenderForward;
		return true;
	}
	if (context->channel->position > volumeLoopSampleEnd)
	{
		if (useVolumeLoop == T4A_LOOP_FORWARDS)
		{
			//context->channel->position -= volumeLoopSampleEnd-volumeLoopSampleStart;
			context->channel->position = volumeLoopSampleStart;
			return true;
		}
		else if (useVolumeLoop == T4A_LOOP_PINGPONG)
		{
			context->channel->position = volumeLoopSampleEnd;
			samplerContext->method = &Ct4aSampleIT::RenderBackwardInVolumeLoop;
			return true;
		}
	}
	return true;
}
bool  Ct4aSampleIT::RenderBackwardInVolumeLoop(SamplerRenderContext* context, SampleITContext* samplerContext, int32* buf) const
{
	RenderMonoSample(context, buf);
	iwfixed pitch = context->channel->c5pitch*c5speed/context->outputFrequency;
	context->channel->position -= pitch;

	if (context->channel->position <= volumeLoopSampleStart)
	{
		samplerContext->method = &Ct4aSampleIT::RenderForwardInVolumeLoop;
		context->channel->position = volumeLoopSampleStart;
	}
	return true;
}

//Render sound to output buffer
bool Ct4aSampleIT::RenderSample(SamplerRenderContext* context, uint8* samplerContext, int32* output) const
{
	return ((*this) .* (((SampleITContext*)samplerContext)->method))(context,((SampleITContext*)samplerContext),output);
}

#ifdef IW_BUILD_RESOURCES
/// <summary>
/// Parse from text file: parse attribute/value pair, return true only if attribute parsed.
/// </summary>
bool Ct4aSampleIT::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
	if (!strcmp(pAttrName, "source"))
	{
		CIwStringL pathname;
		pParser->ReadString(pathname);

		s3eFile* pFile = IwFileOpenPrefixed(pathname.c_str(), "rb");
		if (!pFile)
		{
			IwAssertMsg(SOUND, false, ("Invalid file %s", pathname.c_str()));
			return false;
		}

		CResHandlerWAV::Build(pFile, this);
		s3eFileClose(pFile);
		return true;
	}
	return CIwManaged::ParseAttribute(pParser,pAttrName);
}
/// <summary>
/// Окончание разбора текстового файла
/// </summary>
void Ct4aSampleIT::ParseClose(CIwTextParserITX* pParser)
{

	Ct4aTune *tune = dynamic_cast<Ct4aTune*>(pParser->GetObject(-1));
	if (tune)
		tune->PushSample(this);

	// Return value to resource Build() method
	pParser->SetReturnValue(this);
}
#endif