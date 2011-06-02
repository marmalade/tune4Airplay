#include <s3eSound.h>
#include "TuneMixer.h"


using namespace Tune4Airplay;

CTuneMixer::CTuneMixer()
{
	channel = -1;
	channels.resize(64);
	outputBuffer.resize(1024);
	
	sampleInRow=0;
	nextRowSample=4000;
	outputFreq = s3eSoundGetInt(S3E_SOUND_OUTPUT_FREQ);
	speed = 6;
	tempo = 120;
	channelReleased = true;
	isRenderingBuffer = false;
}

CTuneMixer::~CTuneMixer()
{
	ReleaseChannel();
}

void CTuneMixer::AllocateChannel()
{
	if (channel == -1)
	{
		channel = s3eSoundGetFreeChannel();
		RenderBuffer();
		s3eSoundChannelRegister(channel, S3E_CHANNEL_END_SAMPLE, (s3eCallback)EndSample, this);
		s3eSoundChannelRegister(channel,S3E_CHANNEL_GEN_AUDIO,(s3eCallback)GenAudioStereo,this);
		uint32 size = outputBuffer.size();
		if (s3eSoundGetInt(S3E_SOUND_STEREO_ENABLED))
		{
			s3eSoundChannelRegister(channel,S3E_CHANNEL_GEN_AUDIO_STEREO,(s3eCallback)GenAudioStereo,this);
			size /= 2;
		}
		channelReleased = false;
		s3eSoundChannelPlay(channel,outputBuffer.data(),size,0,0);
	}
}
int32 CTuneMixer::EndSample(s3eSoundEndSampleInfo *info, CTuneMixer *userData)
{
	userData->channelReleased = true;
	while (userData->isRenderingBuffer)
	{
		s3eDeviceYield(0);
	}
	return info->m_RepsRemaining;
}
inline int16 ClipToInt16(int32 sval)
{
    enum
    {
        minval =  INT16_MIN,
        maxval =  INT16_MAX,
        allbits = UINT16_MAX
    };

    // quick overflow test, the addition moves valid range to 0-allbits
    if ((sval-minval) & ~allbits)
    {
        // we overflowed.
        if (sval > maxval)
            sval = maxval;
        else
            if (sval < minval)
                sval = minval;
    }

    return (int16)sval;
}

int32 CTuneMixer::GenAudioStereo(s3eSoundGenAudioInfo *info, CTuneMixer *mixer)
{
	mixer->isRenderingBuffer = true;
	if (mixer->tune == 0 || mixer->channelReleased)
	{
		info->m_EndSample = 1;
		mixer->isRenderingBuffer = false;
		return 0;
	}
	int16* target = (int16*)info->m_Target;
	//IwDebugTraceLinePrintf("info->m_NumSamples = %d",info->m_NumSamples);
	uint32 left = info->m_NumSamples;
	uint32 pos = 0;
	uint32 samplePitch = (info->m_Stereo)?2:1;

	while (left > 0 && !mixer->channelReleased)
	{
		uint32 rest = (mixer->sampleInRow<mixer->nextRowSample)?(mixer->nextRowSample-mixer->sampleInRow):0;
		if (left < rest)
			rest = left;
		if (rest > 0)
		{
			mixer->RenderBlock(target+pos*samplePitch,rest,info);
			pos += rest;
			left -= rest;
			mixer->sampleInRow += rest;
		}
		if (mixer->sampleInRow >= mixer->nextRowSample)
		{
			mixer->sampleInRow = 0;
			mixer->NextRow();
		}
	}
	mixer->isRenderingBuffer = false;
	return info->m_NumSamples;
}
void CTuneMixer::Jump(uint32 i)
{
	patternInSequence = i;
}
void CTuneMixer::JumpToRow(uint32 i)
{
	patternRow = i;
}
void CTuneMixer::NextRow()
{
	++patternRow;
	if (patternRow >= tune->GetPattern(pattern)->GetNumRows())
	{
		patternRow = 0;
		++patternInSequence;
		if (patternInSequence >= tune->GetPatternsSeqLength())
			patternInSequence = 0;
		pattern = tune->GetPatternInSeq(patternInSequence);
	}
	uint32 r = patternRow;
	uint32 p = patternInSequence;
	ApplyRow();
	while ((r != patternRow) || (p != patternInSequence))
	{
		r = patternRow;
		p = patternInSequence;
		ApplyRow();
	}
}
void CTuneMixer::RowDynamicEffect(uint16 channel,DynamicEffectCallback callback,void*args,uint32 argsSize)
{
}
void CTuneMixer::SetActiveChannel(uint16 ch, bool state)
{
	channels[ch].active = state;
}
void CTuneMixer::SetBeatsPerMinute(uint16 _speed, uint16 _tempo)
{
	speed = _speed;
	tempo = _tempo;
	int32 freq = outputFreq*60; //samples per minute;
	nextRowSample = freq/(24*(int32)tempo/speed);
	ticksPerSample = IW_FIXED(6)/nextRowSample;
}
void CTuneMixer::StartNote(uint16 channel, int32 note, iwfixed volume)
{
	CTuneChannel* ch = GetChannel(channel);
	ch->StopSampler();
	ch->SetNote(note);
	ch->StartSampler();
	ch->volume = volume;
	SetActiveChannel(channel,true);
}
uint16 CTuneMixer::GetFreeChannel()const
{
	for (uint16 i = 0; i < (uint16)channels.size(); ++i)
	{
		CTuneChannel* ch = &channels[i];
		if (!ch->active)
		{
			return i;
		}
	}
	return 0;
}
void CTuneMixer::RenderBlock(int16* target,uint32 m_NumSamples,s3eSoundGenAudioInfo*info)
{
	uint32 totalNumSamples = m_NumSamples;
	//Prepare buffer
	if (info->m_Stereo) totalNumSamples*=2;

	SamplerRenderContext context;
	context.outputFrequency = outputFreq;
	context.mixer = this;
	context.numChannels = (info->m_Stereo)?2:1;



	//Render all active channels
	for (uint32 sample=0; sample<m_NumSamples && !this->channelReleased; ++sample)
	{
		int32 temp [] = {0,0};
		for (uint32 i = 0; i < channels.size() && !this->channelReleased; ++i)
		{
			int32 sampled [] = {0,0};
			CTuneChannel* ch = &channels[i];
			if (ch->active)
			{
				if (ch->sampler)
				{
					context.channel = ch;
					if (context.numChannels > 1)
					{
						context.volume[0] = IW_FIXED_MUL(ch->volume,IW_GEOM_ONE);
						context.volume[1] = IW_FIXED_MUL(ch->volume,IW_GEOM_ONE);
					}
					else
					{
						context.volume[0] = ch->volume;
					}
				
					if (!ch->sampler->RenderSample(&context, ch->GetSamplerContext(), sampled))
					{
						SetActiveChannel(i,false);
						break;
					}
					temp[0] += sampled[0];
					if (context.numChannels > 1)
						temp[1] += sampled[1];
				}

			}
		}
		if (info->m_Mix)
			*target = ClipToInt16(*target+temp[0]);
		else
			*target = ClipToInt16(temp[0]);
		++target;
		if (context.numChannels > 1)
		{
			if (info->m_Mix)
				*target = ClipToInt16(*target+temp[1]);
			else
				*target = ClipToInt16(temp[1]);
			++target;
		}
	}
}
void CTuneMixer::RenderBuffer()
{
}
void CTuneMixer::ReleaseChannel()
{
	if (channel >= 0)
	{
		s3eSoundChannelStop(channel);
		if (s3eSoundGetInt(S3E_SOUND_STEREO_ENABLED))
			s3eSoundChannelUnRegister(channel,S3E_CHANNEL_GEN_AUDIO_STEREO);
		s3eSoundChannelUnRegister(channel,S3E_CHANNEL_GEN_AUDIO);
		s3eSoundChannelUnRegister(channel,S3E_CHANNEL_END_SAMPLE);
		channel = -1;
	}
}
void CTuneMixer::Stop()
{
	ReleaseChannel();
	tune = 0;
}
void CTuneMixer::ApplyRow()
{
	const CPatternRow* r = tune->GetPattern(pattern)->GetRow(patternRow);
	for (uint32 i=0; i<r->GetNumCommands();++i)
		r->GetCommand(i)->Apply(this);
}
void CTuneMixer::Play(const Ct4aTune* t)
{
	tune = t;
	AllocateChannel();
	SetBeatsPerMinute(t->GetSpeed(), t->GetTempo());
	patternInSequence = 0;
	pattern = t->GetPatternInSeq(patternInSequence);
	patternRow = 0;
	channels[0].position = 0;
	//channels[0].pitch = IW_FIXED(0.4f);
	//channels[0].sampler = t->GetSample(0);
	ApplyRow();
}
