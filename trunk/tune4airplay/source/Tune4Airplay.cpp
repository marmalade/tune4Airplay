#include "Tune4Airplay.h"
#include "ChannelManager.h"
#include "ResHandlerWAV.h"
#include "ResHandlerIT.h"
#include "ResHandlerS3M.h"
#include "ResHandlerXM.h"
#include "ResHandlerMOD.h"
#include "t4aTune.h"
#include "t4aInstrumentIT.h"
#include "t4aSampleIT.h"
#include "t4aPattern.h"

using namespace Tune4Airplay;

namespace Tune4Airplay
{
	CChannelManager* g_ChannelManager = 0;
	void Tune4AirplayInitCommands();
}
void Tune4Airplay::Tune4AirplayInit()
{
	Tune4AirplayInitCommands();
	#ifdef IW_BUILD_RESOURCES
	IwGetResManager()->AddHandler(new CResHandlerWAV);
	IwGetResManager()->AddHandler(new CResHandlerIT);
	IwGetResManager()->AddHandler(new CResHandlerS3M);
	IwGetResManager()->AddHandler(new CResHandlerXM);
	IwGetResManager()->AddHandler(new CResHandlerMOD);
	#endif

	IW_CLASS_REGISTER(Ct4aTune);
	IW_CLASS_REGISTER(Ct4aInstrumentIT);
	IW_CLASS_REGISTER(Ct4aSampleIT);
	IW_CLASS_REGISTER(Ct4aPattern);

	if (!g_ChannelManager)
		g_ChannelManager = new CChannelManager();
}
void Tune4Airplay::Tune4AirplayTerminate()
{
	if (g_ChannelManager)
	{
		delete g_ChannelManager;
		g_ChannelManager = 0;
	}
}

CChannelManager* Tune4Airplay::GetTune4AirplayChannelManager()
{
	return g_ChannelManager;
}
int8 Tune4Airplay::GetNoteByName(const char* name)
{
	return GetNoteByNameHash(IwHashString(name));
}
int8 Tune4Airplay::GetNoteByNameHash(uint32 hash)
{
	switch (hash)
	{
		case 5863224: return 0; //C0
		case 193486011: return 1; //C#0
		case 5863257: return 2; //D0
		case 193487100: return 3; //D#0
		case 5863290: return 4; //E0
		case 5863323: return 5; //F0
		case 193489278: return 6; //F#0
		case 5863356: return 7; //G0
		case 193490367: return 8; //G#0
		case 5863158: return 9; //A0
		case 193483833: return 10; //A#0
		case 5863191: return 11; //B0
		case 5863225: return 12; //C1
		case 193486012: return 13; //C#1
		case 5863258: return 14; //D1
		case 193487101: return 15; //D#1
		case 5863291: return 16; //E1
		case 5863324: return 17; //F1
		case 193489279: return 18; //F#1
		case 5863357: return 19; //G1
		case 193490368: return 20; //G#1
		case 5863159: return 21; //A1
		case 193483834: return 22; //A#1
		case 5863192: return 23; //B1
		case 5863226: return 24; //C2
		case 193486013: return 25; //C#2
		case 5863259: return 26; //D2
		case 193487102: return 27; //D#2
		case 5863292: return 28; //E2
		case 5863325: return 29; //F2
		case 193489280: return 30; //F#2
		case 5863358: return 31; //G2
		case 193490369: return 32; //G#2
		case 5863160: return 33; //A2
		case 193483835: return 34; //A#2
		case 5863193: return 35; //B2
		case 5863227: return 36; //C3
		case 193486014: return 37; //C#3
		case 5863260: return 38; //D3
		case 193487103: return 39; //D#3
		case 5863293: return 40; //E3
		case 5863326: return 41; //F3
		case 193489281: return 42; //F#3
		case 5863359: return 43; //G3
		case 193490370: return 44; //G#3
		case 5863161: return 45; //A3
		case 193483836: return 46; //A#3
		case 5863194: return 47; //B3
		case 5863228: return 48; //C4
		case 193486015: return 49; //C#4
		case 5863261: return 50; //D4
		case 193487104: return 51; //D#4
		case 5863294: return 52; //E4
		case 5863327: return 53; //F4
		case 193489282: return 54; //F#4
		case 5863360: return 55; //G4
		case 193490371: return 56; //G#4
		case 5863162: return 57; //A4
		case 193483837: return 58; //A#4
		case 5863195: return 59; //B4
		case 5863229: return 60; //C5
		case 193486016: return 61; //C#5
		case 5863262: return 62; //D5
		case 193487105: return 63; //D#5
		case 5863295: return 64; //E5
		case 5863328: return 65; //F5
		case 193489283: return 66; //F#5
		case 5863361: return 67; //G5
		case 193490372: return 68; //G#5
		case 5863163: return 69; //A5
		case 193483838: return 70; //A#5
		case 5863196: return 71; //B5
		case 5863230: return 72; //C6
		case 193486017: return 73; //C#6
		case 5863263: return 74; //D6
		case 193487106: return 75; //D#6
		case 5863296: return 76; //E6
		case 5863329: return 77; //F6
		case 193489284: return 78; //F#6
		case 5863362: return 79; //G6
		case 193490373: return 80; //G#6
		case 5863164: return 81; //A6
		case 193483839: return 82; //A#6
		case 5863197: return 83; //B6
		case 5863231: return 84; //C7
		case 193486018: return 85; //C#7
		case 5863264: return 86; //D7
		case 193487107: return 87; //D#7
		case 5863297: return 88; //E7
		case 5863330: return 89; //F7
		case 193489285: return 90; //F#7
		case 5863363: return 91; //G7
		case 193490374: return 92; //G#7
		case 5863165: return 93; //A7
		case 193483840: return 94; //A#7
		case 5863198: return 95; //B7
		case 5863232: return 96; //C8
		case 193486019: return 97; //C#8
		case 5863265: return 98; //D8
		case 193487108: return 99; //D#8
		case 5863298: return 100; //E8
		case 5863331: return 101; //F8
		case 193489286: return 102; //F#8
		case 5863364: return 103; //G8
		case 193490375: return 104; //G#8
		case 5863166: return 105; //A8
		case 193483841: return 106; //A#8
		case 5863199: return 107; //B8
		case 5863233: return 108; //C9
		case 193486020: return 109; //C#9
		case 5863266: return 110; //D9
		case 193487109: return 111; //D#9
		case 5863299: return 112; //E9
		case 5863332: return 113; //F9
		case 193489287: return 114; //F#9
		case 5863365: return 115; //G9
		case 193490376: return 116; //G#9
		case 5863167: return 117; //A9
		case 193483842: return 118; //A#9
		case 5863200: return 119; //B9
		case 193486473: return 120; //C10
		case 2090071148: return 121; //C#10
		case 193487562: return 122; //D10
		case 2090107085: return 123; //D#10
		case 193488651: return 124; //E10
		case 193489740: return 125; //F10
		case 2090178959: return 126; //F#10
		case 193490829: return 127; //G10
		
		default: IwAssertMsg(SOUND, false, ("Unknown note hash %d",hash)); return 60;
	}
}
