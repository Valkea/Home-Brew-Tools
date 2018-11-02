/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 04.05.2011
	Description : This class provides some small classes to connect between signal and slots in
				  order to manipulate transmited values.

///// Class list //////////////////////////////////////////////////////////////////////////////////
	- xxx					[.] . bytes
	
///// ETA /////////////////////////////////////////////////////////////////////////////////////////
	
** Bugs	- none ATM
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_SIG_MANIP
#define HEADER_SIG_MANIP

#include "Signal.h"

namespace hbt
{
//*************************************************************************************************/
///// BoolInverter ////////////////////////////////////////////////////////////////////////////////

	/** This class provides a simple way to invert a bool value transmitted through Signals & Slots system.
		@code
		
		#include "hbt/SignalSlot/Signal.h"
		#include "hbt/SignalSlot/SignalManipulators.h"
			
		// Defines some functions, methods or functors
			void theFunction ( bool ) { ... };
			struct AnyClass	{ void theMethod ( bool ) { ... } };
			struct AnyFunctor { void operator () ( bool ) { ... } };
			
		// Initializes variables
			AnyClass theClassInstance;
			AnyFunctor theFunctorInstance;
		
			hbt::BoolInverter myInverter;
			hbt::Signal<void(bool)> onBoolEmit;
		
		// Connects the bool inverter to a Signal emitting a bool value.
			onBoolEmit.connect( myInverter );
		
		// Connects some Slots to the BoolInverter Signal.
			myInverter.out.connect( &theFunction );
			myInverter.out.connect( theClassInstance, &AnyClass::theMethod );
			myInverter.out.connect( theFunctorInstance );
			
		// Fires the Signal
			onBoolEmit( true ); // The three slots will receive false.
		
		@endcode
		@ingroup signalmanipulator*/
		
	struct BoolInverter
	{
		void operator() ( bool value )
		{
			out( !value );
		}
		///< Connect this part to a Signal emitting bool.
		/// @param value The bool value to invert.
		
		Signal< void(bool) > out;
		///< Connect slots waiting for the inverted value on this Signal.
	};
	
//*************************************************************************************************/
///// ValueRestricter ////////////////////////////////////////////////////////////////////////////////////
	
	/** This class provides a simple way to restrict values transmitted through Signals & Slots system to the given minimum and maximum values.
		@code
		
		#include "hbt/SignalSlot/Signal.h"
		#include "hbt/SignalSlot/SignalManipulators.h"
			
		// Defines some functions, methods or functors
			void theFunction ( int ) { ... };
			struct AnyClass	{ void theMethod ( int ) { ... } };
			struct AnyFunctor { void operator () ( int ) { ... } };
			
		// Initializes variables
			AnyClass theClassInstance;
			AnyFunctor theFunctorInstance;
		
			hbt::ValueRestricter<int> myRestricter(0,150);
			hbt::Signal<void(int)> onIntEmit;
		
		// Connects the integer restricter to a Signal emitting an integer value.
			onIntEmit.connect( myRestricter );
		
		// Connects some Slots to the ValueRestricter Signal.
			myRestricter.out.connect( &theFunction );
			myRestricter.out.connect( theClassInstance, &AnyClass::theMethod );
			myRestricter.out.connect( theFunctorInstance );
			
		// Fires the Signal
			onIntEmit( 1792 ); // The three slots will receive 150.
		
		@endcode
		@tparam T The type of the value to restrict.
		@ingroup signalmanipulator*/
		
	
	template< class T >
	struct ValueRestricter
	{
		ValueRestricter( T capMin, T capMax ) : capMin(capMin), capMax(capMax) {};
		///< A constructor.
		/// @param capMin This is the maximum cap value.
		/// @param capMax This is the minimum cap value
	
		void operator() ( T i )
		{
			if( i < capMin )
				out( capMin );
			else if( i > capMax )
				out( capMax );
			else
				out( i );			
		}
		///< Connect this part to a Signal emitting T type.
		/// @param value The value to restrict.
		
		Signal< void(T) > out;
		///< Connect slots waiting for the inverted value on this Signal.
		
		T capMax;
		T capMin;
	};
}
#endif

