#include <IwTextParserITX.h>
#include <IwString.h>
#include "t4aPattern.h"
#include "t4aTune.h"
#include "Tune4Airplay.h"

using namespace Tune4Airplay;

IW_CLASS_FACTORY(Ct4aPattern);
IW_MANAGED_IMPLEMENT(Ct4aPattern)

Ct4aPattern::Ct4aPattern()
{
}

Ct4aPattern::~Ct4aPattern()
{
}

void Ct4aPattern::Serialise()
{
	CIwResource::Serialise();

	rows.SerialiseHeader();
	for (uint32 i=0; i<rows.size(); ++i)
		rows[i].Serialise();
}
void Ct4aPattern::SetNumRows(uint32 numRows)
{
	rows.resize(numRows);
}

#ifdef IW_BUILD_RESOURCES
/// <summary>
/// Parse from text file: parse attribute/value pair, return true only if attribute parsed.
/// </summary>
bool Ct4aPattern::ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName)
{
	if (!strcmp(pAttrName, "rows"))
	{
		uint32 rows;
		pParser->ReadUInt32(&rows);
		SetNumRows(rows);
		return true;
	}
	if (!strcmp(pAttrName, "row"))
	{
		pParser->ReadUInt32(&currentParseRow);
		return true;
	}
	if (!strcmp(pAttrName, "channel"))
	{
		pParser->ReadUInt16(&currentParseChannel);
		return true;
	}
	
	if (!strcmp(pAttrName, "sample"))
	{
		int32 id;
		pParser->ReadInt32(&id);
		GetRow(currentParseRow)->PushCommand(CCommand(currentParseChannel, T4A_CMD_SAMPLE, id));
		return true;
	}
	if (!strcmp(pAttrName, "note"))
	{
		uint32 s;
		pParser->ReadStringHash(&s);
		int8 note = GetNoteByNameHash(s);
		
		GetRow(currentParseRow)->PushCommand(CCommand(currentParseChannel, T4A_CMD_NOTE, note));
		return true;
	}
	return CIwManaged::ParseAttribute(pParser,pAttrName);
}
/// <summary>
/// Окончание разбора текстового файла
/// </summary>
void Ct4aPattern::ParseClose(CIwTextParserITX* pParser)
{
	Ct4aTune *tune = dynamic_cast<Ct4aTune*>(pParser->GetObject(-1));
	if (tune)
		tune->PushPattern(this);

	// Return value to resource Build() method
	pParser->SetReturnValue(this);
}
#endif