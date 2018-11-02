/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 06.02.2011
	Description : This derived class of Clock should provide some unusual methods.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- ClockPlus		[8] 32 (24+8)
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_CLOCK_PLUS
#define HEADER_CLOCK_PLUS

#include "Clock.h"

namespace hbt /// [home brew tools]
{
//*************************************************************************************************/
///// ClockPlus ///////////////////////////////////////////////////////////////////////////////////

	/** This derived class of Clock provides some methods unusual to the standard %Clock.
		@ingroup clock */

	class ClockPlus : public Clock
	{
		public:
			ClockPlus();
			~ClockPlus();
			
		//== Public virtual methods =========================
			
			void update( const F32 = 0.0f );	//* override Clock::update()
			void singleStep( const I16 = 1 );	//* override Clock::singleStep()
			
		//== Getters & Setters ==============================
			
			void setDuration( const F32 );
			///< Sets the %ClockPlus time life to tell when it should stop updating
			/// @param duration The updated %ClockPlus time life value in seconds.
			
			F32  getDuration() const;
			///< Returns the %ClockPlus time life.
			/// @return The current %ClockPlus time life value in seconds.
			
			bool isFinished() const;
			///< Returns the finished state value.
			/// @return The finished state flag value.
			
		private:									// Clock 	//[8] 24 bytes
			F32 	m_duration;									//[4] 4 bytes
			bool 	m_isFinished;								//[1] 1 byte  | 3 bytes padding
	};//--------------------------------------------------------->[8] 32 bytes

	
	
//*************************************************************************************************/
///// Examples ////////////////////////////////////////////////////////////////////////////////////

/*! @class ClockPlus
@code

	==> This is an example of how to use the ClockPlus without ClockManager. <==

	#include "Clock.h"

	//== Opening calls

	// Initializes the ClockPlus
		Clock myClockPlus;
		
	// Sets time life limit.
		myClockPlus.setDuration( 10.0f );

	//== Recursive calls

	while(...)
	{
		// Updates the Clock using the time elapsed since the last update call (once per loop !)
			myClockPlus.update( timeInSecondsSinceLastUpdateCall );
	}

	//== Recursive AND/OR event calls

	// Returns the Clock informations related to the time and cycles elapsed
		std::cout
		<< " Time since start: " << myClockPlus.getTime()
		<< " Cycles since start: " << myClockPlus.getCycles()
		<< " Time since last update: " << myClockPlus.getTimeSinceLastUpdate()
		<< " Cycles since last update: " << myClockPlus.getCyclesSinceLastUpdate()
		<< std::endl;
		
	// Reverses the pause state using setPaused() and isPaused()
		myClockPlus.setPaused( !myClockPlus.isPaused() );
		
	// Updates the Clock time scale by 1.0f using setTimeScale() and getTimeScale()
		myClockPlus.setTimeScale( myClockPlus.getTimeScale() + 1.0f );
		
	// Updates the Clock by 1/30 of seconds at each call of the singleStep()
		myClockPlus.singleStep();
		
	// Updates the Clock time life by 1.0f using setDuration() and getDuration()
		myClockPlus.setDuration( myClockPlus.getDuration() + 1.0f );

@endcode */
	
} // hbt end

#endif

