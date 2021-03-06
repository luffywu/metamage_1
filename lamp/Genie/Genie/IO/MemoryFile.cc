/*	=============
 *	MemoryFile.cc
 *	=============
 */

#include "Genie/IO/MemoryFile.hh"

// Standard C++
#include <algorithm>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/memory_mapping.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"

// Genie
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	class MemoryFileHandle : public RegularFileHandle
	{
		private:
			char*        itsBase;  // base address
			std::size_t  itsSize;
		
		public:
			MemoryFileHandle( const FSTreePtr&  file,
			                  int               flags,
			                  char*             base,
			                  std::size_t       size );
			
			~MemoryFileHandle();
			
			IOPtr Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return itsSize; }
			
			memory_mapping_ptr Map( size_t length, int prot, int flags, off_t offset );
	};
	
	
	static ssize_t buffer_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< MemoryFileHandle& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t buffer_geteof( vfs::filehandle* file )
	{
		return static_cast< MemoryFileHandle& >( *file ).GetEOF();
	}
	
	static ssize_t buffer_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return static_cast< MemoryFileHandle& >( *file ).Positioned_Write( buffer, n, offset );
	}
	
	static const vfs::bstore_method_set buffer_bstore_methods =
	{
		&buffer_pread,
		&buffer_geteof,
		&buffer_pwrite,
	};
	
	static const vfs::filehandle_method_set buffer_methods =
	{
		&buffer_bstore_methods,
	};
	
	
	MemoryFileHandle::MemoryFileHandle( const FSTreePtr&  file,
	                                    int               flags,
	                                    char*             base,
	                                    std::size_t       size )
	:
		RegularFileHandle( file, flags, &buffer_methods ),
		itsBase( base ),
		itsSize( size )
	{
	}
	
	MemoryFileHandle::~MemoryFileHandle()
	{
	}
	
	IOPtr MemoryFileHandle::Clone()
	{
		return new MemoryFileHandle( GetFile(),
		                             get_flags(),
		                             itsBase,
		                             itsSize );
	}
	
	ssize_t MemoryFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		if ( offset >= itsSize )
		{
			return 0;
		}
		
		n_bytes = std::min< size_t >( n_bytes, itsSize - offset );
		
		memcpy( buffer, itsBase + offset, n_bytes );
		
		return n_bytes;
	}
	
	ssize_t MemoryFileHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		if ( n_bytes == 0 )
		{
			return 0;
		}
		
		if ( offset >= itsSize )
		{
			p7::throw_errno( ENOSPC );
		}
		
		n_bytes = std::min< size_t >( n_bytes, itsSize - offset );
		
		memcpy( itsBase + offset, buffer, n_bytes );
		
		return n_bytes;
	}
	
	memory_mapping_ptr
	//
	MemoryFileHandle::Map( size_t length, int prot, int flags, off_t offset )
	{
		if ( offset + length > itsSize )
		{
			p7::throw_errno( ENXIO );
		}
		
		return new vfs::memory_mapping( itsBase + offset, length, flags );
	}
	
	vfs::filehandle_ptr open_buffer_file( const vfs::node*  file,
	                                      int               flags,
	                                      char*             addr,
	                                      std::size_t       size )
	{
		return new MemoryFileHandle( file,
		                             flags,
		                             addr,
		                             size );
	}
	
}

