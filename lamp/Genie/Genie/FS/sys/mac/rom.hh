/*
	Genie/FS/sys/mac/rom.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_MAC_ROM_HH
#define GENIE_FS_SYS_MAC_ROM_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_mac_rom( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif
