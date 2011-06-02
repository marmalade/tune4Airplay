#include <IwTextParserITX.h>
#include <IwResManager.h>
#include <IwResGroup.h>
#include "t4aTune.h"
#include "t4aSampleIT.h"

using namespace Tune4Airplay;

IW_CLASS_FACTORY(Ct4aTune);
IW_MANAGED_IMPLEMENT(Ct4aTune)

Ct4aTune::Ct4aTune()
{
	speed = 6;
	tempo = 125;
}

Ct4aTune::~Ct4aTune()
{
	patterns.Delete();
	samples.Delete();
	instruments.Delete();
}
void Ct4aTune::PushPatternIntex(uint32 index)
{
	patternsOrder.push_back(index);
}
void Ct4aTune::PushSample(ISampler*s)
{
	samples.Add(dynamic_cast<CIwManaged*>(s));
}
void Ct4aTune::PushPattern(Ct4aPattern*p)
{
	patterns.Add(p);
}
void Ct4aTune::PushInstrument(ISampler*i)
{
	instruments.Add(dynamic_cast<CIwManaged*>(i));
}
void Ct4aTune::Serialise()
{
	CIwResource::Serialise();
	IwSerialiseUInt16(speed);
	IwSerialiseUInt16(tempo);
	patternsOrder.SerialiseHeader();
	for (uint32 i=0; i<patternsOrder.size(); ++i)
		IwSerialiseUInt32(patternsOrder[i]);

	patterns.Serialise();
	samples.Serialise();
	instruments.Serialise();
	for (uint32 i=0; i<samples.GetSize(); ++i)
		GetSample(i)->SetTune(this);
	for (uint32 i=0; i<instruments.GetSize(); ++i)
		GetInstrument(i)->SetTune(this);
}
#ifdef IW_BUILD_RESOURCES
// function invoked by the text parser when parsing attributes for objects of this type
bool Ct4aTune::ParseAttribute(CIwTextParserITX *pParser, const char* pAttrName)
{
	if (!strcmp(pAttrName, "pattern"))
	{
		uint32 order;
		pParser->ReadUInt32(&order);
		PushPatternIntex(order);
		return true;
	}
	return CIwResource::ParseAttribute(pParser,pAttrName);
}
// Parse from text file: end block.	
void Ct4aTune::ParseClose(CIwTextParserITX* pParser)
{
	// Return value to resource Build() method
	pParser->SetReturnValue(this);

	IwGetResManager()->AddRes(TUNE4AIRPLAY_RESTYPE_TUNE, this);

}
#endif
