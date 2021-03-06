// open.cc
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "poseven/functions/open.hh"


namespace poseven
{
	
	nucleus::owned< fd_t > open( const char* name, open_flags_t oflag, mode_t mode )
	{
		// register errnos
		
		int fd = throw_posix_result( ::open( name, oflag, mode ) );
		
		return nucleus::owned< fd_t >::seize( fd_t( fd ) );
	}
	
	
	namespace Testing
	{
		
		static void Foo()
		{
			fd_t foo = open( "/etc/motd", o_rdonly );
			
			//Read( foo );
			
			int fd = foo;
			
			// This is a compile error with enums!  YES!!
			//Write( fd );
		}
		
	}
	
}

