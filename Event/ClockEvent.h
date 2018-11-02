/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 02.04.2010
	Description : This class should provide a derived Event class for Clock events.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- ClockEvent	[8] 24 bytes (4+20)

***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_CLOCK_EVENT
#define HEADER_CLOCK_EVENT

#include "../PortableTypes.h" 	// define F16, F32, U16, U32 etc...
#include "EventTypes.h"			// eventTypes
#include "Event.h"				// events

namespace hbt /// [home brew tools]
{
//*************************************************************************************************/
///// ClockEvent //////////////////////////////////////////////////////////////////////////////////

	/** This derived class of Event provides a way to pack informations related to Clock instances.
		@ingroup clock @ingroup event */
			
	class ClockEvent : public Event
	{
		public:
			ClockEvent( const EventType& evtType, bool _isPaused, I32 _timeScale, F64 _cpuCycles, F32 _cyclesElapsed, I16 _numSteps = 0 );
			~ClockEvent();
		
		//== EventTypes =====================================
			static const EventType& START;
			static const EventType& STOP;
			static const EventType& PAUSE;
			static const EventType& RESUME;
			static const EventType& UPDATE;			
			static const EventType& SINGLE_STEP;
													// Event 	//[4] 4 bytes
			I32 	timeScale;									//[4] 4 bytes
			F64 	cpuCycles;									//[8] 8 bytes
			F32 	cyclesElapsed;								//[4] 4 bytes
			I16 	numSteps;									//[2] 2 bytes
			bool 	isPaused;									//[1] 1 byte | 1 byte padding
	};//--------------------------------------------------------->[8] 24 bytes
	
	std::ostream& operator<<( std::ostream& os, const ClockEvent& evt);
	///< Returns a formated text output with the %ClockEvent informations.
	/// @param os The target stream. @param evt The %ClockEvent that must display informations.

} // hbt end

#endif