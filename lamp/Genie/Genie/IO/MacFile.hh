/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// nucleus
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	IOPtr
	//
	New_DataForkHandle( const nucleus::shared< Mac::FSFileRefNum >&  refNum,
	                    int                                          flags );
	
	IOPtr
	//
	New_RsrcForkHandle( const nucleus::shared< Mac::FSFileRefNum >&  refNum,
	                    int                                          flags );
	
}

#endif

