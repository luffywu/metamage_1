/*	============
 *	MainEntry.hh
 *	============
 */

#ifndef GENIE_MAINENTRY_HH
#define GENIE_MAINENTRY_HH

// Standard C++
#include <string>

// Boost
#include <boost/shared_ptr.hpp>

// Iota
#include "iota/argv.hh"
#include "iota/environ.hh"


struct FSSpec;


namespace Genie
{
	
	typedef int (*Main0)();
	typedef int (*Main2)( int argc, iota::argv_t argv );
	typedef int (*Main3)( int argc, iota::argv_t argv, iota::environ_t envp );
	
	class MainEntryPoint
	{
		private:
			Main3             itsMain;
			iota::environ_t*  itsEnviron;
		
		public:
			MainEntryPoint( Main3             main,
			                iota::environ_t*  env = NULL) : itsMain   ( main ),
			                                                itsEnviron( env  )
			{
			}
			
			virtual ~MainEntryPoint();
			
			Main3 GetMainPtr() const  { return itsMain; }
			
			iota::environ_t* GetEnvironPtr() const  { return itsEnviron; }
	};
	
	typedef boost::shared_ptr< MainEntryPoint > MainEntry;
	
	MainEntry GetMainEntryFromAddress( Main3 address );
	
	inline MainEntry GetMainEntryFromAddress( Main2 address )
	{
		return GetMainEntryFromAddress( reinterpret_cast< Main3 >( address ) );
	}
	
	inline MainEntry GetMainEntryFromAddress( Main0 address )
	{
		return GetMainEntryFromAddress( reinterpret_cast< Main3 >( address ) );
	}
	
	MainEntry GetMainEntryFromFile( const FSSpec& file );
	
}

#endif

