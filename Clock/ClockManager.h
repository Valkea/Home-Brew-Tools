/*
	Auteur : Letremble Emmanuel
	Date : 31.12.2010
	Description : This class should provide an easy way to update and manage all clocks registered.

	- ClockManager			[4] 28 bytes
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_CLOCK_MANAGER
#define HEADER_CLOCK_MANAGER

#include <iostream>	// cout
#include <set>

#include "../PortableTypes.h" 		// define F16, F32, U16, U32 etc...
#include "../Print/PrintStream.h"	// pout
#include "../Event/ClockManagerEvent.h"	// events

#include "Clock.h"

namespace hbt /// [home brew tools]
{
	/** This enables hbt::clockManager to auto-initialize as a reference to the ClockManager singleton.
		Thus a global alias of the ClockManager singleton exists for convenience and for optimization*, but if a manual initialization is required/preferred, this define should be turned to false, and the ClockManager singleton manually initialized.\n Defined in ClockManager.h
		@verbatim *It calls the static method once and redirects all incoming hbt::clockManager calls directly to the ClockManager singleton instead of asking for the singleton reference each time ClockManager::getInstance() is called. @endverbatim
		@ingroup clock */
		#define AUTO_INIT_CLOCKMGR true

//*************************************************************************************************/
///// Clock Manager  //////////////////////////////////////////////////////////////////////////////

	/** This singleton class updates and manages all registered Clock instances.
		@ingroup clock */

	class ClockManager
	{
		public:
			static ClockManager& getInstance()
			{
				static ClockManager singleton;
				return singleton;
			}
			///< Initializes and/or returns the %ClockManager singleton instance.
			/// @return The %ClockManager singleton instance.
			
			~ClockManager();
			///< A destructor.
			
		//== Public functions ===============================
			
			void addClock( hbt::Clock& clock );
			///< Registers a new Clock to the dispatch list.
			/// @param clock The Clock instance to register.
			
			void removeClock( hbt::Clock& clock );
			///< Unregisters a Clock from the dispatch list.
			/// @param clock The Clock instance to unregister.
			
			void update( const F32 deltaTime = 0.0f );
			///< Dispatches an update call to all registered Clocks.
			/// @param deltaTime The time elapsed in seconds since the last update call.
			
			void singleStep( const I16 numSteps = 1 );
			///< Dispatches a singleStep call to all registered Clocks.
			/// @param numSteps The number of single step to apply at once.
			
			void start( const bool startAtFirstCall = false );
			///< Starts the internal timer (useful if elspased time cannot be passed to the update function)
			/// @param startAtFirstCall Defines whether the timer should wait for the first update call to start or not.
			
			void stop();
			///< Stop the internal timer.
			
		//== Getters & Setters ==============================
			
			void setClocksTimeScale( const F32 timeScale );
			///< Applies the time scale value to all registered Clocks.
			/// @param timeScale The updated time scale value.
			
			void setPaused( const bool isPaused );
			///< Asks all registered Clocks to pause without modifying their own pause state.
			/// @param isPaused The updated pause state.			
			bool isPaused() const;
			///< Returns the current ClockManager pause state.
			/// @return The current pause state.
			
			void setTimeScale( const F32 timeScale );
			///< Sets the global time scale without modifying the time scale of the Clocks.
			/// @param timeScale The updated time scale value.
			F32  getTimeScale() const;
			///< Returns the global time scale value.
			/// @return The current time scale value.
			
		private:
			ClockManager();
			
			void updateFromInside();
			///< Updates Clocks using the internal timer
			void updateFromOutside( const F32 deltaTime );
			///< Updates Clocks using the deltaTime received from outside
			/// @param deltaTime The time elapsed in seconds since the last update call.
			
			std::set< hbt::Clock* > m_clockList; 				//[4] 24 bytes
			
			bool 	m_autoEnable; 								//[1] 1 byte
			bool 	m_resetCycles; 								//[1] 1 byte | 2 bytes padding
	};//--------------------------------------------------------->[4] 28 bytes

//*************************************************************************************************/
///// Namespaces //////////////////////////////////////////////////////////////////////////////////

#if AUTO_INIT_CLOCKMGR
	extern ClockManager& clockManager;
	///< A reference to the ClockManager singleton instance.
	///  This enables a quick and easy access to the ClockManager functions using hbt::%clockManager.function() ...
	/// @ingroup clock
#endif


//*************************************************************************************************/
///// Examples ////////////////////////////////////////////////////////////////////////////////////

/*! @class ClockManager

@code

	==> This is an example of how to use the Clock or ClockPlus with ClockManager receiving elspased time from outside. <==

	#include "Clock.h"

	//== Opening calls
	
	// Initializes the Clock
		Clock myClock01;
		Clock myClock02;
	
	// Registers the Clocks to update the ClockManager
		hbt::clockManager.addClock( myClock01 );
		hbt::clockManager.addClock( myClock02 );

	//== Recursive calls

	while(...)
	{
		// Updates all the Clocks using the ClockManager and the time elapsed since the last update call (once per loop !)
			hbt::clockManager.update( timeInSecondsSinceLastCall );
	}

	//== Recursive AND/OR event calls

	// Reverses ClockManager pause state using setPaused() and isPaused() (applies to all registered Clocks)
		hbt::clockManager.setPaused( !hbt::clockManager.isPaused() );

	// Updates the ClockManager time scale by 1.0f using setTimeScale() and getTimeScale() (applies in addition to the Clock time scale)
		hbt::clockManager.setTimeScale( hbt::clockManager..getTimeScale() + 1.0f );
	
	// Updates all Clocks time scale to 2.0f.
		hbt::clockManager.setClocksTimeScale( 2.0f );
	
	// Updates the ClockManager by 1/30 of seconds at each call of the singleStep() (applies to all Clocks currently paused)
		hbt::clockManager.singleStep();
		
	//== Closing calls
		
	// Unregisters the Clocks to the ClockManager
		hbt::clockManager.removeClock( myClock01 );
		hbt::clockManager.removeClock( myClock02 );

@endcode
\n
@code

	==> This is an example of how to use the Clock or ClockPlus with ClockManager and internal elapsed time calculation. <==

	#include "Clock.h"

	//== Opening calls
	
	// Initializes the Clock
		Clock myClock01;
		Clock myClock02;
	
	// Registers the Clocks to update the ClockManager
		hbt::clockManager.addClock( myClock01 );
		hbt::clockManager.addClock( myClock02 );
		
	// Starts the internal timer (waiting for the first update call)
		hbt::clockManager.start( true );

	//== Recursive calls

	while(...)
	{
		// Updates all the Clocks using the ClockManager and NOTHING MORE (once per loop !)
			hbt::clockManager.update(  );
	}

	//== Recursive AND/OR event calls

	// Reverses ClockManager pause state using setPaused() and isPaused() (applies to all registered Clocks)
		hbt::clockManager.setPaused( !hbt::clockManager.isPaused() );

	// Updates the ClockManager time scale by 1.0f using setTimeScale() and getTimeScale() (applies in addition to the Clock time scale)
		hbt::clockManager.setTimeScale( hbt::clockManager..getTimeScale() + 1.0f );
	
	// Updates all Clocks time scale to 2.0f.
		hbt::clockManager.setClocksTimeScale( 2.0f );
	
	// Updates the ClockManager by 1/30 of seconds at each call of the singleStep() (applies to all Clocks currently paused)
		hbt::clockManager.singleStep();
		
	//== Closing calls
	
	// Stops the internal timer
		hbt::clockManager.stop();
		
	// Unregisters the Clocks to the ClockManager
		hbt::clockManager.removeClock( myClock01 );
		hbt::clockManager.removeClock( myClock02 );

@endcode */
	
}
#endif
