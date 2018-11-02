/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 02.04.2010
	Description : This class should provide a derived Event class for ClockManager events.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- ClockManagerEvent		[4] 4 bytes (0+4)
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_CLOCK_MANAGER_EVENT
#define HEADER_CLOCK_MANAGER_EVENT

#include "../PortableTypes.h" 	// define F16, F32, U16, U32 etc...
#include "EventTypes.h"			// eventTypes
#include "Event.h"				// events

namespace hbt /// [home brew tools]
{
//*************************************************************************************************/
///// ClockManagerEvent ///////////////////////////////////////////////////////////////////////////

	/** This derived class of Event provides a way to pack informations related to ClockManager	instances.
		@ingroup clock @ingroup event */
			
	class ClockManagerEvent : public Event
	{
		public:
			ClockManagerEvent( EventType evtType );
			~ClockManagerEvent();
		
		//== EventTypes =====================================
			static const EventType& START;
			static const EventType& STOP;
			static const EventType& PAUSE;
			static const EventType& RESUME;
			static const EventType& RESUME2;
													// Event 	//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 4 bytes
	
	std::ostream& operator<<( std::ostream& os, const ClockManagerEvent& evt);
	///< Returns a formated text output with the %ClockManagerEvent informations.
	/// @param os The target stream. @param evt The %ClockManagerEvent that must display informations.	
}
#endif
