/*	=====================
 *	ShareOpenTransport.cc
 *	=====================
 */

#include "Genie/Utilities/ShareOpenTransport.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

#ifndef MAC_OS_X_VERSION_10_8

// ClassicToolbox
#include "ClassicToolbox/OpenTransport.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	static unsigned OpenTransport_refcount = 0;
	
	
	void InitOpenTransport_Shared()
	{
		if ( OpenTransport_refcount == 0 )
		{
			N::InitOpenTransport();
		}
		
		++OpenTransport_refcount;
	}
	
	void CloseOpenTransport_Shared()
	{
		if ( --OpenTransport_refcount == 0 )
		{
			::CloseOpenTransport();
		}
	}
	
}

#endif  // #ifndef MAC_OS_X_VERSION_10_8

