#include "Signal.h"

namespace hbt
{

	#if __GNUC__ // Based on __PRETTY_FUNCTION__
		std::string getSignalSignature( std::string tPF, std::string openStr )
		{
			size_t index1 = tPF.find( openStr+"<" );
			size_t index2 = tPF.find( ">", index1 );
			std::string Signature = tPF.substr (index1, index2-index1+1);
			
			std::string searchStr = "Tr = ";
			index1 = tPF.find( searchStr );
			index2 = tPF.find( ",", index1 );
			Signature.replace( Signature.find( "Tr" ),2, tPF.substr(index1+searchStr.size(), index2-index1-searchStr.size()) );
			
			searchStr = "Args = {";
			index1 = tPF.find( searchStr );
			index2 = tPF.find( "},", index1 );			
			Signature.replace( Signature.find( "Args ..." ),8, tPF.substr(index1+searchStr.size(), tPF.find( "},", index1 )-index1-searchStr.size()) );

			return Signature;
		}
	#endif

//*************************************************************************************************/
///// SignalManager ///////////////////////////////////////////////////////////////////////////////

#if ENABLE_DELAYED_SIGNALS

	SignalManager::SignalManager() : m_isEmpty(true), timeBucket(0) {}
	
	SignalManager::~SignalManager() { m_IEventDispatcherList.clear(); }
	
	void SignalManager::checkNextDelayedSignal()
	{
		while( (m_IEventDispatcherList.size() > 0) && ( m_IEventDispatcherList.front().second <= timeBucket) )
		{
			#if ENABLE_FORCE_SYNC
				timeLost = timeBucket - m_IEventDispatcherList.front().second;
			#endif
			
			m_isFiring = true;
			U32 newTime = m_IEventDispatcherList.front().first->fireDelayed( timeBucket );
			m_isFiring = false;
			
			if( newTime > 0 )
			{
				m_IEventDispatcherList.front().second = newTime;
				m_IEventDispatcherList.sort( sortByTime() );
			}
			else
				m_IEventDispatcherList.pop_front();
			
			if( m_IEventDispatcherList.size() == 0 )
			{
				timeBucket = 0;
				m_isEmpty = true;
			}
			
			#if ENABLE_FORCE_SYNC
				timeLost = 0;
			#endif
		}
	}
	
	void SignalManager::update( const F32 seconds )
	{ EVENT_DEBUG("SignalManager::update (" << seconds*1000 << ") \t| timeBucket (" << (timeBucket+(seconds*1000)) << ")\t| next_call_time (" << m_IEventDispatcherList.front().second << ")");
	
		if( m_isEmpty ) return;
		timeBucket+= seconds*1000;
		checkNextDelayedSignal();
	}
	
	#define CLOCK_SCALE (CLOCKS_PER_SEC/1000)
	void SignalManager::updateC( const F32 cycles )
	{ EVENT_DEBUG("SignalManager::updateC(" << cycles << ") \t| timeBucket (" << (timeBucket+(cycles/CLOCK_SCALE)) << ")\t| next_call_time (" << m_IEventDispatcherList.front().second << ")");
	
		if( m_isEmpty ) return;
		timeBucket += cycles/CLOCK_SCALE;
		checkNextDelayedSignal();
	}
	
	void SignalManager::registerDispatcher( ISignal* sig, const U32 nextUpdateTime )
	{ EVENT_DEBUG("SignalManager::registerDispatcher : " << nextUpdateTime);

		std::list< T_DelayedEvent >::iterator it;
		for ( it = m_IEventDispatcherList.begin(); it != m_IEventDispatcherList.end(); ++it)
			if( it->first == sig ) break;
		
		if( it == m_IEventDispatcherList.end() )
		{
			m_IEventDispatcherList.push_back( std::make_pair(sig, nextUpdateTime) );
			m_isEmpty = false;
		}
		else
			it->second = nextUpdateTime;
		
		if( !m_isFiring ) m_IEventDispatcherList.sort( sortByTime() );
	}

	void SignalManager::unregisterDispatcher( const ISignal* sig )
	{ EVENT_DEBUG("SignalManager::unregisterDispatcher");
		
		std::list< T_DelayedEvent >::iterator it;
		for ( it = m_IEventDispatcherList.begin(); it != m_IEventDispatcherList.end(); ++it)
			if( it->first == sig ) break;
		
		if(it != m_IEventDispatcherList.end()) m_IEventDispatcherList.erase(it);
	}
	
	SignalManager& signalManager = SignalManager::getInstance();
	
#endif

} // hbt end

