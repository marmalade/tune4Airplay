#pragma once

#include <IwGeomCore.h>
#include <IwArray.h>
#include <s3eSound.h>

#include "TuneChannel.h"
#include "t4aTune.h"

namespace Tune4Airplay
{
	class CTuneMixer;

	typedef void (*DynamicEffectCallback)(CTuneMixer* mixer, uint16 channel, void* args);
	
	class CTuneMixer
	{
		volatile bool channelReleased;
		volatile bool isRenderingBuffer;
		CIwArray<CTuneChannel> channels;
		//CIwArray<int32> buffer;
		CIwArray<int16> outputBuffer;
		const Ct4aTune* tune;
		uint32 sampleInRow;
		uint32 nextRowSample;
		uint32 outputFreq;
		iwfixed ticksPerSample;
		uint16 speed;
		uint16 tempo;

		int channel;
		uint32 patternInSequence;
		uint32 pattern;
		uint32 patternRow;
		void AllocateChannel();
		void ReleaseChannel();
		void RenderBuffer();
		void ApplyRow();

		static int32 EndSample(s3eSoundEndSampleInfo *info, CTuneMixer *userData);
		static int32 GenAudioStereo(s3eSoundGenAudioInfo  *systemData, CTuneMixer *userData);
		void RenderBlock(int16* target,uint32 totalNumSamples,s3eSoundGenAudioInfo*);
		void NextRow();
	public:
		CTuneMixer();
		~CTuneMixer();

		void SetBeatsPerMinute(uint16,uint16);

		inline CTuneChannel* GetChannel(uint16 i) {return &channels[i];}
		inline const CTuneChannel* GetChannel(uint16 i) const {return &channels[i];}
		inline const iwfixed GetTicksPerSample() const {return ticksPerSample;}
		inline uint16 GetSpeed() const {return speed;}
		inline uint16 GetTempo() const {return tempo;}
		uint16 GetFreeChannel()const;
		void StartNote(uint16 channel, int32 note, iwfixed volume);
		void Jump(uint32 i);
		void JumpToRow(uint32 i);
		void RowDynamicEffect(uint16 channel,DynamicEffectCallback callback,void*args,uint32 argsSize);

		inline const Ct4aTune* GetTune() const {return tune;}
		void SetActiveChannel(uint16 ch, bool state);
		void Stop();
		void Play(const Ct4aTune*);
	};
}