/*	=============
 *	MemoryFile.hh
 *	=============
 */

#ifndef GENIE_IO_MEMORYFILE_HH
#define GENIE_IO_MEMORYFILE_HH

// vfs
#include "vfs/filehandle_ptr.hh"
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	vfs::filehandle_ptr open_buffer_file( const vfs::node*  file,
	                                      int               flags,
	                                      char*             addr,
	                                      std::size_t       size );
	
}

#endif

