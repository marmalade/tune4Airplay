#include <IwTextParserITX.h>
#include <IwString.h>
#include "t4aMIDIStream.h"
#include "t4aTune.h"
#include "Tune4Airplay.h"

using namespace Tune4Airplay;

IW_CLASS_FACTORY(Ct4aMIDIStream);
IW_MANAGED_IMPLEMENT(Ct4aMIDIStream)

Ct4aMIDIStream::Ct4aMIDIStream()
{
}

Ct4aMIDIStream::~Ct4aMIDIStream()
{
}

void Ct4aMIDIStream::Serialise()
{
	CIwResource::Serialise();

}

#ifdef IW_BUILD_RESOURCES
/// <summary>
/// Parse from text file: parse attribute/value pair, return true only if attribute parsed.
/// </summary>
bool Ct4aMIDIStream::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
	/*if (!strcmp(pAttrName, "rows"))
	{
		uint32 rows;
		pParser->ReadUInt32(&rows);
		SetNumRows(rows);
		return true;
	}*/
	
	return CIwManaged::ParseAttribute(pParser,pAttrName);
}
/// <summary>
/// Окончание разбора текстового файла
/// </summary>
void Ct4aMIDIStream::ParseClose(CIwTextParserITX* pParser)
{
	Ct4aTune *tune = dynamic_cast<Ct4aTune*>(pParser->GetObject(-1));
	//if (tune)
	//	tune->PushPattern(this);

	// Return value to resource Build() method
	pParser->SetReturnValue(this);
}
#endif