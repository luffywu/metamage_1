/*	========
 *	Spawn.cc
 *	========
 */

// BSD
#ifndef __MACH__
#include "vfork.h"
#endif

// Genie
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	static int SpawnVFork()
	{
		Process& parent = CurrentProcess();
		
		parent.Status( kProcessForking );
		
		Process* child = new Process( parent.ProcessID() );
		
		RegisterProcessContext( child );
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( SpawnVFork );
	
}

