/*	=====================
 *	SystemCallRegistry.cc
 *	=====================
 */

#include "Genie/SystemCallRegistry.hh"

// Standard C++
#include <algorithm>
#include <vector>

// Genie
#include "Genie/Dispatch/UnimplementedSystemCall.hh"


namespace Genie
{
	
	typedef std::vector< SystemCall > SystemCallRegistry;
	
	SystemCall* gSystemCallArray = NULL;
	
	syscall_number_t gLastSystemCall = 0;
	
	
	SystemCall::SystemCall() : function( (void*) UnimplementedSystemCall ), name()
	{
		
	}
	
	static bool operator<( const SystemCall& a, const SystemCall& b )
	{
		return std::strcmp( a.name, b.name ) < 0;
	}
	
	static bool operator==( const SystemCall& systemCall, const char* name )
	{
		return systemCall.name  &&  std::strcmp( systemCall.name, name ) == 0;
	}
	
	static SystemCallRegistry& TheSystemCallRegistry()
	{
		static SystemCallRegistry theSystemCallRegistry( 1 );
		
		return theSystemCallRegistry;
	}
	
	void RegisterSystemCall( syscall_number_t index, const char* name, void* func )
	{
		SystemCallRegistry& registry = TheSystemCallRegistry();
		
		if ( index >= gLastSystemCall )
		{
			gLastSystemCall = index + 1;
			
			const unsigned required_size = gLastSystemCall + 1;
			
			registry.resize( required_size );
			
			gSystemCallArray = &registry[0];
		}
		
		registry[ index ] = SystemCall( func, name );
	}
	
	const SystemCall* GetSystemCall( syscall_number_t index )
	{
		const syscall_number_t required_size = index + 1;
		
		SystemCallRegistry& registry = TheSystemCallRegistry();
		
		if ( registry.size() < required_size )
		{
			return NULL;
		}
		
		return &registry[ index ];
	}
	
	const SystemCall* LookUpSystemCallByName( const char* name )
	{
		static SystemCallRegistry::iterator begin = TheSystemCallRegistry().begin();
		static SystemCallRegistry::iterator end   = TheSystemCallRegistry().end  ();
		
		SystemCallRegistry::const_iterator it = std::find( begin, end, name );
		
		if ( it == end )
		{
			return NULL;
		}
		
		return &*it;
	}
	
}
