/*
Auteur : Letremble Emmanuel
Date : 05.08.2010
Description : This file define the portable sized types to use.
			  This is required to easily change the types to match the size needed over each operating system.
			  Any type size and alignement can be verified using the functions sizeof( type ) and alignof( type )
			  std::cout << "The size of TYPE is:" << sizeof( TYPE ) << " and its alignment is: " << alignof( TYPE ) << std::endl;
*/

#ifndef HEADER_PORTABLE_SIZED_TYPES
#define HEADER_PORTABLE_SIZED_TYPES

//*************************************************************************************************/
///// Portable Types //////////////////////////////////////////////////////////////////////////////

	// Floating types (verified on Vista 32)
	typedef long double 		F128;	//[4] 12 bytes	// +/- 1.7e +/- 308 (~15 digits)
	typedef double 				F64;	//[8] 8 bytes	// +/- 1.7e +/- 308 (~15 digits)
	typedef float 				F32;	//[4] 4 bytes	// +/- 3.4e +/- 38 (~7 digits)

	// Signed Integer types (verified on Vista 32)
	typedef long long 			I64;	//[8] 8 bytes	// +/- 1.7e +/- 308 (~15 digits)	*to verify
	typedef long 				I32;	//[4] 4 bytes	// -2.147.483.648 to 2.147.483.647	*similar to: int on Vista 32
	typedef short 				I16;	//[2] 2 bytes	// -32.768 to 32.767
	typedef char 				I8;		//[1] 1 byte	// -128 to 127

	// Unsigned Integer types (verified on Vista 32)
	typedef unsigned long long 	U64;	//[8] 8 bytes	// +/- 1.7e +/- 308 (~15 digits)	*to verify
	typedef unsigned long 		U32;	//[4] 4 bytes	// 0 to 4.294.967.295				*similar to: unsigned int on Vista 32
	typedef unsigned short 		U16;	//[2] 2 bytes	// 0 to 65535
	typedef unsigned char 		U8;		//[1] 1 bytes	// 0 to 255
	typedef unsigned char 		BYTE;	//[1] 1 byte	// 0 to 255

	// A & reference is [4] 4 bytes (8 bytes on 64bit architectures)
	// A * pointer is [4] 4 bytes (8 bytes on 64bit architectures)

//*************************************************************************************************/
///// Doxygen /////////////////////////////////////////////////////////////////////////////////////

	//  Doxygen main page
	/// @mainpage About hbt::
	/// @section intro_sec Introduction
	/// The Home Brew Tools suite is made to provide some small but useful crossplatform libraries.
	/// 
	/// @section install_sec Installation
	/// Step 1 copy "hbt" directory into the project directory\n
	/// Step 2 that's all...

	/** @defgroup pout Print-Streams
		This group aggregates all classes related to the pout library.
		Print-Streams are kinds of std::ostream (as cout and cerr) with verbosity and channels.
		*/
		
	/** @defgroup clock Clocks
		This group aggregates all classes related to the Clock library.
		Clocks are objects made to manipulate time. Thus all other classes which are using those clock informations, can be easily modified too.*/	
		
	/** @defgroup signalandslot Signals & Slots
		This group aggregates all classes related to the Signal & Slot library.
		The signals and slots are systems made to dispatch on demand informations to registered callbacks.*/
		
		/** @defgroup event Events
			This group aggregates all classes related to the Event library.
			Events and their derived classes, are kinds of containers made to transmit specific informations about a specific event.\n
			They have nothing to do with event dispatching (see @link signalandslot Signals & Slots@endlink instead), they are packages of informations.
			@ingroup signalandslot */
			
		/** @defgroup signalmanipulator Manipulators
			This group aggregates all classes related to the Signal manipulators library.
			Those signal manipulators are very small classes made to be connected between signal and slots in order to manipulate transmited values.
			@ingroup signalandslot */
		
	/** @defgroup delegate Delegates
		This group aggregates all classes related to the Delegate library.
		The delegates are objects that can store functions, functors and methods pointers under a unique type.
		The following groups uses Delegates: @link pout Print-Streams@endlink and @link signalandslot Signals & Slots@endlink
		*/

	/** @defgroup hash Hashes
		This group aggregates all classes related to the Hash library. */
		
	/** @defgroup fileloader File-Loader
		This group aggregates all classes related to the file loading library. */
	
#endif

