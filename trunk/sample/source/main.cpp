#include <s3e.h>
#include <IwGx.h>
#include <s3eSound.h>

#include <Tune4Airplay.h>
#include <Tune.h>
#include <TuneMixer.h>

using namespace Tune4Airplay;

//-----------------------------------------------------------------------------
// Main global function
//-----------------------------------------------------------------------------
int main()
{
	IwGxInit();
	IwResManagerInit();
	Tune4AirplayInit();

	IwGxSetColClear(0xff, 0xff, 0xff, 0xff);
	IwGxPrintSetColour(128, 128, 128);

	CIwResGroup* music = IwGetResManager()->LoadGroup("music.group");
	CTune* tune = static_cast<CTune*>(music->GetResNamed("drumkit", TUNE4AIRPLAY_RESTYPE_TUNE));
	//ISampler * sample = tune->GetSample(0);

	//int32 freq = (int32)sample->GetC5Speed();
	//s3eSoundSetInt(S3E_SOUND_DEFAULT_FREQ, freq);

	{
		CTuneMixer mixer;
		mixer.Play(tune);

		//int channel = s3eSoundGetFreeChannel();
		//s3eSoundChannelPlay(channel,sample->GetData(0),sample->GetSamples(),0,0);

		
		/*CPattern* p = tune->GetPattern(0);
		for(uint i=0; i<p->GetNumRows(); ++i)
		{
			s3eDebugTracePrintf("Row %d\n",i);
			CPatternRow* row = p->GetRow(i);
			for(uint j=0; j<row->GetNumCommands(); ++j)
			{
				CCommand* cmd = row->GetCommand(j);
				s3eDebugTracePrintf("\tch=%d",cmd->GetChannel());
				if (cmd->HasNote())
				{
					s3eDebugTracePrintf(",n=");
					switch (cmd->GetNote()%12)
					{
					case 0:	s3eDebugTracePrintf("C"); break;
					case 1:	s3eDebugTracePrintf("C#"); break;
					case 2:	s3eDebugTracePrintf("D"); break;
					case 3:	s3eDebugTracePrintf("D#"); break;
					case 4:	s3eDebugTracePrintf("E"); break;
					case 5:	s3eDebugTracePrintf("F"); break;
					case 6:	s3eDebugTracePrintf("F#"); break;
					case 7:	s3eDebugTracePrintf("G"); break;
					case 8:	s3eDebugTracePrintf("G#"); break;
					case 9:	s3eDebugTracePrintf("A"); break;
					case 10:	s3eDebugTracePrintf("A#"); break;
					case 11:	s3eDebugTracePrintf("B"); break;
					}
					s3eDebugTracePrintf("%d",cmd->GetNote()/12);
				}
				if (cmd->HasInstrument())
				{
					s3eDebugTracePrintf(",i=%d",cmd->GetInstrument());
				}
				if (cmd->HasVolume())
				{
					s3eDebugTracePrintf(",v=%f",cmd->GetVolume()/((float)IW_GEOM_ONE));
				}
				if (cmd->HasPan())
				{
					s3eDebugTracePrintf(",v=%f",cmd->GetPan()/((float)IW_GEOM_ONE));
				}
				if (cmd->HasCmd())
				{
					s3eDebugTracePrintf(",c=%d(%d)",cmd->GetCmd(),cmd->GetCmdValue());
				}
			}
			s3eDebugTracePrintf("\n");
		}*/

		while (1)
		{
			s3eDeviceYield(0);
			s3eKeyboardUpdate();
			s3ePointerUpdate();

			bool result = true;
			if	(
				(result == false) ||
				(s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN) ||
				(s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN) ||
				(s3eDeviceCheckQuitRequest())
				)
				break;

			IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
		}

		mixer.Stop();
	}
	Tune4AirplayTerminate();
	IwResManagerTerminate();
	IwGxTerminate();
	return 0;
}