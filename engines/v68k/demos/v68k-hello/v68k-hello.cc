/*
	v68k-hello.cc
	-------------
*/

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <unistd.h>

// v68k
#include "v68k/endian.hh"
#include "v68k/emulator.hh"


#pragma exceptions off


using v68k::big_word;
using v68k::big_longword;


const uint32_t initial_SSP  = 4096;
const uint32_t initial_USP  = 3072;
const uint32_t code_address = 2048;
const uint32_t os_address   = 1024;

static const uint16_t os[] =
{
	// Jump over handlers
	
	0x6016,  // BRA.S  *+24
	
	// Illegal Instruction,
	// Privilege Violation
	
	0x484F,  // BKPT  #7
	
	// Trap 15
	
	0x4E72,  // STOP #FFFF  ; finish
	0xFFFF,
	
	// Trap 0
	
	0x41EF,  // LEA  (2,A7),A0
	0x0002,
	
	0x72FE,  // MOVEQ #FE,D1
	
	0xD390,  // ADD.L  D1,(A0)
	
	0x2050,  // MOVEA.L  (A0),A0
	
	0x30BC,  // MOVE.W  #0x484A,(A0)
	0x484A,
	
	0x4E73,  // RTE
	
	// OS resumes here
	
	0x027C,  // ANDI #DFFF,SR  ; clear S
	0xDFFF,
	
	0x4FF8,  // LEA  (3072).W,A7
	initial_USP,
	
	0x4EB8,  // JSR  0x0800  ; 2048
	0x0800,
	
	0x4e4F   // TRAP  #15
};

const uint32_t bkpt_7_addr = os_address + 2;
const uint32_t finish_addr = os_address + 4;
const uint32_t trap_0_addr = os_address + 8;

static const uint16_t program[] =
{
	0x41F8,  // LEA  (???).W,A0
	0xFFFF,
	
	0x4878,  // PEA  (0x000C).W
	0x000C,
	
	0x4850,  // PEA  (A0)
	
	0x4878,  // PEA  (0x0001).W
	0x0001,
	
	0x6106,  // BSR.S  *+8
	
	0x4FEF,  // LEA  (12,A7),A7
	0x000C,
	
	0x4E75,  // RTS
	
	0x7004,  // MOVEQ  #4,D0  ; write
	
	0x4E40   // TRAP  #0
};

static void load_vectors( uint8_t* mem )
{
	uint32_t* vectors = (uint32_t*) mem;
	
	memset( vectors, 0xFF, 1024 );
	
	vectors[0] = big_longword( initial_SSP );  // isp
	vectors[1] = big_longword( os_address  );  // pc
	
	vectors[4] = big_longword( bkpt_7_addr );  // Illegal Instruction
	vectors[8] = big_longword( bkpt_7_addr );  // Privilege Violation
	
	vectors[32] = big_longword( trap_0_addr );  // Trap  0
	vectors[47] = big_longword( finish_addr );  // Trap 15
}

static void load_code( uint16_t*        dest,
                       const uint16_t*  begin,
                       const uint16_t*  end )
{
	while ( begin < end )
	{
		*dest++ = big_word( *begin++ );
	}
}

static inline void load_n_words( uint8_t*         mem,
                                 uint32_t         offset,
                                 const uint16_t*  begin,
                                 size_t           n_words )
{
	uint16_t* dest = (uint16_t*) (mem + offset);
	
	load_code( dest, begin, begin + n_words );
}

static void load_data( uint8_t* mem )
{
	uint16_t* code = (uint16_t*) (mem + code_address);
	
	uint8_t* data = (uint8_t*) code + sizeof program;
	
	strcpy( (char*) data, "Hello world\n" );
	
	code[1] = big_word( data - mem );
}

static bool get_stacked_args( const v68k::emulator& emu, uint32_t* out, int n )
{
	uint32_t sp = emu.regs.a[7];
	
	while ( n > 0 )
	{
		sp += 4;
		
		if ( !emu.mem.get_long( sp, *out, emu.data_space() ) )
		{
			return false;
		}
		
		++out;
		--n;
	}
	
	return true;
}

static bool emu_write( v68k::emulator& emu )
{
	uint32_t args[3];  // fd, buffer, length
	
	if ( !get_stacked_args( emu, args, 3 ) )
	{
		return emu.bus_error();
	}
	
	const int fd = int32_t( args[0] );
	
	const uint32_t buffer = args[1];
	
	const size_t length = args[2];
	
	const uint8_t* p = emu.mem.translate( buffer, length, emu.data_space(), v68k::mem_read );
	
	if ( p == NULL )
	{
		errno = EFAULT;
	}
	else
	{
		emu.regs.d[0] = write( fd, p, length );
		emu.regs.d[1] = errno;
	}
	
	return true;
}

static bool bridge_call( v68k::emulator& emu )
{
	const uint16_t call_number = emu.regs.d[0];
	
	switch ( call_number )
	{
		case 4:  return emu_write( emu );
		
		default:
			return false;
	}
}

static void emulator_test()
{
	uint8_t mem[ 4096 ];
	
	load_vectors( mem );
	load_n_words( mem, code_address, program, sizeof program / 2 );
	load_n_words( mem, os_address,   os,      sizeof os      / 2 );
	load_data   ( mem );
	
	const v68k::low_memory_region memory( mem, sizeof mem );
	
	v68k::emulator emu( v68k::mc68000, memory );
	
	emu.reset();
	
step_loop:
	
	while ( emu.step() )
	{
		continue;
	}
	
	if ( emu.condition == v68k::bkpt_2 )
	{
		if ( bridge_call( emu ) )
		{
			emu.acknowledge_breakpoint( 0x4E75 );  // RTS
		}
		
		goto step_loop;
	}
}

int main( int argc, char** argv )
{
	emulator_test();
	
	return 0;
}

