/*	===========
 *	realpath.cc
 *	===========
 */

// Standard C
#include <errno.h>

// Standard C++
#include <algorithm>

// POSIX
#include <sys/stat.h>

// Relix
#include "relix/_realpathat.h"

// plus
#include "plus/mac_utf8.hh"

// GetPathname
#include "GetPathname.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/resolve_links_in_place.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/ResolvePathAt.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	static plus::string mac_pathname_from_file( const FSTreePtr& file )
	{
		return GetMacPathname( GetFSSpecFromFSTree( file ) );
	}
	
	
	static ssize_t _realpathat( int dirfd, const char *path, char *buffer, size_t buffer_size, int flags )
	{
		try
		{
			FSTreePtr file = ResolvePathAt( dirfd, path );
			
			vfs::resolve_links_in_place( file );
			
			const bool is_mac = flags & REALPATH_OUTPUT_HFS;
			
			plus::string resolved = is_mac ? mac_pathname_from_file( file )
			                               : pathname( *file );
			
			if ( (flags & REALPATH_OUTPUT_HFS_UTF8) == REALPATH_OUTPUT_HFS_UTF8 )
			{
				resolved = plus::utf8_from_mac( resolved );
			}
			
			const bool too_big = resolved.size() > buffer_size;
			
			const size_t bytes_copied = std::min( buffer_size, resolved.size() );
			
			std::memcpy( buffer, resolved.data(), bytes_copied );
			
			if ( too_big )
			{
				errno = ERANGE;
				
				// Return the bitwise inverse of the data size.
				return ~resolved.size();
			}
			
			return bytes_copied;
		}
		catch ( ... )
		{
			return set_errno_from_exception();
		}
	}
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( _realpathat );
	
	#pragma force_active reset
	
}

