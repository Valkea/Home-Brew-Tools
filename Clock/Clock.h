/*
	Auteur : Letremble Emmanuel
	Date : 17.12.2010
	Description : This class should provide a high-resolution timer and the tools to control it.

	- ClockEvent	[8] 24 bytes (4+20)
	- Clock			[8] 24 bytes
	
///// IMPORTANT ///////////////////////////////////////////////////////////////////////////////////
	To make a real High-Resolution Timer we need to replace the following elements :
	
	- readHiResTimerFrequency() --> CLOCKS_PER_SEC 	: we need to replace it by a function returning
													  the CPU cycle numbers PER SECONDS (or per
													  defined period) depending of the current OS.
	- readHiResTimer() 			--> clock() 		: we need to replace it by a function returning
													  the CPU cycle numbers depending of the current OS.
	
	Currently, we use CLOCKS_PER_SEC and clock() because they are both cross-platform, but I'm not
	sure this is still true on Video-Game Consoles even if they are from the std::. Furthermore, for
	a real High-Resolution Timer we	need to use a 64 bit cycle counter instead of a 32 bit
	
	----> See 'Important' section at the bottom of this file for more details on High-Resolution Timer.

///// Example /////////////////////////////////////////////////////////////////////////////////////
	----> See 'Examples' section at the bottom of the file... (none ATM)

///// ETA /////////////////////////////////////////////////////////////////////////////////////////
 * ToDo - We must verify that F64 members won't create rounding problems with very long durations
		  (few days of plays without restart...)
 * ToDo - Should we use Signals&Slots class to dispatch Clock Event (paused, finished, resumed...)?
		
 * ToDo - (OPTIONAL) We can eventually add the following methods to the ClockManager :
					 getTime(), getTimeSinceLastUpdate(), getCycles(), getCyclesSinceLastUpdate()
 * ToDo	- (OPTIONAL) We can eventually change the ClockManager from singleton to normal class and
					 allow for multiple ClockManagers. (If we do this we need to change the way
					 s_isPaused and s_timeScale works, by adding a member variable m_clockManager
					 which is filled by the ClockManager when adding Clock to it)
		
 * Bugs	- none ATM
		
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_CLOCK
#define HEADER_CLOCK

#include <time.h>						// clock() / CLOCKS_PER_SEC
#include <iostream>						// cout

#include "../PortableTypes.h" 			// define F16, F32, U16, U32 etc...
#include "../Print/PrintStream.h"		// pout
#include "../SignalSlot/Signal.h"		// eventDispatchers
#include "../Event/ClockEvent.h"

//== Defines & Macros ===============================

#ifdef __PRINT_MANAGER__
	#define CLOCK_DEBUG_( msg ) 	POUT_DEBUG( V_INFORMATION	,0, msg );
	#define CLOCK_DEBUG( msg ) 		POUT_DEBUG( V_INFORMATION	,0, msg << std::endl );
	#define CLOCK_DEBUG_W( msg ) 	POUT_DEBUG( V_WARNING		,0, msg << std::endl );
#else
	#define CLOCK_DEBUG_( msg ) 	//std::cerr << msg;
	#define CLOCK_DEBUG( msg ) 		//std::cerr << msg << std::endl;
	#define CLOCK_DEBUG_W( msg )	//std::cerr << "Warning: " << msg << std::endl;
#endif

#define TIME_SCALE_PRECISION 1000 // CLOCKS_PER_SEC or 1000 ? --> CLOCKS_PER_SEC return 1.000.000 on Linux and 1.000 on Win32
#define TIME_SCALE_PRECISION_SQUARE TIME_SCALE_PRECISION * TIME_SCALE_PRECISION

namespace hbt /// [home brew tools]
{
//*************************************************************************************************/
///// Clock ///////////////////////////////////////////////////////////////////////////////////////

	/** This class provides a high-resolution timer and the tools to control it.
		@ingroup clock */
		
	class Clock
	{
		public:
			Clock();
			///< A constructor.
			~Clock();
			///< A destructor.
			
		//== Public functions ===============================
			
			virtual void update( const F32 = 0.0f );
			///< Updates the Clock values using the deltaTime passed in seconds.
			/// @param deltaTime The time elapsed in seconds since the last update call.
			virtual void singleStep( const I16 = 1 );
			///< Sets a mini update of 1/30 seconds. @warning This does apply only if the pause state is TRUE.
			/// @param numSteps The number of single step to apply at once.
			
		//== Getters & Setters ==============================
			
			F64 getCycles() const;
			///< Returns the number of cycles elapsed since the instantiation of the Clock.
			/// @return The number of cycles elapsed.
			F32 getCyclesSinceLastUpdate() const;
			///< Returns the number of cycles elapsed between the two last update calls.
			/// @return The number of cycles elapsed.
			
			F32 getTime() const;
			///< Returns the number of seconds elapsed since the instantiation of the Clock.
			/// @return The time elapsed in seconds.
			F32 getTimeSinceLastUpdate() const;
			///< Returns the number of seconds elapsed between the two last update calls.
			/// @return The time elapsed in seconds.
			
			void setTimeScale( const F32 );
			///< Sets the Clock time scale.
			/// @param timeScale The updated time scale value.
			F32  getTimeScale() const;
			///< Returns the Clock time scale.
			/// @return The current time scale value.
			
			void setPaused( const bool );
			///< Sets the Clock pause state.
			/// @param isPaused The updated pause state.
			bool isPaused() const;
			///< Returns the Clock pause state.
			/// @return The current pause state.
			
		//== Public Statics =================================
			
			static U32 readHiResTimer();
			///< Returns the current number of CPU cycles elapsed since the program started.
			/// @return The current number of CPU cycles elapsed.
			/// @warning Currently not inlined because it should become more complex when replacing clock() @warning Furthermore, this should be U64 when it will really be high res...
			
			static U32 readHiResTimerFrequency();
			///< Returns the frequency of CPU cycles per seconds.
			/// @return The frequency of CPU cycles per seconds.
			/// @warning Currently not inlined because it should become more complex when replacing CLOCKS_PER_SEC @warning Furthermore, this should be U64 when it will really be high res...
	
			static inline F64 secondsToCycles( F64 deltaTime )
			{
				return (F64) (deltaTime * s_cyclesPerSecond);
			}
			///< Transforms seconds in CPU cycles.
			/// @param deltaTime The time in seconds to convert.
			/// @return The corresponding number of cycles.
			
			static inline F32 cyclesToSeconds( F64 cycles )
			{
				return (F32)cycles / s_cyclesPerSecond;
			}
			///< Transforms the CPU cycles number in seconds (best with small values).
			/// @param cycles The number of cycles to convert.
			/// @return The corresponding time in seconds.
			
		//== Friends ========================================
			
			friend class ClockManager;
			
		protected:
			static U32 	s_cyclesPerSecond;						//[4] 4 bytes (static)
			static I32 	s_timeScale;							//[4] 4 bytes (static)
			static bool	s_isPaused;								//[1] 1 byte  (static) | 3 bytes padding
			
												// VIRTUAL		//[4] 4 bytes
			bool 	m_isPaused;									//[1] 1 byte
			bool 	m_isUpToDate; 								//[1] 1 byte
			bool	m_isManaged;								//[1] 1 byte  | 1 byte padding
			F64 	m_cpuCycles;								//[8] 8 bytes
			F32 	m_cyclesElapsed;							//[4] 4 bytes
			I32 	m_timeScale;								//[4] 4 bytes | 0 byte padding
	};//--------------------------------------------------------->[8] 24 bytes
	
	
//*************************************************************************************************/
///// Examples ////////////////////////////////////////////////////////////////////////////////////

/*! @class Clock
@code

	==> This is an example of how to use the Clock without ClockManager. <==

	#include "Clock.h"

	//== Opening calls

	// Initializes the Clock
		Clock myClock;

	//== Recursive calls

	while(...)
	{
		// Updates the Clock using the time elapsed since the last update call (once per loop !)
			myClock.update( timeInSecondsSinceLastUpdateCall );
	}

	//== Recursive AND/OR event calls

	// Returns the Clock informations related to the time and cycles elapsed
		std::cout
		<< " Time since start: " << myClock.getTime()
		<< " Cycles since start: " << myClock.getCycles()
		<< " Time since last update: " << myClock.getTimeSinceLastUpdate()
		<< " Cycles since last update: " << myClock.getCyclesSinceLastUpdate()
		<< std::endl;
		
	// Reverses the pause state using setPaused() and isPaused()
		myClock.setPaused( !myClock.isPaused() );
		
	// Updates the Clock time scale by 1.0f using setTimeScale() and getTimeScale()
		myClock.setTimeScale( myClock.getTimeScale() + 1.0f );
		
	// Updates the Clock by 1/30 of seconds at each call of the singleStep()
		myClock.singleStep();	

@endcode */

//*************************************************************************************************/
///// Important ///////////////////////////////////////////////////////////////////////////////////

/*! @class Clock
\n @verbatim

Useful information for Hi Resolution Timer...

[ Game Engine Architecture p314-316 ]

To deal with time in the MAIN GAME LOOP read from 7.5.2.2 to 7.5.2.5$
   . Updating based on elpased time
   . Avoid the "last frame's delta as an estimate of the upcoming frame" problem using :
	- a running average over the last few frames.
	- a frame rate governed using sleep to guarantee it.
   . Avoid tearing effect by waiting for the v-sync interval (reset of the monitor electron gun)

[ Game Engine Architecture p317-324 ]

///// High Resolution Timer ///////////////////////////////////////////////////////////////////////

All modern CPUs contain a high-resolution timer, which is usually implemented as a hardware
register that counts the number of CPU cycles that have elapsed since the last time the processor
was powered on or reset. This is the timer that we should use when measuring elapsed time in game.

Different microprocessors and different operating systems provide different ways to query the
hight-resolution timer. (Pentium: rdtsc, PowerPC: mftb / mfspr ...)

///// High-Resolution Clock Drift /////////////////////////////////////////////////////////////////

On some multicore processors, the high-resolution timers are independant on each core, and they can
(and do) drift apart. If you try to compare absolute timer readings taken from on different cores
to one another, you might end up with some strange results, even negative time deltas.

///// Time Units and Clock Variables //////////////////////////////////////////////////////////////

A CPU's high-resolution timer is 64 bits wide on most processors, to ensure that it won't wrap too
often. On a 3Ghz Pentium processor that updates its high-res timer once per CPU cycle, the
register's value will wrap back to zero once every 195 years with a 64-bit integer clock, and every
1.4 seconds with a 32-bit integer clock.

. 64-Bit Integer Clocks: This is the most flexible time representation, presuming you can afford 64
bits worth of storage.

. 32-Bit Integer Clocks: Useful when measuring relativly short durations with high-res. For example,
to profile the performancce of a block of code, we can store the delta-t with a 32-bit integer, but
we must use 64-bit integer to compare start and end timer.
	U64-U64 = U32

. 32-Bit Floating-Point Clocks: This is usually only used to store relatively short time deltas,
measuring at most few minutes, and more often just a single frame or less. If an absolute-valued
floating-point clock is used in a game,	you will need to reset the clock to zero periodically, to
avoid accumulation of large magnitudes. To do this we simply multiply a duration measured in CPU
cycles by the CPU's clock frequency which is in cycles per second (CLOCKS_PER_SEC etc.).
	U64-U64 = F32

. Game-defined unit: Some game engines allow timing values to be specified in a game-defined unit
that is fine-grained enough to permit a 32-bit integer format to be used, precise enough to be
useful for wide range of applications within the engine (not all; like time-scaling animation),
and yet large enough that the 32-bit clock won't wrap too often. One common choice is 1/300 second
time unit. This is fine-grained enough for many purposes, wraps once every 165.7 days and it is an
even multiple of both NTSC (60 fps) and PAL/SECAM (50 fps) refresh rates.

///// Dealing with Break Points ///////////////////////////////////////////////////////////////////

When your game hits a break point, its loop stops running and the debuger takes over. However, the
CPU continues to run, and the real-time clock continues to accrue cycles. A simple approach can be
used to get around this problem. In the main game loop, if we ever measure a frame time in excess
of some prefedined upper limit (e.g. 1/10 of a sec) we can assume that we have just resumed
execution after a break point, and we set the delta time artificially at whatever the target frame
rate is (1/30, 1/60 ...)

	if( dt > 1.0f / 10.0f ) dt = 1.0f / 30.0f;

///// Selection of the time variable type /////////////////////////////////////////////////////////

unsigned int : 0 to 4.294.967.295
with 1000 CLOCKS_PER_SEC we need :
	--> 	    60.000  per min
	--> 	 3.600.000  per hour
	-->     864.00.000  per day
	--> ~2.678.400.000  per month
	--> 31.536.000.000  per year
@endverbatim */

} // hbt end

#endif
