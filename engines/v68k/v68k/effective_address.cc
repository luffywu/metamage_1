/*
	effective_address.cc
	--------------------
*/

#include "v68k/effective_address.hh"

// v68k
#include "v68k/fetch.hh"
#include "v68k/state.hh"


#pragma exceptions off


namespace v68k
{
	
	static int32_t read_extended_displacement( processor_state& s, uint16_t size_code )
	{
		switch ( size_code )
		{
			case 1:
				return 0;
			
			case 2:
				return fetch_instruction_word_signed( s );
			
			case 3:
				return fetch_longword( s );
		}
		
		// Assume null displacement on zero size code
		return 0;
	}
	
	static uint32_t read_ea_displaced_address( processor_state& s, uint32_t address )
	{
		return address + fetch_instruction_word_signed( s );
	}
	
	static uint32_t read_ea_indexed_address( processor_state& s, uint32_t address )
	{
		const uint16_t extension = fetch_instruction_word( s );
		
		int32_t base_displacement = int8_t( extension & 0xff );
		
		const uint16_t _n = extension >> 12;
		
		// _n == 0- 7:  D0-D7
		// _n == 8-15:  A0-A7
		
		int32_t index = s.regs.d[_n];  // Xn
		
		const bool long_sized = extension >> 11 & 0x1;
		
		if ( !long_sized )
		{
			index = int16_t( index );
		}
		
		if ( s.model >= mc68020 )
		{
			const uint16_t scale_bits = extension >> 9 & 0x3;
			
			index <<= scale_bits;
		}
		
		const bool full_format = extension & 0x0100;
		
		if ( s.model < mc68020  ||  !full_format )
		{
			return address + index + base_displacement;
		}
		
		const bool base_suppress  = extension & 0x80;
		
		const uint16_t bd_size = extension >> 4 & 0x3;
		
		base_displacement = read_extended_displacement( s, bd_size ) & -!base_suppress;
		
		const bool index_suppress = extension & 0x40;
		
		if ( index_suppress )
		{
			index = 0;
		}
		
		const int iis = extension & 0x7;
		
		const bool postindexed = iis & 0x4;
		
		const bool memory_indirect = iis != 0;
		
		address += base_displacement + !postindexed * index;
		
		if ( memory_indirect )
		{
			/*
				Don't check s.badly_aligned_data(), since Memory Indirect mode
				only occurs on 68020 or later models, which don't take exceptions
				for misaligned data accesses.
			*/
			
			if ( !s.mem.get_long( address, address, s.data_space() ) )
			{
				return s.bus_error();
			}
			
			const int32_t outer_displacement = read_extended_displacement( s, iis & 0x3 );
			
			address += outer_displacement;
		}
		
		if ( postindexed )
		{
			address += index;
		}
		
		return address;
	}
	
	static inline uint32_t postincrement( uint32_t& An, int size )
	{
		const uint32_t result = An;
		
		An += size;
		
		return result;
	}
	
	static inline uint32_t predecrement( uint32_t& An, int size )
	{
		return An -= size;
	}
	
	uint32_t fetch_effective_address( processor_state& s, uint16_t mode, uint16_t n, int size )
	{
		const uint32_t An = s.regs.a[n];
		
		if ( mode == 3  ||  mode == 4 )
		{
		//	ASSERT( size != 0 );
			
			if ( size == 1  &&  n == 7 )
			{
				size = 2;  // SP is kept word-aligned
			}
		}
		
		switch ( mode )
		{
			case 0:  // Data
			case 1:  // Address
				// Return a register id for Data/Address Register Direct mode.
				
				return mode << 3 | n;
			
			case 2:
				return An;
			
			case 3:
				return postincrement( s.regs.a[n], size );
			
			case 4:
				return predecrement( s.regs.a[n], size );
			
			case 5:
				return read_ea_displaced_address( s, An );
			
			case 6:
				return read_ea_indexed_address( s, An );
			
			case 7:
				switch ( n )
				{
					case 0:
						return fetch_instruction_word_signed( s );
					
					case 1:
						return fetch_longword( s );
					
					case 2:
						return read_ea_displaced_address( s, s.regs.pc );
					
					case 3:
						return read_ea_indexed_address( s, s.regs.pc );
					
					default:
						break;
				}
				
				break;
		}
		
		// Not reached
		return 0;
	}
	
}

