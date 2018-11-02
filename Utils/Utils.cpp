#include "Utils.h"

namespace hbt
{
	std::string exe_path()
	{
		#ifdef _WIN32
			return "";
		#endif
		
		#ifdef __APPLE__
		
			char path[1024];
			CFBundleRef mainBundle = CFBundleGetMainBundle();
			if(!mainBundle)
				return "";

			CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
			if(!mainBundleURL)
				return "";

			CFStringRef cfStringRef = CFURLCopyFileSystemPath(mainBundleURL, kCFURLPOSIXPathStyle);
			if(!cfStringRef)
				return "";

			CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);

			CFRelease(mainBundleURL);
			CFRelease(cfStringRef);

			return std::string(path)+"/Contents/MacOS/";
		#endif
		
		#ifdef __linux
			int len;
			static char path[1024];
			char* slash;
			
			if( std::string(path) == "" )
			{
				// Read symbolic link /proc/self/exe
				
				len = readlink("/proc/self/exe", path, sizeof(path)); 			//(Linux)
				//len = readlink("/proc/curproc/file", path, sizeof(path));		//(FreeBSD)
				//len = readlink("/proc/self/path/a.out", path, sizeof(path));	//(Solaris)
				
				if(len == -1)
					return NULL;

				path[len] = '\0';

				// Get the directory in the path by stripping exe name 
				slash = strrchr(path, '/');
				if(! slash || slash == path)
					return NULL;
				
				*slash = '\0';    // trip slash and exe name
			}
			
			return std::string(path)+"/";
		#endif
	}
	
}
