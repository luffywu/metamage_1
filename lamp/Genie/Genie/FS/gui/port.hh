/*
	Genie/FS/gui/port.hh
	--------------------
*/

#ifndef GENIE_FS_GUI_PORT_HH
#define GENIE_FS_GUI_PORT_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr new_port();
	
	void remove_port( const FSTree* port );
	
	
	FSTreePtr New_sys_port( const FSTreePtr&     parent,
	                        const plus::string&  name,
	                        const void*          args );
	
}

#endif
