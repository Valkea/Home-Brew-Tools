/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 05.01.2011
	Description : This class should provide a stream manager to redirect stream inputs using
				  channels and verbosity to the registered callback functions.
				  
///// Class list //////////////////////////////////////////////////////////////////////////////////
	- PrintManager		[4] 292 bytes
	
///// ETA /////////////////////////////////////////////////////////////////////////////////////////
	** ToDo PrintManager	- Add define instead of variable m_logAll and LogToFile() ? This should
							  minimize the impact of hbt::pout when in Release mode.
	
	** Bugs					- It seems to produce an "internal compiler error : Segmentation fault"
							  when compiling with GCC using flag -O3
***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __PRINT_MANAGER__
#define __PRINT_MANAGER__

#include <iostream>				// cout
#include <fstream>				// ofstream
#include <sstream>				// stringstream
#include <iomanip>				// setw
#include <vector>
#include <set>
#include <list>	
#include <assert.h>

#include "../Utils/Utils.h"		// exe_path
#include "../PortableTypes.h" 	// define F16, F32, U16, U32 etc...
#include "../Delegate.h" 		// delegates

namespace hbt /// [home brew tools]
{

	//== Defines & Macros ===============================
		
	/** This enables the redirection of the std::cout and std::cerr streams to stdout.txt and stderr.txt
		@ingroup pout*/
		#define COUT_TO_FILE true
		
		#if COUT_TO_FILE
			void* consoleToFile();
		#endif
		
	/** This enables the PrintManager log messages to be compiled.
		This option offers the ability to easily disable the PrintManager internal log system at release.
		By disabling it, the LOG_TO_FILE calls will be removed at compile time, thus avoiding useless function calls at runtime without having to remove by hands the LogToFile() calls everywhere.
		@verbatim This SHOULD be disabled at release @endverbatim
		@warning LOG_TO_FILE() can't be used outside of PrintManager itself as it refers to the private LogToFile() method.		
		@see PrintManger.setEnableLog to enable/disable the PrintManager log at runtime.
		@ingroup pout*/
		#define ENABLE_PRINTMGR_LOG true
		
		#if ENABLE_PRINTMGR_LOG
			#define LOG_TO_FILE( verbo, msg ) LogToFile( verbo ) << msg
		#else
			#define LOG_TO_FILE( verbo, msg )
		#endif
		
	/** This enables hbt::printManager to auto-initialize as a reference to the PrintManager singleton.
		Thus a global alias of the PrintManager singleton exists for convenience and for optimization*, but if a manual initialization is required/preferred, this define should be turned to false, and the PrintManager singleton manually initialized.\n Defined in PrintManager.h
		@verbatim *It calls the static method once and redirects all incoming hbt::printManager calls directly to the PrintManager singleton instead of asking for the singleton reference each time PrintManager::getInstance() is called. @endverbatim
		@ingroup pout */
		#define AUTO_INIT_PRINTMGR true
		
	/** These are the default levels of verbosity.
		But we can use up to 256 different levels.
		@ingroup pout */
		enum Verbosity 
		{
			V_CRITICAL,		///< 0: Critical
			V_ERROR,		///< 1: Critical, Error
			V_WARNING,		///< 2: Critical, Error, Warning
			V_INFORMATION,	///< 3: Critical, Error, Warning, Information
			V_ALL			///< 4: All
		};
		
	/** These are the reserved channels.
		There is up to 256 different channels, but some are reserved for special purpose.
		@ingroup pout */
		enum ReservedChannels 
		{
			ALL_CHANNELS = 255		///< 255: All channels
		};
	
//*************************************************************************************************/
///// PrintManager ////////////////////////////////////////////////////////////////////////////////

	/** This singleton class provides a stream manager to dispatch stream inputs.
		@brief This can be done using channels and verbosity to redirect inputs to the registered callback functions.
		@tparam T The type of the function pointer used as callback.
		@ingroup pout */
	
	class PrintManager
	{		
		public:
			static PrintManager& getInstance()			
			{
				#if COUT_TO_FILE
					static void* cToFileS = consoleToFile();
				#endif				
				static PrintManager singleton;
				return singleton;
			}
			///< Initializes and/or return the %PrintManager singleton instance.
			// @return The %PrintManager singleton instance.
			
			~PrintManager();
			///< A destructor.
			
		//== Public functions ===============================
			
			void dispatchMessage( std::string msg );
			///< Dispatches the message to all registered functions matching to the current channel list and verbosity setting.
			/// @param msg The message to dispatch.
			
			void addDispatchChannel( const U8 channel );
			///< Add a channel to the next dispatch.
			/// @param channel The channel to add to the channel list.
			
			void addDispatchChannel( std::vector<U8> channels );
			///< Add some channels to the next dispatch.
			/// @param channels The channels to add to the channel list. hbt::ALL_CHANNELS will listen all the current channels.

			template < class Tr >
			void connect( const std::vector<U8> channels, Tr(*func)(std::string) )
			{ 
				connectICallback( std::move(channels), new FunctionDelegate<Tr,std::string>( func ) );
			}
			///< Registers a function listener over one or multiple channels.
			/// @param channels The channels to listen for. hbt::ALL_CHANNELS will listen all the current channels. @param func The function to call when a message is received on the associated channel(s).
			/// @tparam Tr The return type of the assigned function, functor or method.
		
			template < class Tr, class C >
			void connect( const std::vector<U8> channels, C& target, Tr(C::*method)(std::string) )
			{ 
				connectICallback( std::move(channels), new MethodDelegate<C,Tr,std::string>( target, method ) );
			}
			///< Registers a method listener over one or multiple channels.
			/// @param channels The channels to listen for. hbt::ALL_CHANNELS will listen all the current channels. @param target The class member instance used to call the method. @param method The class member method to call when a message is received on the associated channel(s).
			/// @tparam Tr The return type of the assigned function, functor or method. @tparam C The type of the class instance used to call the registered method.
			
			template < class C >
			void connect( const std::vector<U8> channels, C& functor )
			{ 
				connectICallback( std::move(channels), new FunctorDelegate<C, std::string>( functor ) );
			}
			///< Registers a functor listener over multiple channels.
			/// @param channels The channels to listen for. hbt::ALL_CHANNELS will listen all the current channels. @param functor The functor to call when a message is received on the associated channel(s).
			/// @tparam C The type of the functor to call the registered method.
			
			template < class Tr >
			void disconnect( const std::vector<U8> channels, Tr(*func)(std::string) )
			{
				FunctionDelegate<Tr,std::string> tmp( func );
				disconnectICallback( std::move(channels), &tmp );
			}
			///< Unregisters a function listener over one or multiple channels.
			/// @param channels The channels to disconnect from. hbt::ALL_CHANNELS will disconnect all the current channels. @param func The function to disconnect.
			/// @tparam Tr The return type of the assigned function, functor or method.
		
			template < class Tr, class C >
			void disconnect( const std::vector<U8> channels, C& target, Tr(C::*method)(std::string) )
			{
				MethodDelegate<C,Tr,std::string> tmp( target, method );
				disconnectICallback( std::move(channels), &tmp );
			}
			///< Unregisters a method listener over one or multiple channels.
			/// @param channels The channels to disconnect from. hbt::ALL_CHANNELS will disconnect all the current channels. @param target The class member instance used to call the method. @param method The class member method to disconnect.
			/// @tparam Tr The return type of the assigned function, functor or method. @tparam C The type of the class instance used to call the registered method.
			
			template < class C >
			void disconnect( const std::vector<U8> channels, C& functor )
			{
				FunctorDelegate<C, std::string> tmp( functor );
				disconnectICallback( std::move(channels), &tmp );
			}
			///< Unregisters a functor listener over one or multiple channels.
			/// @param channels The channels to disconnect from. hbt::ALL_CHANNELS will disconnect all the current channels. @param functor The functor to disconnect.
			/// @tparam C The type of the functor to call the registered method.
			
			void disconnectAll( const U8 channel );
			///< Unregisters all listeners from one channel.
			/// @param channel The channel to remove from the channel list.			
			void disconnectAll( const std::vector<U8> channels );
			///< Unregisters all listeners from multiple channels.
			/// @param channels The channels to remove from the channel list.			
			void disconnectAll();
			///< Unregisters all listeners from all channels.
			
			void catchStream( const bool, std::ostream& );
			///< Redirects any ostream (cout, cerr...) to pout.
			/// @warning (defined later in PrintStream.h)
			/// @param catchIt enabled/disabled the catching. @param streamName The stream to catch.
			
		//== Getters & Setters ==============================
			
			void setEnableLog( const bool, const U8 = V_INFORMATION );
			///< Enables/Disables the PrintManger log.
			/// @param logIt enabled/disabled the PrintManager log. @param verbo The PrintManager log verbosity threshold.
			/// @see ENABLE_PRINTMGR_LOG to enable/disable the %PrintManager log at compile time.
			
			void setDispatchVerbosity( const U8 verbo );
			///< Set the verbosity of incoming dispatch messages.
			/// @param verbo The verbosity level. @see hbt::Verbosity
			
			void setMaxChannel( const U8 max );
			///< Sets the maximum channel usable by the hbt::pout stream.
			/// @param max The number of channels enabled.
			U8   getMaxChannel() const;
			///< Returns the maximum channel usable by the hbt::pout stream.
			/// @return The number of channels enabled.
			
			void setVerbosityThreshold( const U8 verbo );
			///< Sets the current verbosity threshold needed to dispatch messages.
			/// @param verbo The verbosity threshold level. @see hbt::Verbosity
			U8   getVerbosityThreshold() const;
			///< Returns the current verbosity threshold needed to dispatch messages.
			/// @return The verbosity threshold level. @see hbt::Verbosity
			
		private:
			PrintManager();	
			bool isValidChannel( const U8 channel, const std::string function );
			///< Verifies if the channel is inferior or equal to the getMaxChannel() value.
			/// @param channel The channel to verify. @param function The function name for log/debug purpose. @return The channel validity.
			
			void isAllChannels( std::vector<U8>& channels );
			///< Verifies if the channels value request for all channels at once.
			/// Is the vector is empty, this function populate the vector with all the registered channels.
			/// @param channels The channels vector reference to verify and to eventually populate.
			
			void connectICallback( std::vector<U8> channels, IDelegate<std::string>* iDeleg );
			///< Registers a IDelegate listener over one or multiple channels.
			/// @param channels The channels to listen for. @param iDeleg The IDelegate to call when a message is received for the associated channel(s).
			
			void disconnectICallback( std::vector<U8> channels, IDelegate<std::string>* iDeleg );
			///< Unregisters an IDelegate listener over one or multiple channels.
			/// @param channels The channels to listen for. @param iDeleg The IDelegate to disconnect.
			
			std::ofstream& 	LogToFile( const U8 = V_ALL );
			///< Prints << message to the hbtout.log @see hbt::Verbosity
			std::string 	timeStamp( const char* );
			///< Returns the current formated timestamp 

			enum ResetOptions { ResetChannelList = 0x01, ResetCallbackList = 0x02 };		//* m_resetLists Bit Flags
			
			std::vector< std::list< IDelegate<std::string>* >> m_channelCallbacks; 			//[4] 12 bytes
			std::set< U8 > m_dispatchChannelList;											//[4] 24 bytes
			
			U8 m_maxChannel; 																//[1] 1 byte
			U8 m_verboThreshold;															//[1] 1 byte
			U8 m_dispatchVerbosity;															//[1] 1 byte
			U8 m_resetLists;																//[1] 1 byte | 0 byte padding	* 2 bool bit flags inside *

			std::ofstream 	m_logToFile;													//[4] 248 bytes
			U8 				m_logVerbo;														//[1] 1 byte
			bool 			m_logAll;														//[1] 1 byte | 2 bytes padding
	};//------------------------------------------------------------------------------------->[4] 292 bytes

	
//*************************************************************************************************/
///// Namespaces //////////////////////////////////////////////////////////////////////////////////

#if AUTO_INIT_PRINTMGR
	extern PrintManager& printManager;
	///< A reference to the PrintManager singleton instance.
	/// This enables a quick and easy access to the PrintManager functions using hbt::printManager.function()
	/// @ingroup pout
#endif	

	// vector<U8> channelList( U8, U8, U8, U8, U8, U8, U8, U8, U8, U8 );

//*************************************************************************************************/
///// Examples ////////////////////////////////////////////////////////////////////////////////////

/*! @class PrintManager
@code

	==> This is an example of how to use the hbt::printManager or any reference to the PrintManager singleton <==

	@include "PrintStream.h"
	
	//-- Defines lambda callback function --/

	void MyFunc( const std::string txt )
	{
		// do your stuff...
	}

	//-- Defines lambda object instance owning a Callback function --/

	class MyClass
	{	
		public:
			MyClass();
			~MyClass();
			
			//-- Callback method
			void append( const std::string string ) { //.. do your stuff }
			
			//-- Callback functor
			void operator()( const std::string string ) { //.. do your stuff }
	};

	MyClass myFile01;
	
	//== PrintManager

	//-- connect --/

	// Registers the function to the channel 9 of the dispatch list
		hbt::printManager.connect( {9}, &MyFunc );

	// Registers the function to the channels 3,6 and 14 of the dispatch list
		hbt::printManager.connect( {14,6,3}, &MyFunc );

	// Registers the "append" method with the associated instance to the channel 11 of the dispatch list
		hbt::printManager.connect( {11}, myFile01, &MyClass::append );

	// Registers the "append" method with the associated instance to the channels 1,2 and 3 of the dispatch list
		hbt::printManager.connect( {1,2,3}, myFile01, &MyClass::append );

	// Registers the functor method with the associated instance to the channels 12 and 13 of the dispatch list
		hbt::printManager.connect( {12,13}, myFile01 );

		
	//-- disconnect --/

	// Remove the function from the dispatch list of the channel 9
		hbt::printManager.disconnect( {9}, &MyFunc );

	// Remove the function from the dispatch list of the channels 3 and 6
		hbt::printManager.disconnect( {6,3}, &MyFunc );

	// Remove the "append" method with the associated instance from the dispatch list of the channel 11
		hbt::printManager.disconnect( {11}, myFile01, &MyClass::append );

	// Remove the "append" method with the associated instance from the dispatch list of the channels 1 and 3
		hbt::printManager.disconnect( {1,3}, myFile01, &MyClass::append );
		
	// Remove the functor method with the associated instance from the dispatch list of the channel 12
		hbt::printManager.disconnect( {12}, myFile01 );

		
	//-- disconnectAll --/

	// Remove all the listerners registered to the channel 9
		hbt::printManager.disconnectAll( 9 );
		
	// Remove all the listerners registered to the channels 1, 2 and 3
		hbt::printManager.disconnectAll( {1,2,3} );

	// Remove all the listeners registered over all channels
		hbt::printManager.disconnectAll();

		
	//-- PrintManager others functions

	// Enables the PrintStream Log with verbosity = V_INFORMATION
		hbt::printManager.setEnableLog( true, hbt::V_INFORMATION );
		
	// Disable the PrintStream Log
		hbt::printManager.setEnableLog( false );
		
	// Sets the verbosity threshold to V_CRITICAL in order to dispatch only critical msg (all channels all functions)
		hbt::printManager.setVerbosityThreshold( hbt::V_CRITICAL );
		
	// Sets the channel number at 7
		hbt::printManager.setMaxChannel( 7 );
		
	// Enables cout redirection to pout
		hbt::printManager.catchStream(true);

	// Enables cerr redirection to pout
		hbt::printManager.catchStream(true, cerr);
		

	//== PrintStream / hbt::pout (defined later in the code or in another file...)
		hbt::pout( hbt::V_CRITICAL, {1,4,7}) << "Critical Hello World on channels 1, 4 and 7 !" << std::endl;
		{ "etc... see PrintStream examples." }

@endcode */

}

#endif
