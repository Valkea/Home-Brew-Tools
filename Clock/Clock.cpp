#include "Clock.h"

namespace hbt
{
//*************************************************************************************************/
///// Clock ///////////////////////////////////////////////////////////////////////////////////////

	Clock::Clock() : m_cpuCycles(0.0f), m_cyclesElapsed(0.0f), m_timeScale(TIME_SCALE_PRECISION), m_isPaused(false), m_isUpToDate(true), m_isManaged(false)
	{
		CLOCK_DEBUG( "INIT Clock" << " | " << &(*this));
	}

	Clock::~Clock()
	{
		CLOCK_DEBUG("CLOSE Clock " << " | " << &(*this));
	}
	
	
///// Clock::Public functions /////////////////////////////////////////////////////////////////////

	//ClockEvent gEvt = ClockEvent( ClockEvent::SINGLE_STEP, 0, 0, 0, 0, 0 );
	void Clock::update( const F32 deltaTime )
	{
		std::string stateStr;
		
		if( !m_isPaused && !s_isPaused )
		{
			m_cyclesElapsed = secondsToCycles( deltaTime * getTimeScale() );
			m_cpuCycles += m_cyclesElapsed;
			CLOCK_DEBUG_( "Clock::update(" << deltaTime << ")" );
		}
		else if( !m_isUpToDate )
		{
			m_isUpToDate = true;
			CLOCK_DEBUG_( "Clock::update( SINGLE-STEP )" );
		}
		else
		{	
			m_cyclesElapsed = 0.0f;
			CLOCK_DEBUG_( "Clock::update( PAUSE )" );
		}
		
		CLOCK_DEBUG( "\t-- m_cpuCycles: " << m_cpuCycles << "\t-- time: " << std::fixed << ( (F32)m_cpuCycles / s_cyclesPerSecond ) );
	}

	void Clock::singleStep( const I16 numSteps )
	{
		if( m_isPaused || s_isPaused )
		{
			m_cyclesElapsed = secondsToCycles( numSteps * (10.0f / 30.0f) * getTimeScale() );
			m_cpuCycles += m_cyclesElapsed;
			m_isUpToDate = false;
			CLOCK_DEBUG( "Clock::singleStep() m_cpuCycles: " << m_cpuCycles << "\t-- m_cyclesElapsed: " << m_cyclesElapsed << '\n' );
		}
	}

	
///// Clock::Getters & Setters ////////////////////////////////////////////////////////////////////

	//-- Cycles
	F64 Clock::getCycles() const
	{
		return m_cpuCycles;
	}
	
	F32 Clock::getCyclesSinceLastUpdate() const
	{
		return m_cyclesElapsed;
	}

	//-- Time (in seconds)
	F32 Clock::getTime() const
	{
		return cyclesToSeconds( m_cpuCycles );
	}

	F32 Clock::getTimeSinceLastUpdate() const
	{
		return cyclesToSeconds( (F64) m_cyclesElapsed );
	}

	//-- Time Scale
	void Clock::setTimeScale( const F32 timeScale )
	{
		m_timeScale = (I32)(timeScale * TIME_SCALE_PRECISION);
		CLOCK_DEBUG( "Clock::setTimeScale(" << m_timeScale << ")" );
	}

	F32 Clock::getTimeScale() const
	{
		if( m_isManaged )
			return ((F32)m_timeScale * (F32)s_timeScale) / (TIME_SCALE_PRECISION_SQUARE);
		else
			return ((F32)m_timeScale * (F32)TIME_SCALE_PRECISION) / (TIME_SCALE_PRECISION_SQUARE);
	}

	//-- States
	void Clock::setPaused( const bool isPaused )
	{
		m_isPaused = isPaused;
		CLOCK_DEBUG( "Clock::setPaused(" << m_isPaused << ")" );
	}
	
	bool Clock::isPaused() const
	{
		return m_isPaused;
	}

	
///// Clock::Static methods ///////////////////////////////////////////////////////////////////////

	bool Clock::s_isPaused = false;
	I32  Clock::s_timeScale = 1.0f * TIME_SCALE_PRECISION;
	U32  Clock::s_cyclesPerSecond = readHiResTimerFrequency();
	
	//-- Hi-Resolsution Timer
	U32 Clock::readHiResTimer() 			
	{
		return clock();
	}

	U32 Clock::readHiResTimerFrequency()
	{
		return CLOCKS_PER_SEC;
	}
	
} // hbt end

