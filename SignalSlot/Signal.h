/*
	Auteur : Letremble Emmanuel
	Date : 13.02.2011
	Description : This class should provide an easy way to dispatch parameters to callbacks.

	- ISignal		[4] 4 bytes
	- SignalManager	[4] 16 bytes
	- Signal		[4] 20 bytes with delayed / 8 bytes without delayed
	
///// ETA /////////////////////////////////////////////////////////////////////////////////////////
	
** ToDo - Unregisters a Delegate if the pointed class instance member of functor does not exists
		  anymore.
		  
** ToDo - Should we support connect for const methods ? [Should we convert const methods
		  using: "new MethodDelegate<C,Tr,Args...>( target, (Tr(C::*)(Args...))method )" ? ]
		  Attention: There is a problem when registering const function with this method...
		  The tuples are not deleted !
		  
** ToDo - (OPTIONAL) Should I add a SlotGrapes class to handle multiple Slots at once ?
		  This way we can first create slot grapes and signals, and then easily automate connections
		  and disconnections of slots. (This system refer to my old Signal and Slot system)
		  
** ToDo - (OPTIONAL) Should we move the m_delayedTuples from Signal to SignalManager ?
		  Signal would change from [4]20 to [4]12 if ISignal is still required or [4]8 if
		  ISignal is not required anymore.
		  From: std::list< std::pair< U32, std::tuple<Args...> > > m_delayedTuples
		  To:   std::list< std::tuple< U32, ISignal*, std::tuple<Args...> > > m_delayedTuples
		  Or:   std::list< std::pair< U32, std::pair<ISignal*, std::tuple<Args...> > > > m_delayedTuples
		  
** ToDo - (OPTIONAL) Should we do something with the returned values registered to Signals by non
		  void functions, functors or methods ? Add the return value support by returning a vector
		  of the T value. One value for each connected Delegate... std::vector< Tr >
		  see : http://www.boost.org/doc/libs/1_46_1/doc/html/signals/tutorial.html#id2683230
		  
** ToDo - (OPTIONAL) Add a m_blockIt member to Dispatchers to enable blocking them ?
		
** ToDo - (OPTIONAL) Add the reentrance for multithread support
		
** Bugs	- none ATM

***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef HEADER_SIGNALS_SLOTS
#define HEADER_SIGNALS_SLOTS

#include <iostream>					// cout
#include <list>

#include "../PortableTypes.h" 		// define F16, F32, U16, U32 etc...
#include "../Print/PrintStream.h"	// pout
#include "../Delegate.h"			// IDelegate / MethodDelegate / FunctionDelegate / FunctorDelegate

//== Defines & Macros ===============================

#ifdef __PRINT_MANAGER__
	#define EVENT_DEBUG_( msg ) 	POUT_DEBUG( V_INFORMATION	,1, msg );
	#define EVENT_DEBUG( msg ) 		POUT_DEBUG( V_INFORMATION	,1, msg << std::endl );
	#define EVENT_DEBUG_W( msg ) 	POUT_DEBUG( V_WARNING		,1, msg << std::endl );
#else
	#define EVENT_DEBUG_( msg ) 	//std::cerr << msg;
	#define EVENT_DEBUG( msg ) 		//std::cerr << msg << std::endl;
	#define EVENT_DEBUG_W( msg )	//std::cerr << "Warning: " << msg << std::endl;
#endif

	/** This enables the delayed events.
		If this is enabled, the Signal will accept delayed events and Signal will have the "operator( delay, args... )" enabled.\n Defined in Signal.h
		@warning The cost of this, is that Signal is 20 bytes with delayed events, and 8 bytes without them.
		@ingroup signalandslot */
		#define ENABLE_DELAYED_SIGNALS false
		
		#if ENABLE_DELAYED_SIGNALS
			#include "TupleUnpack.h"
			#include <time.h>				// CLOCKS_PER_SEC
		#endif
		
namespace hbt /// [home brew tools]
{
	/** This forces delayed events to synchronize on the expected time instead of real time.
		This way, if for any reason there is a big "lag" between two SignalManager updates, it will try to register and fire already passed delayed events until the current time is reached.
		@verbatim Example: Suppose that a function initializes a delayed event to call itself 1 sec later. If there is a "big" lag of say 4.5 sec, the function will fire 4 times before the next update.@endverbatim
		If this side effect is not required, ENABLE_FORCE_SYNC_NO_REWIND should be used.\n\n Defined in Signal.h
		@warning This define is useful ONLY when a delayed event calls a function that initializes (a) delayed event(s). @warning This require ENABLE_DELAYED_SIGNALS to be enabled in Signal.h
		@ingroup signalandslot*/
		#define ENABLE_FORCE_SYNC true

	/** This disables the synchronized delayed events ability to register in past time.
		This way, as ENABLE_FORCE_SYNC is enabled, it will try to lower at maximum the lost time, but will never register lower than present time.
		@verbatim Example: Suppose that a function initializes a delayed event to call itself 1 sec later. If there is a "big" lag of say 4.5 sec, the function will fire 1 time before the next update and will initialize the next one as soon as possible (present time + 1 ms)@endverbatim Defined in Signal.h
		@warning This define is useful ONLY when a delayed event calls a function that initializes (a) delayed event(s). @warning This require ENABLE_DELAYED_SIGNALS and ENABLE_FORCE_SYNC to be enabled in Signal.h
		@ingroup signalandslot */
		#define ENABLE_FORCE_SYNC_NO_REWIND true
		
	/** This enables the life cycles on the Delegates.
		If this is enabled, the "lifeCycles" parameter on the "connect" methods is taken into account, thus enabling the connected functor, function or method to be automatically disconnected from the Signal when the defined cycle number is reached.\n\n Defined in Signal.h
		@warning The current maximum value for lifeCycles of a delegate is 65535 (U16), if you need more than that, you need to change the "lifeCycles" variables in the Signal.h file from U16 to U32 (or U64), and to do the same with the 3rd value of the "T_EventHandler" typedef.
		@ingroup signalandslot */
		#define ENABLE_DELEGATE_LIFECYCLES true

	#ifndef HBT_ENUM_PRIORITY
	#define HBT_ENUM_PRIORITY
	/** These are the default levels of priority.
		But we can use up to 256 different levels.
		@ingroup signalandslot */
		enum Priority	{
							P_VLOW,		///< 0: Very low priority
							P_LOW, 		///< 1: Low priority
							P_NORMAL,	///< 2: Standard priority
							P_HIGH,		///< 3: High priority
							P_VHIGH		///< 4: Very high priority
						};
	#endif

	#if __GNUC__ // Based upon __PRETTY_FUNCTION__
		std::string getSignalSignature( std::string tPF, std::string openStr );
	#endif	
	
//*************************************************************************************************/
///// ISignal /////////////////////////////////////////////////////////////////////////////////////

#if ENABLE_DELAYED_SIGNALS || __DOXYGEN__

	/** This abstract class aggregates all Signal types under a unique type.
		@ingroup signalandslot */

	struct ISignal
	{
		ISignal() {};
		virtual ~ISignal() {};
		virtual U32 fireDelayed( const U32 updateTime ) = 0;
		///< Dynamically calls the fireDelayed method of the Signal inheritors.
		/// @param updateTime The time to compares with the first Delayed Signal fire time, registered in the Delayed Signal list.
	
												// VIRTUAL		//[4] 4 bytes
	};//--------------------------------------------------------->[4] 4 bytes
	
#endif	
	
//*************************************************************************************************/
///// SignalManager ///////////////////////////////////////////////////////////////////////////////

#if ENABLE_DELAYED_SIGNALS || __DOXYGEN__

	/** This singleton class manages and fires delayed events.
		@ingroup signalandslot */
	
	class SignalManager
	{
		typedef std::pair< ISignal*, U32 > T_DelayedEvent;
		
		public:
			static SignalManager& getInstance()
			{
				static SignalManager singleton;
				return singleton;
			}
			///< Initializes and/or returns the %SignalManager singleton instance.
			/// @return The %SignalManager singleton instance.
			
			~SignalManager();
			///< A destructor.
			
			void update( const F32 seconds );
			///< Updates the current internal time using the given time in seconds. At the same time this will call for the delayed signals that are ready to go.
			/// @param seconds The time elapsed in seconds since the last update call.
			
			void updateC( const F32 cycles ); 
			///< Updates the current internal time using the given time in CPU cycles. At the same time this will call for the delayed signals that are ready to go.
			/// @param cycles The number of CPU cycles elapsed since the last updateC call.
			
			void registerDispatcher( ISignal* sig, const U32 nextUpdateTime );
			///< Registers a new Signal instance to the dispatch list, or updates an already registered Signal.
			/// @param sig The Signal instance to register or update. @param nextUpdateTime The time (in ms) of the next delayed event of the Signal instance targeted.
			/// @warning This function should be used only by the Signal class itself. But as partially specialized template classes can't be defined as friend...
			
			void unregisterDispatcher( const ISignal* sig );
			///< Unregisters an Signal instance from the dispatch list.
			/// @param sig The Signal instance to unregister.
			/// @warning This function should be used only by the Signal class itself. But as partially specialized template classes can't be defined as friend...
			
		private:	
			SignalManager();
			
			void checkNextDelayedSignal();
			
			struct sortByTime
			{ bool operator()(const T_DelayedEvent& lhs, const T_DelayedEvent& rhs) const { return lhs.second < rhs.second; }	};
			
		public:
			F32 timeBucket;										//[4] 4 bytes	//* in milliseconds -> ~end of the world maximum... I will try to live as long as this var ;)
			U16 timeLost; 										//[2] 2 bytes	//* in milliseconds -> ~65.5 seconds maximum			
		private:	
			bool m_isEmpty;										//[1] 1 byte
			bool m_isFiring;									//[1] 1 byte
			std::list< T_DelayedEvent > m_IEventDispatcherList;	//[4] 8 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 16 bytes
	
	extern SignalManager& signalManager;
	///< A reference to the SignalManager singleton instance.
	///  This enables a quick and easy access to the SignalManager.
	/// @ingroup signalandslot
	
#endif
	
//*************************************************************************************************/
///// Signal /////////////////////////////////////////////////////////////////////////////
	
	template <class Tr, class... Args> struct Signal : public Signal<Tr(Args...)> {};	// Required for function without argument.
	
	/** This class can be connected to functions, functors and methods in order to dispatch event calls.
		@code
			==> This is an example of how to use the Signal with and without delayed signals. <==
			
			#include "hbt/SignalSlot/Signal.h"
			
			// Defines some functions, methods or functors
				void theFunction ( int ) { ... };
				struct AnyClass	{ void theMethod ( int ) { ... } };
				struct AnyFunctor { void operator () ( int ) { ... } };
			
			// Initializes variables
				AnyClass theClassInstance;
				AnyFunctor theFunctorInstance;
				
			//== Opening calls
				
			// Initializes some Signals
				hbt::Signal<void(int)> onUpdateInteger1;
				hbt::Signal<void(int)> onUpdateInteger2;
				
			// Connects a Signal to a function Slot
				onUpdateInteger1.connect( &theFunction );
				
			// Connects a Signal to a method Slot with a different priority setting.
				onUpdateInteger1.connect( theClassInstance, &AnyClass::theMethod, hbt::P_VHIGH );
				
			// Connects a Signal to a functor Slot with a different priority setting and a life cycle setting of 10.
				onUpdateInteger1.connect( theFunctorInstance, hbt::P_LOW, 10 );
				
			// Connects a Signal to another Signal.
				onUpdateInteger2.connect( onUpdateInteger1 );
				
			//== Recursive AND/OR event calls
				
			// Fires the both Signals immediately
				onUpdateInteger1( 10 );
				onUpdateInteger2( 15 );
			
			// Fires the first Signal 5 seconds later
				onUpdateInteger1( 5000, 20 );
				
			// Fires the second Signals 10.5 seconds later
				onUpdateInteger2( 10500, 35 );
				
			//== Closing calls
			
			// Disconnects few Slots
				onUpdateInteger1.disconnect( theClassInstance, &AnyClass::theMethod );
				onUpdateInteger1.disconnect( theFunctorInstance );
				
			// Disconnects all Slots
				onUpdateInteger1.disconnectAll();
				onUpdateInteger2.disconnectAll();
				
		@endcode
		@tparam Tr The return type of the assigned function, functor or method. @tparam Args The argument list of the assigned functions, functors or methods.
		@ingroup signalandslot */
	
	template <class Tr, class... Args>
	class Signal<Tr(Args...)>
	#if ENABLE_DELAYED_SIGNALS
		: public ISignal
	#endif
	{
		#if ENABLE_DELAYED_SIGNALS
			typedef std::list<std::pair<U32, std::tuple<Args...>* >> 	T_DelayedEvent;
		#endif
		#if ENABLE_DELEGATE_LIFECYCLES
			typedef std::tuple< IDelegate<Args...>*, U8, U16 > 			T_EventHandler;
		#else
			typedef std::tuple< IDelegate<Args...>*, U8 > 				T_EventHandler;
		#endif
		typedef std::list< T_EventHandler > 							T_EventHandlerList;
		
		public:
			Signal()
			{ EVENT_DEBUG("Signal"); }
			///< A constructor.
			/// This is the default constructor.
			
			Signal( const Signal<Tr(Args...)>& obj ) : m_IDelegateList(obj.m_IDelegateList)
			{ EVENT_DEBUG("Signal COPY");
			
				typename T_EventHandlerList::iterator it;
				for( it = m_IDelegateList.begin(); it != m_IDelegateList.end(); ++it )
				{
					++std::get<0>(*it)->count;
					EVENT_DEBUG("[#" << std::get<0>(*it)->count << "] Deep copy " << &m_IDelegateList << " from " << &obj.m_IDelegateList);
				}
			}
			///< A copy constructor.
			/// @param obj The Signal instance to copy.
			/// @warning We do not copy the delayed events (m_delayedTuples) because it doesn't seem logic, and because this signal is not registered to the signalManager anyway.
			
			~Signal()
			{ EVENT_DEBUG("~Signal::m_IDelegateList.size(" << m_IDelegateList.size() << ")");
			
				disconnectAll();
				
			#if ENABLE_DELAYED_SIGNALS || __DOXYGEN__ //<---- ToDo est on certain de l'ajout de cette ligne ?
				while( !m_delayedTuples.empty() )
				{					
					delete std::get<1>(m_delayedTuples.front());
					std::get<1>(m_delayedTuples.front()) = NULL;
					m_delayedTuples.pop_front();
				}
			#endif
			}
			///< A destructor.
			
		//== Public functions ===============================
		
		struct sortByPriority
		{
			bool operator()(const T_EventHandler& lhs, const T_EventHandler& rhs) const { return std::get<1>(lhs) < std::get<1>(rhs); }
		};
		
		private:
			bool connect( IDelegate<Args...>* ideleg, const U16 lifeCycles, const U8 priority )
			{ EVENT_DEBUG("Signal::connect::IDelegate");
				
				typename T_EventHandlerList::iterator it;
				for ( it = m_IDelegateList.begin(); it != m_IDelegateList.end(); ++it)
				{
					if( *std::get<0>(*it) == *ideleg )
					{
						std::get<1>(*it) = priority;
						#if ENABLE_DELEGATE_LIFECYCLES
							if( lifeCycles ) std::get<2>(*it) = lifeCycles;
						#endif
						EVENT_DEBUG("Signal::connect::IDelegate::Update-Priority/LifeCycles");
						
						delete ideleg;
						ideleg = NULL;
						return false;
					}
				}
	
				#if ENABLE_DELEGATE_LIFECYCLES
					m_IDelegateList.push_back( std::make_tuple( ideleg, priority, lifeCycles ) );
				#else
					m_IDelegateList.push_back( std::make_tuple( ideleg, priority ) );
				#endif
				if( std::get<0>(m_IDelegateList.back())->count == 0 )	++std::get<0>(m_IDelegateList.back())->count;
				m_IDelegateList.sort( sortByPriority() );
				return true;
			}
			///< Sets this Signal to call the provided IDelegate when fired.
			/// @param ideleg The IDelegate to call when this Signal is fired. @param lifeCycles (Optional) The number of time this IDelegate can be called before being deleted (default 0 is unlimited). @param priority (Optional) The priority of the connected IDelegate in the dispatch list of this Signal.
			/// @return Returns true if the IDelegate was not already connected, otherwise it returns false.
			
		public:	
			bool connect( Tr(*func)(Args...), const U16 lifeCycles = 0, const U8 priority = P_NORMAL )
			{ EVENT_DEBUG("Signal::connect::FunctionDelegate");
			
				return connect( new FunctionDelegate<Tr,Args...>( func ), lifeCycles, priority );
			}
			///< Sets this Signal to call the provided function when fired.
			/// @param func The function to call when this Signal is fired. @param lifeCycles (Optional) The number of time this Function can be called before being deleted (default 0 is unlimited). @param priority (Optional) The priority of the connected Function in the dispatch list of this Signal.
			/// @return Returns true if the function was not already connected, otherwise it returns false.
		
			template < class C >
			bool connect( C& target, Tr(C::*method)(Args...), const U16 lifeCycles = 0, const U8 priority = P_NORMAL )
			{ EVENT_DEBUG("Signal::connect::MethodDelegate");
			
				return connect( new MethodDelegate<C,Tr,Args...>( target, method ), lifeCycles, priority );
			}
			///< Sets this Signal to call the provided class instance and method when fired.
			/// @param target The class member instance used to call the method. @param method The class member method to call when this Signal is fired. @param lifeCycles (Optional) The number of time this Method can be called before being deleted (default 0 is unlimited). @param priority (Optional) The priority of the connected Method in the dispatch list of this Signal.
			/// @tparam C The type of the class instance associated to the registered method.
			/// @return Returns true if the method was not already connected, otherwise it returns false.
			
			template < class C >
			bool connect( C& functor, const U16 lifeCycles = 0, const U8 priority = P_NORMAL )
			{ EVENT_DEBUG("Signal::connect::FunctorDelegate");
			
				return connect( new FunctorDelegate<C,Args...>( functor ), lifeCycles, priority );
			}
			///< Sets this Signal to call the provided functor when fired.
			/// @param functor The functor to call when this Signal is fired. @param priority (Optional) The priority of the connected Functor in the dispatch list of this Signal. @param lifeCycles (Optional) The number of time this Functor can be called before being deleted (default 0 is unlimited).
			/// @tparam C The type of the functor.
			/// @return Returns true if the functor was not already connected, otherwise it returns false.
			
			
		private:
			bool disconnect( const IDelegate<Args...>* ideleg )
			{ EVENT_DEBUG("Signal::disconnect::IDelegate");
			
				typename T_EventHandlerList::iterator it;
				for( it = m_IDelegateList.begin(); it != m_IDelegateList.end(); ++it )
				{
					if( *std::get<0>(*it) == *ideleg )
					{
						if( --std::get<0>(*it)->count == 0 )
						{
							delete std::get<0>(*it);
							std::get<0>(*it) = NULL;	
						}
						
						m_IDelegateList.erase( it );
						return true;
					}
				}
				return false;
			}
			///< Disconnects the provided IDelegate from this Signal.
			/// @param ideleg The IDelegate to disconnect.
			/// @return Returns true if the IDelegate was found and disconnected, otherwise it returns false.
			
		public:
			bool disconnect( Tr(*func)(Args...) )
			{ EVENT_DEBUG("Signal::disconnect::FunctionDelegate");
			
				FunctionDelegate<Tr,Args...> tmp( func );
				return disconnect(&tmp);
			}
			///< Disconnects the provided function from this Signal.
			/// @param func The function to disconnect.
			/// @return Returns true if the function was found and disconnected, otherwise it returns false.
			
			template < class C >
			bool disconnect( C& target, Tr(C::*method)(Args...) )
			{ EVENT_DEBUG("Signal::disconnect::MethodDelegate");
			
				MethodDelegate<C,Tr,Args...> tmp( target, method );
				return disconnect(&tmp);
			}
			///< Disconnects the provided method from this Signal.
			/// @param target The class member instance used to call the method. @param method The class member method to disconnect.
			/// @tparam C The type of the class instance associated to the registered method.
			/// @return Returns true if the method was found and disconnected, otherwise it returns false.
			
			template < class C >
			bool disconnect( C& functor )
			{ EVENT_DEBUG("Signal::disconnect::FunctorDelegate");
			
				FunctorDelegate<C,Args...> tmp( functor );
				return disconnect(&tmp);
			}
			///< Disconnects the provided functor from this Signal.
			/// @param functor The functor to disconnect.
			/// @tparam C The type of the functor.
			/// @return Returns true if the functor was found and disconnected, otherwise it returns false.
			
			void disconnectAll()
			{ EVENT_DEBUG("Signal::disconnectAll");
			
				#if ENABLE_DELAYED_SIGNALS
					signalManager.unregisterDispatcher( this );
				#endif
				typename T_EventHandlerList::iterator it = m_IDelegateList.begin(), it2;
				while( it != m_IDelegateList.end() )
				{
					it2 = it++;
					if( --std::get<0>(*it2)->count == 0 )
					{
						delete std::get<0>(*it2);
						std::get<0>(*it2) = NULL;
					}
					m_IDelegateList.erase( it2 );
				}
			}
			///< Disconnects all functions, functors and methods registered to this Signal.


			void operator() ( Args... args )
			{ EVENT_DEBUG("Signal::operator ()");
			
				typename T_EventHandlerList::iterator it = m_IDelegateList.begin(), itDel;
				while( it != m_IDelegateList.end() )
				{
					itDel = it++;
					(*std::get<0>(*itDel))( args... );
					
					#if ENABLE_DELEGATE_LIFECYCLES
						if( std::get<2>(*itDel) > 0 ) // Verifies and updates LifeCycles
							if( --std::get<2>(*itDel) == 0 ) disconnect( static_cast<IDelegate<Args...>*>(std::get<0>(*itDel)) );
					#endif
				}
			}
			///< Fires connected functions, functors and methods using the provided argument values. This enables calling Signal just like any function using MySignalName( values... ); @code hbt::Signal<void(int)> myIntSignal;     myIntSignal.connect( mySlotWaitingForInt );      myIntSignal( 10 ); @endcode
			/// @param args The argument values to fire.
			
		#if ENABLE_DELAYED_SIGNALS || __DOXYGEN__
		
			void operator() ( U32 delay, Args... args )
			{ EVENT_DEBUG("Signal::fireLater ( delay = " << delay << ") \t| timeBucket (" << signalManager.timeBucket << ") \t| timeLost (" << signalManager.timeLost << ")");
				
				if( delay > 0 )
				{
					delay += signalManager.timeBucket;
					
					#if ENABLE_FORCE_SYNC
						delay -= signalManager.timeLost;
						#if ENABLE_FORCE_SYNC_NO_REWIND
							if( delay < signalManager.timeBucket ) delay = signalManager.timeBucket + 1;
						#endif
					#endif
					
					typename T_DelayedEvent::iterator it;
					for( it = m_delayedTuples.begin(); it != m_delayedTuples.end(); ++it )
						if( it->first > delay ) break;
					
					if( it == m_delayedTuples.begin() )
						signalManager.registerDispatcher( this, delay );
					
					m_delayedTuples.insert( it, std::make_pair(delay, new std::tuple<Args...>(args...) ) ); // new avoid multiple copies
				}
				else
					(*this)( args... );
			}
			///< Fires connected functions, functors and methods after a given time using the provided argument values. This enables calling Signals just like any function using MySignalName( delay, values... ); @code hbt::Signal<void(int)> myIntSignal;     myIntSignal.connect( mySlotWaitingForInt );      myIntSignal( 5000, 10 ); @endcode
			/// @param delay The time to wait in milliseconds before firing the Signal with the provided argument values and/or references. @param args The argument values to fire with.
			/// @warning This require the ENABLE_DELAYED_SIGNALS define in the Signal.h
		#endif
			
			
			bool operator == ( const Signal<Tr(Args...)>& obj ) const
			{ EVENT_DEBUG("Signal::operator ==");
			
				return ((*m_IDelegateList) == *(obj.m_IDelegateList));
			}
			///< Verifies equality with another Signal using the assigned callback functions, functors and methods.
			/// @param obj The Signal to compare with.
			/// @return The equality test value.
			
			std::ostream& toOstream( std::ostream& os ) const
			{
			#if __GNUC__
				os << getSignalSignature(__PRETTY_FUNCTION__, "Signal") << " [ ";
			#else
				os << "Signal [ ";
			#endif
				os << "slots: " << m_IDelegateList.size() << ", ";
			#if ENABLE_DELAYED_SIGNALS
				os << "delayed events in queue: " << m_delayedTuples.size() << ", ";
			#endif
				os << "address: " << this << ", ";
				os << "arguments: " << sizeof...(Args) << ", ";
				os << "type: " << typeid(*this).name();
				os << " ]";
				return os;
			}
			///< Prints informations about the Signal. This can be used by printing the Signal in a traditionnal way: @code std::cout << mySignal << std::endl; @endcode
			/// @param os The ostream to insert Signal information in.
			/// @return The modified ostream.
			
		private: 
			
		#if ENABLE_DELAYED_SIGNALS || __DOXYGEN__
			U32 fireDelayed( const U32 updateTime )
			{ EVENT_DEBUG("Signal::fireDelayed(" << updateTime << ")");
				
				std::list< std::tuple<Args...>* > tmpTuples;
				while( (m_delayedTuples.size() > 0) && ( std::get<0>(m_delayedTuples.front()) <= updateTime ) )
				{
					tmpTuples.push_back( std::move( std::get<1>(m_delayedTuples.front()) ) );
					m_delayedTuples.pop_front();
				}
				
				while( !tmpTuples.empty() )
				{					
					tUnpack< sizeof...(Args) >::unpack( this, *tmpTuples.front() );
					delete tmpTuples.front();
					tmpTuples.front() = NULL;
					tmpTuples.pop_front();
				}
				
				return (m_delayedTuples.size() > 0) ? ( std::get<0>(m_delayedTuples.front()) ) : 0;
			}
			///< Dispatches out-of-date delayed Signals using the the provided time as time reference.
			/// @param updateTime The current time.
			/// @warning This virtual function is and should only be used by signalManager to call for delayed events.
			
														// ISignal	//[4] 4 bytes
			T_DelayedEvent m_delayedTuples;							//[4] 8 bytes
		#endif
			T_EventHandlerList m_IDelegateList;						//[4] 8 bytes | 0 byte padding
	};//------------------------------------------------------------->[4] 20 bytes with delayed / 8 bytes without delayed
	
	template < class Tr, class... Args >
	std::ostream& operator<<( std::ostream& os, const Signal<Tr(Args...)>& evtD)
	{
		return evtD.toOstream( os );
	}
	///< Returns a formated text output with Signal informations.
	/// @param os The target stream. @param evtD The Slot that must display informations.	
	
} // hbt end

#endif



