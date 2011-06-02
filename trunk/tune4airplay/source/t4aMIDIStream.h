#pragma once
#include <IwResource.h>

#define TUNE4AIRPLAY_RESTYPE_MIDISTREAM	"Ct4aMIDIStream"

namespace Tune4Airplay
{
	//Music composition pattern
	class Ct4aMIDIStream : public CIwResource
	{
#ifdef IW_BUILD_RESOURCES
#endif
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(Ct4aMIDIStream);

		//Constructor
		Ct4aMIDIStream();
		//Desctructor
		virtual ~Ct4aMIDIStream();

		//Serialise to binary
		virtual void Serialise();

		// ---- Text resources ----
#ifdef IW_BUILD_RESOURCES
		// function invoked by the text parser when parsing attributes for objects of this type
	    virtual bool ParseAttribute(CIwTextParserITX *pParser, const char *pAttrName);

		// function invoked by the text parser when the object definition end is encountered
		virtual void ParseClose(CIwTextParserITX* pParser);
#endif
	};
}