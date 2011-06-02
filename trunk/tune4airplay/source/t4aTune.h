#pragma once
#include <IwResource.h>
#include <IwManagedList.h>

#include "t4aPattern.h"
#include "ISampler.h"

#define TUNE4AIRPLAY_RESTYPE_TUNE	"Ct4aTune"

namespace Tune4Airplay
{
	//Music composition
	class Ct4aTune : public CIwResource
	{
		uint16 speed;
		uint16 tempo;

		CIwArray<uint32> patternsOrder;
		CIwManagedList	patterns;
		CIwManagedList	samples;
		CIwManagedList	instruments;
	public:
		//Declare managed class
		IW_MANAGED_DECLARE(Ct4aTune);

		inline void SetSpeed(uint16 speed) {this->speed = speed;}
		inline void SetTempo(uint16 tempo) {this->tempo = tempo;}
		inline uint16 GetSpeed() const {return speed;}
		inline uint16 GetTempo() const {return tempo;}

		//Constructor
		Ct4aTune();
		//Desctructor
		virtual ~Ct4aTune();

		virtual void PushPatternIntex(uint32 index);
		virtual void PushSample(ISampler*);
		virtual void PushPattern(Ct4aPattern*);
		virtual void PushInstrument(ISampler*);

		inline uint32 GetNumSamples() const {return samples.GetSize(); };
		inline ISampler* GetSample(uint32 i) { return dynamic_cast<ISampler*>(samples[i]);}
		inline const ISampler* GetSample(uint32 i) const { return dynamic_cast<ISampler*>(samples[i]);}

		inline uint32 GetNumInstruments() const {return instruments.GetSize(); };
		inline ISampler* GetInstrument(uint32 i) { return dynamic_cast<ISampler*>(instruments[i]);}
		inline const ISampler* GetInstrument(uint32 i) const { return dynamic_cast<ISampler*>(instruments[i]);}

		inline uint32 GetPatternsSeqLength() const {return patternsOrder.size(); };
		inline uint32 GetPatternInSeq(uint32 i) const { return patternsOrder[i];}

		inline uint32 GetNumPatterns() const {return patterns.GetSize(); };
		inline Ct4aPattern* GetPattern(uint32 i) { return static_cast<Ct4aPattern*>(patterns[i]);}
		inline const Ct4aPattern* GetPattern(uint32 i) const { return static_cast<Ct4aPattern*>(patterns[i]);}

		//Serialise to binary
		virtual void Serialise();

#ifdef IW_BUILD_RESOURCES
		// Parse from text file: parse attribute/value pair, return true only if attribute parsed.
		virtual bool ParseAttribute(CIwTextParserITX* pParser, const char* pAttrName);
		// Parse from text file: end block.	
		virtual void ParseClose(CIwTextParserITX* pParser);
#endif
	};
}