#include "ClockManagerEvent.h"

namespace hbt
{
//*************************************************************************************************/
///// ClockManagerEvent ///////////////////////////////////////////////////////////////////////////
	
	const EventType& ClockManagerEvent::START = EventType::get("CLOCKMGR_START");
	const EventType& ClockManagerEvent::STOP = EventType::get("CLOCKMGR_STOP");
	const EventType& ClockManagerEvent::PAUSE = EventType::get("CLOCKMGR_PAUSE");
	const EventType& ClockManagerEvent::RESUME = EventType::get("CLOCKMGR_RESUME");

	ClockManagerEvent::ClockManagerEvent( EventType evtType ) : Event( evtType )
	{
		EVET_DEBUG( "INIT ClockManagerEvent" );
	}
	
	ClockManagerEvent::~ClockManagerEvent()
	{
		EVET_DEBUG( "CLOSE ClockManagerEvent" );
	}
	
	std::ostream& operator<<( std::ostream& os, const ClockManagerEvent& evt)
	{
		os << "ClockManagerEvent[ type: " << evt.type.name << ", hashId: " << evt.type.id << ", address: " << &evt << "]";
		return os;
	}
}
