/*
Auteur : Letremble Emmanuel
Date : 15.07.2011
Description : This file define some useful miscanelenous functions
*/

#ifndef HEADER_HBT_UTILS
#define HEADER_HBT_UTILS


	#ifdef __APPLE__
		#include <CoreFoundation/CoreFoundation.h>
	#endif
	
	#include <string.h>
	#include <iostream>
	#include <string>
	#include <sstream>
	
	namespace hbt
	{
		std::string exe_path();

	    template<class T>
        std::string toString(const T& t)
	    {
	    	std::ostringstream stream;
		    stream << t;
		    return stream.str();
	    }

	    template<class T>
	    T fromString(const std::string& s)
	    {
	    	std::istringstream stream (s);
	    	T t;
	    	stream >> t;
	    	return t;
	    }
	}
	
#endif

