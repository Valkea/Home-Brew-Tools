#include "PrintToFile.h"

namespace hbt // [home brew tools]
{
//*************************************************************************************************/
///// PrintToFile /////////////////////////////////////////////////////////////////////////////////

	PrintToFile::PrintToFile( const bool showTime ) : std::ofstream(), m_showTime(showTime)
	{
		std::cout << "INIT PrintToFile (to open)" << std::endl;
	}
	
	PrintToFile::PrintToFile( const char* fileName, const bool showTime ) : std::ofstream(hbt::exe_path()+fileName), m_showTime(showTime)
	{
		std::cout << "INIT PrintToFile" << std::endl;
		(*this) << std::left << std::setfill('.') << std::setw(165) << timeStamp("%A %c") <<std::endl;
	}

	PrintToFile::PrintToFile( const std::string fileName, const bool showTime ) : std::ofstream(hbt::exe_path()+fileName.c_str()), m_showTime(showTime)
	{
		std::cout << "INIT PrintToFile" << std::endl;
		(*this) << std::left << std::setfill('.') << std::setw(165) << timeStamp("%A %c") <<std::endl;
	}

	PrintToFile::~PrintToFile()
	{
		#if ENABLE_AUTO_DISCONNECT_PM
			#if AUTO_INIT_PRINTMGR
				printManager.disconnect( {hbt::ALL_CHANNELS}, *this );
			#else
				PrintManager::getInstance().disconnect( {hbt::ALL_CHANNELS}, *this );
			#endif
		#endif
		std::cout << "CLOSE PrintToFile " << std::endl;
		close();
	}


///// PrintToFile::Public functions ///////////////////////////////////////////////////////////////

	void PrintToFile::operator()( const std::string msg )
	{
		if( m_showTime ) (*this) << timeStamp("%X") << " ";				//* Display timestamp
		(*this) << msg;													//* Write the message		
		msg[msg.length()-1] == '\n' ? flush() : (*this) << std::endl;	//* Avoid double newline or no newline
	}

///// PrintToFile::Getters & Setters //////////////////////////////////////////////////////////////

	void PrintToFile::setTimeStampEnabled( const bool showTime )
	{
		m_showTime = showTime;
	}

	bool PrintToFile::isTimeStampEnabled() const
	{
		return m_showTime;
	}

/////  PrintManager::Private functions ////////////////////////////////////////////////////////////

	std::string PrintToFile::timeStamp( const char* specifier )
	{
		time_t rawtime;
		char buffer [80];
		
		time ( &rawtime );

		strftime (buffer,80, specifier, localtime ( &rawtime ) );
		return std::string( buffer );
	}
}
