/*
	GetMachineName.cc
	-----------------
	
	Copyright 2010, Joshua Juran
*/

#include "Genie/Utilities/GetMachineName.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifndef __GESTALT__
#include <Gestalt.h>
#endif


namespace Genie
{
	
	const unsigned char* GetMachineName()
	{
		SInt32 mnam;
		
		ConstStr255Param& result = *(ConstStr255Param*) &mnam;
		
		if ( OSErr err = ::Gestalt( gestaltUserVisibleMachineName, &mnam ) )
		{
			result = NULL;
		}
		
		return result;
	}
	
}

