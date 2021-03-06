/*	===============
 *	debug/assert.cc
 *	===============
 */

#include "debug/assert.hh"

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif


namespace debug
{
	
	void handle_failed_assertion( const char*  text,
	                              const char*  func,
	                              const char*  file,
	                              unsigned     line )
	{
		::DebugStr( "\p" "Assertion failure" );
		
		::ExitToShell();
	}
	
}

