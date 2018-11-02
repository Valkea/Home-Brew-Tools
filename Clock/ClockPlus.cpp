#include "ClockPlus.h"

namespace hbt
{
//*************************************************************************************************/
///// ClockPlus ///////////////////////////////////////////////////////////////////////////////////

	ClockPlus::ClockPlus() : Clock(), m_duration(0.0f), m_isFinished(false)
	{
		CLOCK_DEBUG( "INIT ClockPlus" );
	}

	ClockPlus::~ClockPlus()
	{
		CLOCK_DEBUG( "CLOSE ClockPlus" );
	}


///// ClockPlus::Public functions /////////////////////////////////////////////////////////////////

	void ClockPlus::update( const F32 _deltaTime )
	{
		if( m_isFinished ) return;
		
		if( m_duration > 0.0f && m_cpuCycles >= m_duration )
		{
			m_cyclesElapsed = 0.0f;
			m_isFinished = true;
			CLOCK_DEBUG( "ClockPlus::update( ENDED )\t-- m_cpuCycles: " << m_cpuCycles << "\t-- time: " << std::fixed << ( (F32)m_cpuCycles / s_cyclesPerSecond ) );
		}
		else
		{
			Clock::update( _deltaTime );
		}
	}

	void ClockPlus::singleStep( const I16 _numSteps )
	{
		if( (m_isPaused || s_isPaused ) && !m_isFinished ) Clock::singleStep( _numSteps );
	}
	
	
///// ClockPlus::Getters & Setters ////////////////////////////////////////////////////////////////

	//-- Duration
	void ClockPlus::setDuration( const F32 duration )
	{
		if( secondsToCycles( (F64)(duration) ) > m_cpuCycles )
		{
			m_isFinished = false;
			m_duration = secondsToCycles( (F64)duration );
			CLOCK_DEBUG( "ClockPlus::setDuration(" << duration << ") " << m_duration );
		}
		else
			CLOCK_DEBUG_W( "ClockPlus::setDuration(" << duration << ") DURATION ALREADY ELAPSED" );
			
	}

	F32 ClockPlus::getDuration() const
	{
		return cyclesToSeconds( (F64) m_duration );
	}

	//-- States
	bool ClockPlus::isFinished() const
	{
		return m_isFinished;
	}
	
} // hbt end

