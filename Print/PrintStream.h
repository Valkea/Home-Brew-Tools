/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 05.01.2011
	Description : 	This should provide an easy way to send messages to the PrintManager using
					something like :
					pout << verbosity(2) << channel(10) << channel(12) << "bla" << endl;
					pout( 2, {10,12} ) << "bla" << endl;
				
///// Class list //////////////////////////////////////////////////////////////////////////////////
	- PrintStringBuffer		[4] 40 bytes	(0+40)
	- PrintStream			[4] 204 bytes	(140+68)
	
///// IMPORTANT ///////////////////////////////////////////////////////////////////////////////////

	- Currently the maximum channel number is U8 = 255

	- Passing initializer_list require C++0x --->
	  set(CMAKE_CXX_FLAGS "-std=gnu++0x") / set(CMAKE_CXX_FLAGS "-std=gcc++0x")
	  (if C++0x can't be used, we need te replace {1,2,3} by hbt::printManager.channelList(1,2,3)
	  and reactivate it by deleting comments symbols)
	  
	- The last channel or verbosity setting are used as default value for the incoming pout calls
	  (take care of nested functions)
	  
	- The channel array is reset at the first channel setting modification after a endl or flush
	  call. This way if you do not specify any channel after a flush the channel list remain the
	  same, but if you specify some new channels the list won't keep old ones.
	  
	- You can use almost all the regular stream manipulators.
	
	- Verbosity Flags are : V_CRITICAL, V_ERROR, V_WARNING, V_INFORMATION, V_ALL

///// Example /////////////////////////////////////////////////////////////////////////////////////

	----> See examples at the bottom of the file...	

///// ETA /////////////////////////////////////////////////////////////////////////////////////////
	** ToDo
		- Add a way to reserve a full channel by asking for a free channel and returning it. (This
		  way we could register by name in addition to channel number)
		- Ajouter des "try catch" sur les fonctions d'envoie pour signaler les eventuels problemes ?
		- Ajouter de quoi switcher entre un mode qui utilise la dernière verbosity utilisée ou un
		  mode qui utilise une verbosity par defaut si rien n'est précisé. setMode( "global" /
		  "specific", verbo default = -1, chans default = -1)

	** Bugs		- none ATM
	
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_PrintStream
#define HEADER_PrintStream

#include "PrintManager.h"	
#include "../PortableTypes.h" 	// define F16, F32, U16, U32 etc...

#include <iostream>				// cout
#include <sstream>				// stringbuf
#include <ostream>
#include <map>
#include <functional>			// ref

namespace hbt /// [home brew tools]
{

	/** This enables hbt::pout to auto-initialize as a reference to the PrintStream singleton.
		Thus a global alias of the PrintStream singleton exists for convenience and for optimization*, but if a manual initialization is required/preferred, this define should be turned to false, and the PrintStream singleton manually initialized.\n Defined in PrintStream.h
		@verbatim *It calls the static method once and redirects all incoming hbt::pout calls directly to the PrintStream singleton instead of asking for the singleton reference each time PrintStream::getInstance() is called. @endverbatim
		@ingroup pout */
		#define AUTO_INIT_POUT true

		#if AUTO_INIT_POUT
			#define PRINT_STR_INSTANCE pout
		#else
			#define PRINT_STR_INSTANCE PrintStream::getInstance()
		#endif
		
		#if AUTO_INIT_PRINTMGR
			#define PRINT_MGR_INSTANCE printManager
		#else
			#define PRINT_MGR_INSTANCE PrintManager::getInstance()
		#endif
		
	/** This enables the POUT_DEBUG() calls to use some hbt::pout calls in debug mode only.
		This option offers the ability to add an hbt::pout witch can be easily removed at release. @code POUT_DEBUG( verbo, channel, msg ); @endcode can be used instead of @code hbt::pout( verbo, channel ) << msg; @endcode
		By disabling this option, the POUT_DEBUG calls will be removed at compile time, thus avoiding useless function calls at runtime without having to remove by hands the %hbt::pout calls everywhere.
		@ingroup pout*/
		#define POUT_TO_DEBUG true

		#if POUT_TO_DEBUG
			#if AUTO_INIT_POUT
				#define POUT_DEBUG( verbo, channel, msg ) hbt::pout(verbo, channel) << msg
			#else
				#define POUT_DEBUG( verbo, channel, msg ) PrintStream::getInstance()(verbo, channel) << msg
			#endif
		#else
			#define POUT_DEBUG( verbo, channel, msg )
		#endif
		
//*************************************************************************************************/
///// PrintStringBuffer ///////////////////////////////////////////////////////////////////////////

// DOXYGEN_SHOULD_SKIP_THIS // @cond PrintStringBuffer

	class PrintStringBuffer : public std::stringbuf
	{
		public:
			PrintStringBuffer();
			~PrintStringBuffer();
			
			int sync();	///< override stringbuf::sync() to enable the dispatch function
			
											// std::stringbuf	//[4] 40 bytes | 0 byte padding
	};//--------------------------------------------------------->[4] 40 bytes
	
// DOXYGEN_SHOULD_SKIP_THIS_END // @endcond

//*************************************************************************************************/
///// PrintStream /////////////////////////////////////////////////////////////////////////////////

	/** This class provides an easy way to send messages to the PrintManager.
	  *	@ingroup pout
	  *	@brief This can be done using something like :\n
	  *	@code hbt::pout << hbt::verbosity(2) << hbt::channel(10) << hbt::channel(12) << "bla" << std::endl; @endcode or
	  *	@code hbt::pout( 2, {10,12} ) << "bla" << std::endl; @endcode or
	  *	@code PrintStream psInstance; psInstance( 2, {10,12} ) << "bla" << std::endl; @endcode \n
	  *	This way, if the verbosity threshold is higher or equal to 2, each registered function on channels 10 and/or 12 will receive the message. */
	
	class PrintStream : public std::ostream
	{
		public:
		
			static PrintStream& getInstance()
			{
				static PrintStream singleton;
				return singleton;
			}
			///< Initializes and/or returns the PrintStream singleton instance.
			/// @return The PrintStream singleton instance.
			
			~PrintStream();
			///< A destructor.
			
		//== Public functions ===============================
			
			void catchStream( const bool, std::ostream& );
			///< Redirects any ostream (cout, cerr...) to hbt::%pout
			/// @param catchIt enabled/disabled the catching. @param streamName The stream to catch.
			
		//== Operators override =============================
			
			PrintStream &operator()();
			///< Enables hbt::%pout
			/// @return The PrintStream singleton instance.
			
			PrintStream &operator()( const U8 verbo );
			///< Enables hbt::%pout( verbo )
			/// @param verbo The verbosity of the incoming message. @return The PrintStream singleton instance.
			
			PrintStream &operator()( const std::vector<U8> channels );
			///< Enables hbt::%pout( {channels} )
			/// @param channels The channels of the incoming message. @return The PrintStream singleton instance.
			
			PrintStream &operator()( const U8 verbo, const U8 channel );
			///< Enables hbt::%pout( verbo, channel )
			/// @param verbo The verbosity of the incoming message. @param channel The channel of the incoming message. @return The PrintStream singleton instance.
			
			PrintStream &operator()( const U8 verbo, const std::vector<U8> channels );
			///< Enables hbt::%pout( verbo, {channels} )
			/// @param verbo The verbosity of the incoming message. @param channels The channels of the incoming message. @return The PrintStream singleton instance.

		private:
			PrintStream();
			///< A constructor.
													// std::ostream		//[4] 140 bytes
			PrintStringBuffer m_buffer;									//[4] 40 bytes
			std::map< std::ostream*,std::streambuf* > m_streamBackup;	//[4] 24 bytes | 0 byte padding
	};//----------------------------------------------------------------->[4] 204 bytes

	
//*************************************************************************************************/
///// Stream Manipulators /////////////////////////////////////////////////////////////////////////

// DOXYGEN_SHOULD_SKIP_THIS // @cond TwoArgManip

	template<typename A1, typename A2 = void*> class TwoArgManip
	{
		public:
			TwoArgManip (std::ostream& (*pFun) (std::ostream&, A1), A1 val1) : manipFun1_(pFun), val_1(val1), val_2(NULL){}
			TwoArgManip (std::ostream& (*pFun) (std::ostream&, A1, A2), A1 val1, A2 val2) : manipFun2_(pFun), val_1(val1), val_2(val2){}
			void operator()(std::ostream& os) const
			{
				if( !val_2 )
					manipFun1_(os, val_1);
				else
					manipFun2_(os, val_1,val_2);
			}
			
		private:
			A1 val_1;
			A2 val_2;
			std::ostream& (*manipFun1_) (std::ostream&, A1);
			std::ostream& (*manipFun2_) (std::ostream&, A1, A2);
			
	};
	
	template<typename A1, typename A2> std::ostream& operator<< (std::ostream& stream, const TwoArgManip<A1,A2>& manipulator)
	{
		manipulator(stream);
		return(stream);
	}

	template<typename A1, typename A2> std::ostream& operator<< (std::ostream& (*stream)(), const TwoArgManip<A1,A2>& manipulator)
	{
		manipulator(stream);
		return(stream);
	}

	std::ostream& OneArgFunc(std::ostream& stream, U8 a);
	std::ostream& TwoArgFunc(std::ostream& stream, U8 a, U8 b);
	
// DOXYGEN_SHOULD_SKIP_THIS_END // @endcond

//*************************************************************************************************/
///// Namespaces //////////////////////////////////////////////////////////////////////////////////

	#if AUTO_INIT_POUT
		//== PrintStream ====================================
		extern PrintStream& pout;
		///< A reference to the PrintStream singleton instance.
		/// This enables a quick and easy access to the PrintStream functions using hbt::%pout << "msg" ...
		/// @ingroup pout
	#endif

	//== Manipulators ===================================
	TwoArgManip<U8> 	channel( const U8 chan );
	///< Enables the hbt::%channel( channel ) manipulator.
	/// @param chan The channel of the incoming message.
	
	TwoArgManip<U8> 	channels( const std::vector<U8> chans );
	///< Enables the hbt::%channels( {channels} ) manipulator.
	/// @param chans The channels of the incoming message.
	
	TwoArgManip<U8> 	allChannels();
	///< Enables the hbt::%allChannels() manipulator.
	/// @param chans The channels of the incoming message.
	
	TwoArgManip<U8> 	verbosity( const U8 verbo );
	///< Enables the hbt::%verbosity( verbo ) manipulator.
	/// @param verbo The verbosity of the incoming message.
	
	template <class T>
	TwoArgManip<U8,U8> 	verbochan( const U8 verbo, const T chans );
	///< Enables the hbt::%verbochan( verbo, channel ) manipulator.
	/// @param verbo The verbosity of the incoming message. @param chans The channel of the incoming message.

//*************************************************************************************************/
///// Examples ////////////////////////////////////////////////////////////////////////////////////

/*! @class PrintStream
@code

	==> This is an example of how to use the hbt::pout stream. <==
	
	@include "PrintStream.h"
	
	//== PrintManager
		hbt::printManager.setVerbosityThreshold( hbt::V_CRITICAL );
		hbt::printManager.connect( {14,6,3}, MyFunctor );
		{ "registers all listeners etc... see PrintManager examples." }

	//== PrintStream / hbt::pout (later in the code or in another file...)

	//-- Using stream manipulators channel(), channels(), verbosity() and verbochan() --/

	// Sends text to the channel 0 using default verbosity setting ( V_INFORMATION )
		hbt::pout << hbt::channel(0) << "Hello World !" << std::endl;
		
	// Sends text to the channels 0 and 3 using verbosity setting ( V_WARNING )	
		hbt::pout << hbt::channel(0) << hbt::channel(3) << hbt::verbosity( hbt::V_WARNING ) << "Warning for channels 0 and 3 !" << std::endl;
		
	// Same with arrays of channels (If C++0x initializer_list is not enabled, you should use hbt::printManager.channelList(0,3) instead of {0,3}
		hbt::pout << hbt::channels( {0,3} ) << hbt::verbosity( hbt::V_WARNING ) << "Warning for channels 0 and 3 !" << std::endl; 

	// Sends text to the channels 0 using verbosity setting ( V_ERROR ) using only one manipulator
		hbt::pout << hbt::verbochan( hbt::V_ERROR, 0 ) << "Warning for channels 0 and 3 !" << std::endl;
		
	// Sends text to the channels 0 and 3 using manipulators outside the stream
		hbt::verbosity( hbt::V_INFORMATION );
		hbt::channels( {6,2} );
		hbt::pout << "Information for channels 2 and 6 !" << std::endl;
		
	//-- Using overridden hbt::pout --/

	// pout( vector<U8> channels ) 					-- Sends text to the channel 0 using default verbosity setting ( V_INFORMATION )
		hbt::pout({0}) << "Hello World !" << std::endl;
		
	// pout( U8 verbosity ) 						-- Sends text to the last channel(s) using verbosity setting ( V_CRITICAL )	
		hbt::pout( hbt::V_CRITICAL ) << "Critical message for channel 0 !" << std::endl;
		
	// pout( U8 verbosity, U8 channel ) 			-- Sends text to the channel 3 using verbosity setting ( V_INFORMATION )	
		hbt::pout( hbt::V_INFORMATION, 3) << "Information for channels 3 !" << std::endl;
		
	// pout( U8 verbosity, vector<U8> channels ) 	-- Sends text to the channels 1, 4 AND 7 using verbosity setting ( V_WARNING )	
		hbt::pout( hbt::V_WARNING, {1,4,7}) << "Warning for channels 1, 4 and 7 !" << std::endl;

	// pout() 										-- Sends text using the last channels (1, 4 and 7) and the last verbosity setting ( V_WARNING )	
		hbt::pout << "Warning for channels 1, 4 and 7 !" << std::endl;
		
	// pout() 										-- Same using flush instead of endl to avoid auto \n insert;
		hbt::pout << "Warning for channels 1, 4 and 7 !" << std::flush;
		
@endcode */

}

#endif