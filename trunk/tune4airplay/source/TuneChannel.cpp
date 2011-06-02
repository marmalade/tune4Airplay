#include <IwGeomCore.h>
#include <math.h>
#include "TuneChannel.h"
#include "ISampler.h"

using namespace Tune4Airplay;

CTuneChannel::CTuneChannel()
{
	samplerContext.resize(128);
	position = 0;
	hasSamplerContext = false;
	volume = IW_GEOM_ONE;
	c5pitch = IW_GEOM_ONE;
	sustain = false;
	sampler = 0;
	active = false;
	SetNote(60);
}

CTuneChannel::~CTuneChannel()
{
}
void CTuneChannel::SetSampler(const ISampler* _sampler)
{
	StopSampler();
	sampler = _sampler;
}
void CTuneChannel::StopSampler()
{
	if (!sampler)
		return;
	if (hasSamplerContext)
	{
		sampler->ReleaseContext(baseNote, GetSamplerContext());
		hasSamplerContext = false;
	}
}
void CTuneChannel::StartSampler()
{
	if (!sampler)
		return;
	if (!hasSamplerContext)
	{
		RequestContext(sampler->GetContextSize(baseNote));
		sampler->InitContext(baseNote, GetSamplerContext());
		hasSamplerContext = true;
	}
}
void CTuneChannel::RequestContext(uint32 bytes) 
{ 
	if(samplerContext.size()<bytes) 
	{
		IwAssertMsg(SOUND,false,("can't allocate data in sound callback"));
		samplerContext.resize(bytes);
	}
}
void CTuneChannel::SetNotePitch(uint8 baseNote)
{
	//1,0594630943592952645618252949461
	c5pitch = IW_FIXED_FROM_FLOAT(powf(2.0f,((int)baseNote-60)/12.0f));
}
void CTuneChannel::SetNote(uint8 n)
{
	position = 0;
	tick = 0;
	note = true;
	baseNote = n;
	SetNotePitch(baseNote);
}