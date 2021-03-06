/*
	load_store.hh
	-------------
*/

#ifndef V68K_LOADSTORE_HH
#define V68K_LOADSTORE_HH

// C99
#include <stdint.h>


namespace v68k
{
	
	struct processor_state;
	struct op_params;
	
	
	void load( processor_state& s, op_params& pb );
	
	bool store( processor_state& s, const op_params& pb );
	
}

#endif

