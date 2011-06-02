#include <math.h>
#include "Command.h"
#include "TuneMixer.h"

using namespace Tune4Airplay;

namespace Tune4Airplay
{
	CommandProcessorProc g_commandProcessor[T4A_CMD_MAX];

	void NoCommand(const CCommand*, CTuneMixer*)
	{
	}
	void NoteCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		int32 note = (uint8)cmd->GetCommandArgument();
		CTuneChannel* ch = mixer->GetChannel(cmd->GetChannel());
		if (ch->sampler)
			mixer->StartNote(cmd->GetChannel(),note,ch->sampler->GetDefaultVolume(note));
		else
			mixer->StartNote(cmd->GetChannel(),note,IW_GEOM_ONE);
	}
	void NoteOffCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->GetChannel(cmd->GetChannel())->note = false;
	}
	
	void InstrumentCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		CTuneChannel* ch = mixer->GetChannel(cmd->GetChannel());
		ch->StopSampler();
		ch->sampler = mixer->GetTune()->GetInstrument(cmd->GetCommandArgument());
		//ch->StartSampler();
	}
	void SampleCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		int32 sample = cmd->GetCommandArgument();
		if (sample >= 0)
		{
			CTuneChannel* ch = mixer->GetChannel(cmd->GetChannel());
			ch->SetSampler(mixer->GetTune()->GetSample(cmd->GetCommandArgument()));
		}
	}
	void VolumeCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->GetChannel(cmd->GetChannel())->volume = cmd->GetCommandArgument();
	}
	void PitchSlideCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		CTuneChannel* ch = mixer->GetChannel(cmd->GetChannel());
		ch->SetNotePitch((uint8)(ch->baseNote+cmd->GetCommandArgument()));
	}
	void TempoCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->SetBeatsPerMinute(mixer->GetSpeed(),(uint16)cmd->GetCommandArgument());
	}
	void SpeedCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->SetBeatsPerMinute((uint16)cmd->GetCommandArgument(),mixer->GetTempo());
	}
	void RetriggerNoteProc(CTuneMixer* mixer, uint16 channel, void* args)
	{
	}
	void RetriggerNoteCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->RowDynamicEffect(cmd->GetChannel(),RetriggerNoteProc,0,0);
	}
	void JumpToOrderCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->Jump(cmd->GetCommandArgument());
	}
	void JumpToRowCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		mixer->JumpToRow(cmd->GetCommandArgument());
	}
	
	void VolumeSlideCommand(const CCommand* cmd, CTuneMixer*mixer)
	{
		iwfixed& v = mixer->GetChannel(cmd->GetChannel())->volume;
		v += cmd->GetCommandArgument();
		if (v < 0) v = 0;
		if (v > IW_GEOM_ONE) v=IW_GEOM_ONE;
	}
	void Tune4AirplayInitCommands()
	{
		g_commandProcessor[T4A_CMD_NOTEOFF] = NoteOffCommand;
		g_commandProcessor[T4A_CMD_NOTE] = NoteCommand;
		g_commandProcessor[T4A_CMD_SAMPLE] = SampleCommand;
		g_commandProcessor[T4A_CMD_INSTRUMENT] = InstrumentCommand;
		g_commandProcessor[T4A_CMD_VOLUME] = VolumeCommand;
		g_commandProcessor[T4A_CMD_VOLUME_SLIDE] = VolumeSlideCommand;
		g_commandProcessor[T4A_CMD_PITCH_SLIDE] = PitchSlideCommand;
		g_commandProcessor[T4A_CMD_PAN] = NoCommand;
		g_commandProcessor[T4A_CMD_SPEED] = SpeedCommand;
		g_commandProcessor[T4A_CMD_TEMPO] = TempoCommand;
		g_commandProcessor[T4A_CMD_RETRIGGER] = RetriggerNoteCommand;
		g_commandProcessor[T4A_CMD_JUMP_TO_ORDER] = JumpToOrderCommand;
		g_commandProcessor[T4A_CMD_JUMP_TO_ROW] = JumpToRowCommand;
	}
}

CCommand::CCommand()
{
	channel = 0;
	command = T4A_CMD_NOTEOFF;
	arg = 0;
}

CCommand::~CCommand()
{
}
void CCommand::Apply(CTuneMixer*m) const
{
	g_commandProcessor[command](this,m);
}
void CCommand::Serialise()
{
	IwSerialiseUInt16(channel);
	int32 a = command;
	IwSerialiseInt32(a);
	command = (Commands)a;
	IwSerialiseInt32(arg);
	
}