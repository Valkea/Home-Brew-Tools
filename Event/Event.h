/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 13.02.2011
	Description : This class should provide a base event class.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- Event				[4] 4 bytes

///// ETA /////////////////////////////////////////////////////////////////////////////////////////

** Bugs - none ATM

***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef HEADER_EVENT
#define HEADER_EVENT

#include <iostream>					// cout
#include "EventTypes.h"				// EventType

#include <functional>					// std::ref
	
namespace hbt /// [home brew tools]
{
//*************************************************************************************************/
///// Event ///////////////////////////////////////////////////////////////////////////////////////

	/** This class is a base class for all derived "Event" classes.
		Its purpose is to offer a common base to transfer event parameters into one unique container.\n
		Thus a "ClockEvent" class and a "WeaponEvent" class would have the same "Event" base plus
		special parameters relevant to their own state.	This way a function, functor or method can
		wait for an "Event" and receive any classes derived from it, or wait for a specific derivation.
		Then informations related to the event (as timeScale, cpuCycles, etc. for a ClockEvent) can
		be extracted and used or transfered at once as one parameter.
		@ingroup event */
		
	class Event
	{
		public: 
		//== EventTypes =====================================
		
			static const EventType& DISPATCH;
			static const EventType& REGISTER;
			static const EventType& UNREGISTER;
			
		//== Constructors ===================================
		
			Event( const EventType& evtType );
			///< A constructor
			~Event();
			///< A destructor

			Event ( const Event& obj );
			///< A copy constructor.
			/// @param obj The Event instance to copy.
			
			Event ( Event&& obj );
			///< A move constructor.
			/// @param obj The Event instance to move.
			
		//== Public members =================================
		
			const EventType& 	type;							//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 4 bytes
	
	std::ostream& operator<<( std::ostream& os, const Event& evt);
	///< Returns a formated text output with Event informations.
	/// @param os The target stream. @param evt The Event that must display informations.
	
} // hbt end

#endif



