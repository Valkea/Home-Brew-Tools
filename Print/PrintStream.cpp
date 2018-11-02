#include "PrintStream.h"

namespace hbt // [home brew tools]
{

	#if AUTO_INIT_POUT
		PrintStream& pout = PrintStream::getInstance();
	#endif 

//*************************************************************************************************/
///// PrintStringBuffer ///////////////////////////////////////////////////////////////////////////

	PrintStringBuffer::PrintStringBuffer()
	{
		std::cout << "INIT PrintStringBuffer" << std::endl;
	}

	PrintStringBuffer::~PrintStringBuffer()
	{
		std::cout << "CLOSE PrintStringBuffer " << std::endl;
	}

	int PrintStringBuffer::sync()
	{
	 	if( str() != "\n" )	PRINT_MGR_INSTANCE.dispatchMessage( std::move(str()) );
		str(""); //* Reset the stringstream
		return 0;
	}

	
//*************************************************************************************************/
///// PrintStream /////////////////////////////////////////////////////////////////////////////////

	PrintStream::PrintStream() : std::ostream( &m_buffer ), std::ios( &m_buffer )	
	{
		//PRINT_MGR_INSTANCE.setEnableLog(true);
		//PRINT_MGR_INSTANCE.setMaxChannel( 11 );
		std::cout << "INIT PrintStream" << std::endl;
	}

	PrintStream::~PrintStream()
	{
		std::map< std::ostream*, std::streambuf* >::iterator it;
		while ( !m_streamBackup.empty() )
		{
			it = m_streamBackup.begin();
			std::ostream& streamName = *(*it).first;
			streamName.rdbuf((*it).second);
			m_streamBackup.erase( it );
		}
		
		std::cout << "CLOSE PrintStream " << std::endl;
		//delete rdbuf();
	}

	
///// PrintStream::Public Functions ///////////////////////////////////////////////////////////////

	void PrintStream::catchStream( const bool catchIt, std::ostream& streamName )
	{
		if( catchIt )
		{
			m_streamBackup.insert( make_pair( &streamName, streamName.rdbuf() ) );
			std::streambuf *psbuf = (*this).rdbuf();	//* get PrintStream streambuf
			streamName.rdbuf(psbuf);					//* assign streambuf to ostream
		}
		else
		{
			std::map< std::ostream*, std::streambuf* >::const_iterator it;	
			it = m_streamBackup.find( &streamName );
			if( it != m_streamBackup.end() )
			{
				streamName.rdbuf((*it).second);
				m_streamBackup.erase( & streamName );
			}
		}
	}

	
///// PrintStream::Operators //////////////////////////////////////////////////////////////////////


	PrintStream& PrintStream::operator()()
	{
		return PRINT_STR_INSTANCE;
	}
	
	PrintStream& PrintStream::operator()( const U8 verbo )
	{
		hbt::verbosity( verbo );
		return PRINT_STR_INSTANCE;
	}

	PrintStream& PrintStream::operator()( const std::vector<U8> chans )
	{
		hbt::channels( std::ref(chans) );
		return PRINT_STR_INSTANCE;
	}

	PrintStream& PrintStream::operator()( const U8 verbo, const U8 chan )
	{
		hbt::verbochan( verbo, chan );
		return PRINT_STR_INSTANCE;
	}

	PrintStream& PrintStream::operator()( const U8 verbo, const std::vector<U8> chans )
	{
		hbt::verbochan( verbo, chans );
		return PRINT_STR_INSTANCE;
	}
	
//********************************************************************************************************************
//*** Stream Manipulators ********************************************************************************************

// DOXYGEN_SHOULD_SKIP_THIS // @cond ArgFunc

	std::ostream& OneArgFunc( std::ostream& stream, U8 a)
	{
		return(stream);
	}

	std::ostream& TwoArgFunc( std::ostream& stream, U8 a, U8 b)
	{
		return(stream);
	}

// DOXYGEN_SHOULD_SKIP_THIS_END // @endcond

//*************************************************************************************************/
///// Namespaces //////////////////////////////////////////////////////////////////////////////////
	
	//== Manipulators ===================================
	TwoArgManip<U8> channel( const U8 chan )
	{
		PRINT_MGR_INSTANCE.addDispatchChannel( std::ref(chan) );
		return (TwoArgManip<U8>(OneArgFunc, chan));
	}
	
	TwoArgManip<U8> channels( const std::vector<U8> chans )
	{
		PRINT_MGR_INSTANCE.addDispatchChannel( std::ref(chans) );
		return (TwoArgManip<U8>(OneArgFunc, 0));
	}
	
	TwoArgManip<U8> allChannels()
	{
		PRINT_MGR_INSTANCE.addDispatchChannel( {hbt::ALL_CHANNELS} );
		return (TwoArgManip<U8>(OneArgFunc, 0));
	}
	
	TwoArgManip<U8> verbosity( const U8 verbo )
	{
		PRINT_MGR_INSTANCE.setDispatchVerbosity( std::ref(verbo) );
		return (TwoArgManip<U8>(OneArgFunc, verbo));
	}
	
	template <class T>
	TwoArgManip<U8,U8> verbochan( const U8 verbo, const T chan )
	{
		PRINT_MGR_INSTANCE.addDispatchChannel( std::ref(chan) );
		PRINT_MGR_INSTANCE.setDispatchVerbosity( std::ref(verbo) );
		return (TwoArgManip<U8,U8>(TwoArgFunc, verbo, 0));
	}
	
	//TwoArgManip<U8,U8> verbochan( const U8 verbo, const U8 chan );
	//TwoArgManip<U8,U8> verbochan( const U8 verbo, const std::vector<U8> chan );
	
	// Include this function into PrintManager namespace for better organization...
	void PrintManager::catchStream( const bool catchIt, std::ostream& streamName )
	{
		PRINT_STR_INSTANCE.catchStream( catchIt, streamName );
	}
}

