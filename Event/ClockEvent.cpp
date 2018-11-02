#include "ClockEvent.h"

namespace hbt
{
//*************************************************************************************************/
///// ClockEvent //////////////////////////////////////////////////////////////////////////////////

	const EventType& ClockEvent::START = EventType::get("CLOCK_START");
	const EventType& ClockEvent::STOP = EventType::get("CLOCK_STOP");
	const EventType& ClockEvent::PAUSE = EventType::get("CLOCK_PAUSE");
	const EventType& ClockEvent::RESUME = EventType::get("CLOCK_RESUME");
	const EventType& ClockEvent::UPDATE = EventType::get("CLOCK_UPDATE");
	const EventType& ClockEvent::SINGLE_STEP = EventType::get("CLOCK_SINGLE_STEP");
	
	ClockEvent::ClockEvent( const EventType& evtType, bool _isPaused, I32 _timeScale, F64 _cpuCycles, F32 _cyclesElapsed, I16 _numSteps )
	: Event( evtType ), isPaused(_isPaused), timeScale(_timeScale), cpuCycles(_cpuCycles), cyclesElapsed(_cyclesElapsed), numSteps(_numSteps)
	{
		EVET_DEBUG( "INIT ClockEvent" );
	}
	
	ClockEvent::~ClockEvent()
	{
		EVET_DEBUG( "CLOSE ClockEvent" );
	}
	
	std::ostream& operator<<( std::ostream& os, const ClockEvent& evt)
	{
		os << "ClockEvent[ type: " << evt.type.name << ", isPaused:" << evt.isPaused << ", timeScale:" << evt.timeScale << ", cpuCycles:" << evt.cpuCycles<< ", cyclesElapsed:" << evt.cyclesElapsed;
		
		///if( evt.type == ClockEvent::SINGLE_STEP ) os << ", numSteps:" << evt.numSteps;
		
		os << ", hashId: " << evt.type.id << ", address: " << &evt << "]";
		return os;
	}
	
} // hbt end

