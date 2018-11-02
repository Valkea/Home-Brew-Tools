/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 01.03.2011
	Description : This class should return a unique identifier for each event type registered.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- EventType				[4] 8 bytes

///// IMPORTANT ///////////////////////////////////////////////////////////////////////////////////
	As EventTypes are created very soon in the initialization process there is some problems to
	consider.
	
	- std::map should be used here, but it seems that a std::map is not ready fast enough and as
	std::multimap is fixing the problem and duplicates verified anyway, the map was replaced by
	a multimap.
	
	- hbt::pout are sent using EVET_DEBUG and in some special conditions printManager is not ready
	yet, so to avoid crashes a POUT_INITIALIZED verification was added to verify if the PrintManager
	and PrintStream instances are ready to be used.
	
///// ETA /////////////////////////////////////////////////////////////////////////////////////////

** ToDo - should we keep a simple incremented index as ID or should we replace it with a hashID ?
** Bugs	- none ATM
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef HEADER_EVENT_TYPE
#define HEADER_EVENT_TYPE

#include <iostream>					// cout
#include <map>
#include <set>
#include <assert.h>

#include "../PortableTypes.h" 		// define F16, F32, U16, U32 etc...
//#include "../Print/PrintStream.h"	// pout								----> Bug under MAC OSX : because PrintManager isn't ready yet ! (m_maxChannel not initialized)

//== Defines & Macros ===============================

#ifdef __PRINT_MANAGER__
	#define EVET_DEBUG( msg ) 	POUT_DEBUG( V_INFORMATION	,1, msg << std::endl );
#else
	#define EVET_DEBUG( msg ) 	//std::cerr << msg << std::endl;
#endif

namespace hbt /// [home brew tools]
{
	#if AUTO_INIT_POUT
		#define POUT_INITIALIZED &hbt::pout //&& &hbt::printManager 
	#else
		#define POUT_INITIALIZED 0
	#endif
	// 	Required to avoid crashes... As EventTypes are created very soon in the initialization
	//	process the printManager seems to not be ready even if we call the getInstance() directly.

//*************************************************************************************************/
///// EventType ///////////////////////////////////////////////////////////////////////////////////
	
		/** This class returns a singleton with a unique identifier for each registered event type.
		At the first EventType::get call, the EventType is created and stored, thereafter this same
		instance is returned each time the EventType is called using get static method.
		@code
			hbt::EventType& onClockUpdate = hbt::EventType::get("CLOCK_UPDATE");
			
			hbt::Signal< void(ClockEvent) > onClockEvent;
			onClockEvent( hbt::ClockEvent( onClockUpdate, 0,0,0,0,0 ) ); // dispatch using predefined EventType
			onClockEvent( hbt::ClockEvent( hbt::EventType::get("CLOCK_STOP"), 0,0,0,0,0 ) ); // dispatch using EventType defined on the fly
			
			hbt::EventType::freeAll();
			
		@endcode
		@warning The copy constructor and the assignment operator are privates to avoid any %EventType
		copies... thus we are 100% sure that all %EventTypes using the same name refer to the same
		instance and that this instance will not disappear because it was passed as a temporary argument.
		@ingroup event */

	class EventType
	{
		typedef std::multimap< std::string, EventType* > T_EventTypeList;
		public:
			~EventType();
			///< A destructor
			
		//== Public functions ===============================
		
			static EventType& get( std::string name );
			///< Initializes and/or return a %EventType singleton instance with a unique id.
			///@param name The name used to identify the EventType.
			///@return The %EventType singleton instance.
			
			static void free( std::string name );
			///< Uninitializes a %EventType singleton instance using its name.
			///@param name The name used to identify the EventType.
			
			static void free( EventType& ref );
			///< Uninitializes a %EventType singleton instance using its reference.
			///@param ref The reference pointing to the EventType to delete.
			
			static void freeAll();
			///< Uninitializes all the %EventType instances.
			
			static EventType* getInstanceByName( std::string name );
			///< Search for an EventType using its name.
			///@return Returns a pointer to the searched EventType, or 0 if not found.
			
			static EventType* getInstanceById( U32 id );	
			///< Search for an EventType using its id.
			///@return Returns a pointer to the searched EventType, or 0 if not found.
			
			operator U32() const;
			///< A conversion operator overload to enable U32 assignment.
			///@return The id value as a U32.
			
			operator std::string() const;
			///< A conversion operator overload to enable std::string assignment.
			///@return The name value as a string.
			
		private:
			EventType( U32 id, std::string name );
			///< A constructor
			
			EventType ( const EventType& obj ){};
			///< A forbidden copy constructor.
			/// This way we are 100% sure to use the EventType stored into the eventTypeList.
			/// @param obj The EventType instance to copy.
			
			EventType& operator= ( const EventType& obj ){};
			///< A forbidden copy assignment operator.
			/// @param obj The EventType instance to copy.
			
			static T_EventTypeList eventTypeList;
			
		public:
			U32			id;										//[4] 4 bytes
			std::string	name;									//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 8 bytes
	
	
	std::ostream& operator<<( std::ostream& os, const EventType& evt);
	///< Returns a formated text output with the %EventType informations.
	/// @param os The target stream. @param evt The %EventType that must display informations.

} // hbt end

#endif

