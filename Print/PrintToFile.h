/*/////////////////////////////////////////////////////////////////////////////////////////////////

	Auteur : Letremble Emmanuel
	Date : 14.01.2011
	Description : This class should provide an easy way to write strings into log files.
	
///// Class list //////////////////////////////////////////////////////////////////////////////////
	- PrintToFile		[4] 252 bytes (248+4)

***************************************************************************************************/
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef HEADER_PrintToFile
#define HEADER_PrintToFile

#include "../PortableTypes.h" 	// define F16, F32, U16, U32 etc...
#include "../Utils/Utils.h"		// exe_path

#include <iostream>	// cout
#include <fstream>	// ofstream
#include <iomanip>	// setw
#include <time.h>

	/** This enables the PrintToFiles to unregister themselves from the PrintManager.
		If this is enabled, the PrintToFile will unregister from the PrintManager when the destructor is called.
		This way, it's not needed anymore to inform the connected PrintManager about the PrintToFile destruction.\n Defined in PrintToFile.h
		@ingroup pout */
		#define ENABLE_AUTO_DISCONNECT_PM true
		
		#if ENABLE_AUTO_DISCONNECT_PM
			#include "PrintManager.h"
		#endif
		
namespace hbt /// [home brew tools]
{
//*************************************************************************************************/
///// PrintToFile /////////////////////////////////////////////////////////////////////////////////

	/** This class writes strings into log files.
		@ingroup pout */

	class PrintToFile : public std::ofstream
	{
		public:
			PrintToFile( const bool showTime = false );
			///< A constructor.
			/// @param showTime Defines if the time stamp is enabled or disabled. 
			/// @warning calling the open method is required with this constructor. 
			
			PrintToFile( const char* fileName, const bool showTime = false );
			///< A constructor. @overload
			/// @param fileName The name of the file to create/open. @param showTime Defines if the time stamp is enabled or disabled. 
			
			PrintToFile( const std::string fileName, const bool showTime = false );
			///< A constructor. @overload
			/// @param fileName The name of the file to create/open. @param showTime Defines if the time stamp is enabled or disabled. 
			
			~PrintToFile();
			///< A destructor.

		//== Public functions ===============================
			
			void operator()( const std::string msg );
			///< Appends a message to the text file opened by this PrintToFile instance.
			/// @param msg The message to append.
			
		//== Getters & Setters ==============================
			
			void setTimeStampEnabled( const bool showTime );
			///< Enables/Disables the time stamp.
			/// @param showTime Defines if the time stamp is enabled or disabled.
			
			bool isTimeStampEnabled() const;
			///< Returns the state of the time stamp flag.
			/// @return The current time stamp flag state.
			
		private:
			std::string timeStamp( const char* );
			
											// std::ofstream	//[4] 248 bytes
			bool m_showTime;									//[1] 1 byte | 3 bytes padding
	};//--------------------------------------------------------->[4] 252 bytes
}

#endif