#include "PrintManager.h"

namespace hbt // [home brew tools]
{

#if COUT_TO_FILE
/// @cond consoleToFile
	void* consoleToFile()
	{
		FILE* fout = std::freopen( std::string(exe_path() + "stdout.txt").c_str(), "w", stdout);
		FILE* ferr = std::freopen( std::string(exe_path() + "stderr.txt").c_str(), "w", stderr);
	}
/// @endcond
#endif

#if AUTO_INIT_PRINTMGR
	PrintManager& printManager = PrintManager::getInstance();
#endif


//*************************************************************************************************/
///// PrintManager ////////////////////////////////////////////////////////////////////////////////

	PrintManager::PrintManager() : m_logToFile(), m_resetLists( !ResetChannelList | ResetCallbackList ), m_logAll(false), m_maxChannel(1), m_verboThreshold(0), m_dispatchVerbosity(0)
	{
		setDispatchVerbosity( V_INFORMATION );
		setVerbosityThreshold( V_ALL );
		setMaxChannel(5);
		std::cout << "INIT PrintManager" << std::endl;
	}

	PrintManager::~PrintManager()
	{
		catchStream(false, std::cout);
		catchStream(false, std::cerr);
		disconnectAll();
		if( m_logToFile.is_open() ) m_logToFile.close();
		std::cout << "CLOSE PrintManager " << std::endl;
	}


///// PrintManager::Public functions //////////////////////////////////////////////////////////////

	void PrintManager::dispatchMessage( std::string msg )
	{	
		if( msg == "" ) return; 
		
		if( m_dispatchVerbosity > m_verboThreshold )
		{
			//LOG_TO_FILE( V_INFORMATION , "::dispatchMessage\t\tVERBOSITY OVERFLOW | verbo: " << (int)m_dispatchVerbosity << "(" << (int)m_verboThreshold << ") | msg: " << msg << std::flush );
			m_resetLists = ResetChannelList;
			return; 
		}
		
		static std::stringstream channels (std::stringstream::in | std::stringstream::out);
		static std::list< IDelegate<std::string>* > 	dcCallbacks;
		
		if( m_dispatchChannelList.size() == 0 )
		{
			LOG_TO_FILE( V_ERROR, "::dispatchMessage\t\tERROR: no channel defined | msg: " << msg << std::flush);
			return;
		}
		
		if( m_resetLists & ResetCallbackList )
		{
			m_resetLists = !ResetCallbackList;
			dcCallbacks.clear();
			channels.str("");
			
			std::set< U8 >::const_iterator itChannels;
			for ( itChannels=m_dispatchChannelList.begin() ; itChannels != m_dispatchChannelList.end(); ++itChannels )
			{	
				U8 channel = (*itChannels);
				channels << (int)channel << " ";
				dcCallbacks.insert( dcCallbacks.end(), m_channelCallbacks[channel].begin(), m_channelCallbacks[channel].end() );
			}
			
			LOG_TO_FILE( V_INFORMATION, "::dispatchMessage\t\tPREPARE | " << dcCallbacks.size() << " callbacks to dispatch" <<  std::endl);
		}
		
		if( !dcCallbacks.empty() )
		{
			std::list< IDelegate<std::string>* >::const_iterator itCallbacks;
			for ( itCallbacks=dcCallbacks.begin() ; itCallbacks != dcCallbacks.end(); ++itCallbacks )
				(**itCallbacks)( msg );
		}	
		
		if( dcCallbacks.empty() )
			LOG_TO_FILE( V_ERROR, "::dispatchMessage\t\tERROR: no registred callback | msg: " << msg << std::flush);
		else
			LOG_TO_FILE( V_INFORMATION, "::dispatchMessage\t\t-OK-  | verbo: " << (int)m_dispatchVerbosity << "(" << (int)m_verboThreshold	<< ") | channel(s): " << std::left << std::setfill(' ') << std::setw(10) << channels.str() << "| msg: " << msg << std::flush);
		
		m_resetLists = ResetChannelList;
	}

	//== Channels management ============================

	//- addDispatchChannel
	void PrintManager::addDispatchChannel( const U8 channel ) 
	{
		if( channel == ALL_CHANNELS )
		{
			addDispatchChannel( std::vector<U8>{ALL_CHANNELS} );
			return;
		}
		
		if( !isValidChannel( channel, __FUNCTION__ ) ) return;
		
		if( m_resetLists & ResetChannelList )
		{
			m_dispatchChannelList.clear();
			m_resetLists = !ResetChannelList | ResetCallbackList;
			
			LOG_TO_FILE( V_INFORMATION, "::addDispatchChannel\tRESET" << std::endl);
		}
		
		m_dispatchChannelList.insert( channel );
		LOG_TO_FILE( V_INFORMATION, "::addDispatchChannel(" << (int)channel << ")" << std::endl);
	}
	
	void PrintManager::addDispatchChannel( std::vector<U8> channels )
	{
		isAllChannels( channels );
	
		for ( std::vector<U8>::const_iterator it = channels.begin() ; it < channels.end(); ++it )
			addDispatchChannel( *it );
	}

	//- disconnectAll
	void PrintManager::disconnectAll( const U8 channel )
	{
	 	if( channel == ALL_CHANNELS )
		{
			disconnectAll();
			return;
		}

		if( !isValidChannel( channel, __FUNCTION__ ) ) return;
		
		std::list<IDelegate<std::string>*>::iterator itDeleg;
		for ( itDeleg = m_channelCallbacks[channel].begin() ; itDeleg != m_channelCallbacks[channel].end(); ++itDeleg )
		{
			if( --(*itDeleg)->count == 0 )
			{
				delete *itDeleg;
				*itDeleg = NULL;
			}
		}
		m_channelCallbacks[channel].clear();
		
		m_resetLists = ResetCallbackList;
		LOG_TO_FILE( V_INFORMATION, "::disconnectAll(" << (int)channel <<") " << std::endl);
	}
	
	void PrintManager::disconnectAll( std::vector<U8> channels )
	{	
		for ( std::vector<U8>::const_iterator it = channels.begin() ; it < channels.end(); ++it )
			disconnectAll( *it );
	}

	void PrintManager::disconnectAll()
	{
		for ( U8 i = 0 ; i < m_channelCallbacks.size() ; ++i )
			if( !m_channelCallbacks[i].empty() ) disconnectAll( i );
			
		LOG_TO_FILE( V_INFORMATION, "::disconnectAll(ALL)" << std::endl);
	}


///// PrintManager::Getters & Setters /////////////////////////////////////////////////////////////

	void PrintManager::setEnableLog( const bool logIt, const U8 verbo )
	{
		m_logAll = logIt;
		m_logVerbo = verbo;
		
		if(m_logAll && !m_logToFile.is_open())	m_logToFile.open(  std::string(exe_path() + "hbtout.log") );
		else if(!m_logAll && m_logToFile.is_open())	m_logToFile.close();
		
		LOG_TO_FILE( V_INFORMATION, "::setEnableLog(" << m_logAll << ", " << (int)verbo << ")\tMax channel: " << (int)m_maxChannel << " | Verbosity threshold: " << (int)m_verboThreshold << std::endl);
	}
	
	void PrintManager::setDispatchVerbosity( const U8 verbo )
	{
		m_dispatchVerbosity = verbo;
		LOG_TO_FILE( V_INFORMATION, "::setDispatchVerbosity(" << (int)verbo << ")" << std::endl);
	}

	//== Channels number ================================
	void PrintManager::setMaxChannel( const U8 max )
	{
		m_maxChannel = max;
		m_channelCallbacks.resize(m_maxChannel + 1);
		LOG_TO_FILE( V_INFORMATION, "::setMaxChannel(" << (int)max << ")" << std::endl);

		std::set< U8 >::const_iterator itChannels;
		for ( itChannels=m_dispatchChannelList.begin() ; itChannels != m_dispatchChannelList.end(); ++itChannels )
			if( (*itChannels) > m_maxChannel )
				m_dispatchChannelList.erase( itChannels );
		
		m_resetLists = ResetCallbackList;
	}

	U8 PrintManager::getMaxChannel() const
	{
		return m_maxChannel;
	}

	//== Verbosity threshold ============================
	void PrintManager::setVerbosityThreshold( const U8 verbo )
	{
		m_verboThreshold = verbo;
		LOG_TO_FILE( V_INFORMATION, "::setVerbosityThreshold(" << (int)verbo << ")" << std::endl);
	}

	U8 PrintManager::getVerbosityThreshold() const
	{
		return m_verboThreshold;
	}


///// PrintManager::Private functions /////////////////////////////////////////////////////////////

	bool PrintManager::isValidChannel( const U8 channel, const std::string function )
	{
		if( channel > m_maxChannel)
		{
			LOG_TO_FILE( V_ERROR, "::" << function << "\tERROR: channel " << (int)channel << " is out of range (Max: " << (int)m_maxChannel << ")" << std::endl);
			assert( channel <= m_maxChannel);
			return false;
		}
		
		if( U8(0-1) < channel )
		{
			LOG_TO_FILE( V_ERROR, "::" << function << "\tERROR: channel " << (int)channel << " is reserved." << std::endl);
			assert( U8(0-1) >= channel );
			return false;
		}
		
		return true;
	}
	
	void PrintManager::isAllChannels( std::vector<U8>& channels )
	{
		if( channels.size() == 1 && channels[0] == ALL_CHANNELS )
		{
			channels.resize( m_channelCallbacks.size() );
			for ( U8 i = 0 ; i < m_channelCallbacks.size() ; ++i )
				channels[i] = i;
		}
	}
	
	void PrintManager::connectICallback( std::vector<U8> channels, IDelegate<std::string>* iDeleg )
	{
		isAllChannels( channels );
		std::list<IDelegate<std::string>*>::const_iterator itDeleg;
		
		for ( std::vector<U8>::const_iterator it = channels.begin() ; it != channels.end(); ++it )
		{
			if( !isValidChannel( *it, __FUNCTION__ ) ) continue;
			
			for ( itDeleg = m_channelCallbacks[*it].begin() ; itDeleg != m_channelCallbacks[*it].end(); ++itDeleg )
				if( **itDeleg == *iDeleg ) break;
					
			if( itDeleg == m_channelCallbacks[*it].end() )
			{
				m_channelCallbacks[*it].push_back( iDeleg );
				++m_channelCallbacks[*it].back()->count;
				LOG_TO_FILE( V_INFORMATION, "::connectICallback(" << (U32)*it << ")\t"<< m_channelCallbacks[*it].size() << " callbacks" << std::endl);
			}
		}
		
		m_resetLists = ResetCallbackList;
	}
	
	void PrintManager::disconnectICallback( std::vector<U8> channels, IDelegate<std::string>* iDeleg )
	{	
		isAllChannels( channels );
		
		for ( std::vector<U8>::const_iterator it = channels.begin() ; it != channels.end(); ++it )
		{	
			if( m_channelCallbacks[*it].size() == 0 || !isValidChannel( *it, __FUNCTION__ ) ) continue;
				
			std::list<IDelegate<std::string>*>::iterator itDeleg;
			for ( itDeleg = m_channelCallbacks[*it].begin() ; itDeleg != m_channelCallbacks[*it].end(); ++itDeleg )
				if( **itDeleg == *iDeleg ) break;
					
			if( itDeleg !=  m_channelCallbacks[*it].end() )
			{
				if( --(*itDeleg)->count == 0 )
				{
					delete *itDeleg;
					*itDeleg = NULL;
				}
				m_channelCallbacks[*it].erase( itDeleg );
				LOG_TO_FILE( V_INFORMATION, "::removeChannelListener(" << (U32)*it  << ")\t"<< m_channelCallbacks[*it].size() << " callbacks" << std::endl);
			}
			else
				LOG_TO_FILE( V_WARNING, "::removeChannelListener(" << (U32)*it  << ")\tERROR: target not found (still "<< m_channelCallbacks[*it].size() << " callbacks)" << std::endl );
		}
		
		m_resetLists = ResetCallbackList;
	}

	std::ofstream& PrintManager::LogToFile( const U8 verbo )
	{
		if( m_logAll )
		{
			m_logToFile.clear( (verbo > m_logVerbo) ? std::ios_base::badbit : std::ios_base::goodbit );
			m_logToFile << std::left << std::setfill(' ') << std::setw(12) << timeStamp("%X");
		}
		return m_logToFile;
	}

	std::string PrintManager::timeStamp( const char* specifier )
	{
		time_t rawtime;
		char buffer [80];
		
		time ( &rawtime );

		strftime (buffer,80, specifier, localtime ( &rawtime ) );
		return std::string( buffer );
	}

	
//*************************************************************************************************/
///// Namespaces //////////////////////////////////////////////////////////////////////////////////

	//* Required to pass multiple channels at once if C++0x can't features can't be activated.
	/*vector<U8> channelList( U8 v0, U8 v1, U8 v2, U8 v3, U8 v4, U8 v5, U8 v6, U8 v7, U8 v8, U8 v9)
	{
		vector<U8> tmpVect;
		tmpVect.push_back(v0);
		if( v1 != -1 ) tmpVect.push_back(v1);
		if( v2 != -1 ) tmpVect.push_back(v2);
		if( v3 != -1 ) tmpVect.push_back(v3);
		if( v4 != -1 ) tmpVect.push_back(v4);
		if( v5 != -1 ) tmpVect.push_back(v5);
		if( v6 != -1 ) tmpVect.push_back(v6);
		if( v7 != -1 ) tmpVect.push_back(v7);
		if( v8 != -1 ) tmpVect.push_back(v8);
		if( v9 != -1 ) tmpVect.push_back(v9);
		
		return tmpVect;
	}*/
}
