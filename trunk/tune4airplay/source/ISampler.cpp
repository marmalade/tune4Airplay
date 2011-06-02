#include "ISampler.h"

using namespace Tune4Airplay;

void ISampler::SetTune(Ct4aTune*)
{
}

bool ISampler::RenderSample(SamplerRenderContext* context, uint8* samplerContext, int32* output) const
{
	return false;
}
uint32 ISampler::GetContextSize(uint8 note) const
{
	return 0;
}
iwfixed ISampler::GetDefaultVolume(uint8 note) const
{
	return IW_GEOM_ONE;
}
void ISampler::InitContext(uint8 note, uint8* samplerContext) const
{
}
void ISampler::ReleaseContext(uint8 note, uint8* samplerContext) const
{
}

ISampler::~ISampler()
{
}