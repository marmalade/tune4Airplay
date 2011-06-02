#pragma once
#include <IwResource.h>
#include <IwManagedList.h>
#include <IwGeomCore.h>

#define TUNE4AIRPLAY_RESTYPE_COMMAND	"CCommand"

namespace Tune4Airplay
{
	class CCommand;
	class CTuneMixer;
	class CTuneChannel;
	typedef void (*CommandProcessorProc)(const CCommand*, CTuneMixer*);

	enum Commands
	{
		T4A_CMD_NOTEOFF=0,
		T4A_CMD_NOTE,
		T4A_CMD_SAMPLE,
		T4A_CMD_INSTRUMENT,
		T4A_CMD_VOLUME,
		T4A_CMD_VOLUME_SLIDE,
		T4A_CMD_PITCH_SLIDE,
		T4A_CMD_PAN,
		T4A_CMD_SPEED,
		T4A_CMD_TEMPO,
		T4A_CMD_RETRIGGER,
		T4A_CMD_JUMP_TO_ORDER,
		T4A_CMD_JUMP_TO_ROW,

		T4A_CMD_MAX,
	};

	//Music composition
	class CCommand
	{
		uint16 channel;

		Commands command;

		int32 arg;
		/*
		uint8 note;
		bool hasNote;
		bool noteOff;

		uint16 instrument;
		bool hasInstrument;

		iwsfixed volume;
		bool hasVolume;

		iwsfixed pan;
		bool hasPan;

		uint16 cmd;
		uint16 cmdVal;
		bool hasCmd;
		*/
	public:
		//Declare managed class
		//IW_MANAGED_DECLARE(CCommand);

		//Constructor
		CCommand();
		//Constructor
		inline CCommand(uint16 ch, Commands cmd):channel(ch),command(cmd),arg(0) {}
		//Constructor
		inline CCommand(uint16 ch, Commands cmd, int32 a):channel(ch),command(cmd),arg(a) {}
		//Desctructor
		~CCommand();

		void Apply(CTuneMixer*) const;

		//Serialise to binary
		void Serialise();

		inline uint16 GetChannel() const { return (channel);}
		inline void SetChannel(uint16 _channel) { channel = _channel; }

		inline Commands GetCommand() const { return (command);}
		inline int32 GetCommandArgument() const { return (arg);}
		inline void SetCommand(Commands _cmd, int32 _arg) { command = _cmd; arg = _arg; }

		/*
		inline uint8 GetNote() const { return (note);}
		inline void SetNote(uint8 _node) { note = _node; hasNote = true; }
		inline bool HasNote() const { return hasNote; }

		inline iwsfixed GetVolume() const { return (volume);}
		inline void SetVolume(iwsfixed _node) { volume = _node; hasVolume = true; }
		inline bool HasVolume() const { return hasVolume; }

		inline iwsfixed GetPan() const { return (pan);}
		inline void SetPan(iwsfixed _node) { pan = _node; hasPan = true; }
		inline bool HasPan() const { return hasPan; }

		inline uint16 GetCmd() const { return (cmd);}
		inline uint16 GetCmdValue() const { return (cmdVal);}
		inline void SetCmd(int16 _cmd, int16 _cmdVal) { cmd = _cmd; cmdVal = _cmdVal; hasCmd = true; }
		inline bool HasCmd() const { return hasCmd; }

		inline uint16 GetInstrument() const { return (instrument);}
		inline void SetInstrument(uint16 _instrument) { instrument = _instrument; hasInstrument = true; }
		inline bool HasInstrument() const { return hasInstrument; }

		inline void SetNoteOff() { noteOff = true; }
		inline bool IsNoteOff() const { return noteOff; }
		*/
	};

}