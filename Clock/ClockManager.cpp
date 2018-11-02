#include "ClockManager.h"

namespace hbt
{
	#if AUTO_INIT_CLOCKMGR
		ClockManager& clockManager = ClockManager::getInstance();
	#endif
	
//*************************************************************************************************/
///// Clock Manager  //////////////////////////////////////////////////////////////////////////////
	
	ClockManager::ClockManager() : m_autoEnable(false), m_resetCycles(false)//, m_timeScale(1000), m_isPaused(false) <--- délocalisé en static dans Clock pour pouvoir les utiliser même sans ClockManager
	{
		std::cout << "INIT ClockManager" << std::endl;
	}

	ClockManager::~ClockManager()
	{
		std::cout << "CLOSE ClockManager" << std::endl;
	}


///// ClockManager::Public functions //////////////////////////////////////////////////////////////

	//-- Clocks management
	void ClockManager::addClock( hbt::Clock& clock )
	{
		m_clockList.insert( &clock );
		clock.m_isManaged = true;
		
		CLOCK_DEBUG( "ClockManager::addClock()\t" << m_clockList.size() << " clock(s) to dispatch" );
	}

	void ClockManager::removeClock( hbt::Clock& clock )
	{
		CLOCK_DEBUG_( "ClockManager::removeClock()\t" );
		
		std::set< hbt::Clock* >::const_iterator it;
		it = m_clockList.find( &clock );
		
		if( it != m_clockList.end() )
		{
			m_clockList.erase( it );
			clock.m_isManaged = false;
		}
		else
			CLOCK_DEBUG_W( "ERROR: target not found.\t" );
		
		CLOCK_DEBUG( "Still " << m_clockList.size() << " clock(s) to dispatch" );
	}

	//-- Clocks dispatch
	void ClockManager::update( const F32 deltaTime )
	{ CLOCK_DEBUG( "ClockManager::updateDispatch( " << deltaTime << " )" );
		
		m_autoEnable ? updateFromInside() : updateFromOutside( deltaTime );
	}

	void ClockManager::singleStep( const I16 numSteps )
	{ CLOCK_DEBUG( "ClockManager::singleStep( " << numSteps << " )" );
		
		for( std::set< hbt::Clock* >::const_iterator it = m_clockList.begin(); it != m_clockList.end(); ++it )
			(*it)->singleStep( numSteps );
	}

	//-- Internal timer management
	void ClockManager::start( const bool startAtFirstCall )
	{ CLOCK_DEBUG( "ClockManager::start()" );
		
		m_autoEnable = true;
		m_resetCycles = startAtFirstCall;
	}

	void ClockManager::stop()
	{ CLOCK_DEBUG( "ClockManager::stop()" );
		
		m_autoEnable = false;
	}


///// ClockManager::Getters & Setters /////////////////////////////////////////////////////////////

	void ClockManager::setClocksTimeScale( const F32 timeScale )
	{ CLOCK_DEBUG( "ClockManager::setClocksTimeScale( " << timeScale << " )" );
		
		for( std::set< hbt::Clock* >::const_iterator it = m_clockList.begin(); it != m_clockList.end(); ++it )
			(*it)->setTimeScale( timeScale );
	}

	//-- Pause
	void ClockManager::setPaused( const bool isPaused )
	{
		hbt::Clock::s_isPaused = isPaused;
	}

	bool ClockManager::isPaused() const
	{
		return hbt::Clock::s_isPaused;
	}

	//-- Time scale
	void ClockManager::setTimeScale( const F32 timeScale )
	{
		hbt::Clock::s_timeScale = timeScale * TIME_SCALE_PRECISION;
	}

	F32 ClockManager::getTimeScale() const
	{
		return hbt::Clock::s_timeScale / TIME_SCALE_PRECISION;
	}


///// ClockManager::Private methods ///////////////////////////////////////////////////////////////

	void ClockManager::updateFromInside()
	{ CLOCK_DEBUG( "ClockManager::update( AUTO )" );
		static F32 hiResTimerOld = 0;
		
		if( m_resetCycles )
		{
			hiResTimerOld = hbt::Clock::readHiResTimer();
			m_resetCycles = false;
		}
		
		F32 hiResTimer = hbt::Clock::readHiResTimer();
		updateFromOutside( hbt::Clock::cyclesToSeconds((F64) (hiResTimer - hiResTimerOld)) );
		hiResTimerOld = hiResTimer;
	}

	void ClockManager::updateFromOutside( const F32 deltaTime )
	{ CLOCK_DEBUG( "ClockManager::update( " << deltaTime << " )" );
		
		for( std::set< hbt::Clock* >::const_iterator it = m_clockList.begin(); it != m_clockList.end(); ++it )
			(*it)->update( deltaTime );
	}

}
