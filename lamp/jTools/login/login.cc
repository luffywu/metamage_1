/*	========
 *	login.cc
 *	========
 */

// Standard C
#include <errno.h>
#include <stdlib.h>

// POSIX
#include <fcntl.h>
#include <unistd.h>

// more-posix
#include "more/perror.hh"


#pragma exceptions off


static void SetVariables()
{
	#if defined(__MC68K__)
	#define ENV_HOSTTYPE "m68k"
	#elif defined( __POWERPC__ )
	#define ENV_HOSTTYPE "powerpc"
	#elif defined( __i386__ )
	#define ENV_HOSTTYPE "i386"
	#endif
	
	#if defined( __MACH__ )
	#define ENV_MAC_RUNTIME "mach-o"
	#elif defined( __POWERPC__ ) || defined( __CFM68K__ )
	#define ENV_MAC_RUNTIME "cfm"
	#else
	#define ENV_MAC_RUNTIME "rsrc"
	#endif
	
	#if TARGET_API_MAC_CARBON
	#define ENV_MAC_BACKEND "carbon"
	#else
	#define ENV_MAC_BACKEND "blue"
	#endif
	
	setenv( "HOSTTYPE", ENV_HOSTTYPE                  , 0 );
	setenv( "OSTYPE",                       "macrelix", 0 );
	setenv( "MACHTYPE", ENV_HOSTTYPE "-apple-macrelix", 0 );
	
	setenv( "MAC_RUNTIME", ENV_MAC_RUNTIME, 0 );
	setenv( "MAC_BACKEND", ENV_MAC_BACKEND, 0 );
	
	const char* path =  "/usr/local/sbin"
	                   ":/usr/local/bin"
	                   ":/usr/sbin"
	                   ":/usr/bin"
	                   ":/sbin"
	                   ":/bin";
	
	setenv( "PATH", path, 0 );
	
	char path_buffer[ 4096 ];
	
	ssize_t size = _readlink( "/sys/mac/user/prefs/MacRelix/Home",
	                          path_buffer,
	                          sizeof path_buffer - 1 );
	
	const char* home;
	
	if ( size < 0 )
	{
		home = "/home/jr";
	}
	else
	{
		path_buffer[ size ] = '\0';
		
		home = path_buffer;
	}
	
	int changed = chdir( home );
	
	if ( changed < 0 )
	{
		more::perror( "login: chdir", home );
	}
	else
	{
		setenv( "HOME", home, 0 );
	}
}

static void DumpMOTD()
{
	int fd = open( "/etc/motd", O_RDONLY );
	
	if ( fd != -1 )
	{
		pump( fd, NULL, STDOUT_FILENO, NULL, 0, 0 );
		
		(void) close( fd );
	}
}

int main( int /*argc*/, char const *const /*argv*/[] )
{
	// Error output may be going to a log somewhere
	dup2( STDOUT_FILENO, STDERR_FILENO );
	
	SetVariables();
	
	DumpMOTD();
	
	char* exec_argv[] = { "-sh", NULL };
	
	execv( "/bin/sh", exec_argv );
	
	_exit( errno == ENOENT ? 127 : 126 );
	
	return 0;  // Not reached
}

