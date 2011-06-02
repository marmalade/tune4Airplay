#pragma once
#include <IwResource.h>
#include "PatternRow.h"
#define TUNE4AIRPLAY_RESTYPE_PATTERN	"Ct4aPattern"

namespace Tune4Airplay
{
	//Music composition pattern
	class Ct4aPattern : public CIwResource
	{
		CIwArray<CPatternRow> rows;
#ifdef IW_BUILD_RESOURCES
		uint32 currentParseRow;
		uint16 currentParseChannel;
#endif
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(Ct4aPattern);

		//Constructor
		Ct4aPattern();
		//Desctructor
		virtual ~Ct4aPattern();

		//Serialise to binary
		virtual void Serialise();

		void SetNumRows(uint32 numRows);
		inline uint32 GetNumRows() const {return rows.size(); };
		inline CPatternRow* GetRow(uint32 i) { return (&rows[i]);}
		inline const CPatternRow* GetRow(uint32 i) const { return (&rows[i]);}

		// ---- Text resources ----
#ifdef IW_BUILD_RESOURCES
		// function invoked by the text parser when parsing attributes for objects of this type
	    virtual bool ParseAttribute(CIwTextParserITX *pParser, const char *pAttrName);

		// function invoked by the text parser when the object definition end is encountered
		virtual void ParseClose(CIwTextParserITX* pParser);
#endif
	};
}