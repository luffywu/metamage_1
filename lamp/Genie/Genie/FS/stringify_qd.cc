/*
	stringify_qd.cc
	---------------
*/

#include "Genie/FS/stringify_qd.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"


namespace Genie
{
	
	void stringify_Point::apply( plus::var_string& out, const Point& pt )
	{
		out += gear::inscribe_decimal( pt.h );
		
		out += ',';
		
		out += gear::inscribe_decimal( pt.v );
	}
	
	void stringify_Rect::apply( plus::var_string& out, const Rect& rect )
	{
		const Point* points = (const Point*) &rect;  // reinterpret_cast
		
		stringify_Point::apply( out, points[0] );  // top left
		
		out += ':';
		
		stringify_Point::apply( out, points[1] );  // bottom right
	}
	
}

