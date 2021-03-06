/*
	v68k-div.cc
	-----------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/emulator.hh"
#include "v68k/endian.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 15 + 15;


using v68k::big_word;
using v68k::big_longword;

using tap::ok_if;


static void divu()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	code[ 1 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	code[ 2 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	code[ 3 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	code[ 4 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	code[ 5 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	code[ 6 ] = big_word( 0x82C0 );  // DIVU.W  D0,D1
	
	code[ 10 ] = big_word( 0x4848 );  // BKPT #0
	
	const uint32_t divide_by_zero_address = 1024 + sizeof (uint16_t) * 10;
	
	vectors[4] = big_longword( divide_by_zero_address );
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0;
	
	emu.regs.d[0] = 0x00000001;
	emu.regs.d[1] = 0x00000000;
	
	
	// 0 / 1 = 0
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00000000 );
	
	ok_if( emu.regs.nzvc == 0x4 );
	
	
	// 20 / 1 = 20
	
	emu.regs.d[1] = 0x0000014;  // 20
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0000014 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 20 / 2 = 10
	
	emu.regs.d[0] = 0x0000002;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0000000A );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 10 / 3 = 3r1
	
	emu.regs.d[0] = 0x0000003;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00010003 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 65539 / 1 = *OVERFLOW*
	
	emu.regs.d[0] = 0x00000001;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00010003 );
	
	ok_if( emu.regs.nzvc == 0x2 );
	
	
	// 65539 / 65534 = 1r5
	
	emu.regs.d[0] = 0x0000FFFE;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00050001 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 327681 / 0 = *EXCEPTION*
	
	emu.regs.nzvc = 0x1;
	
	emu.regs.d[0] = 0x00000000;
	
	emu.step();
	
	ok_if( emu.regs.pc == divide_by_zero_address );
	
	ok_if( emu.regs.d[1] == 0x00050001 );
	
	ok_if( !(emu.regs.nzvc & 0x1) );  // C is always cleared
	
}

static void divs()
{
	using namespace v68k;
	
	uint8_t mem[ 4096 ];
	
	memset( mem, 0xFF, sizeof mem );  // spike memory with bad addresses
	
	uint32_t* vectors = (uint32_t*) mem;
	
	vectors[0] = big_longword( 4096 );  // isp
	vectors[1] = big_longword( 1024 );  // pc
	
	uint16_t* code = (uint16_t*) (mem + 1024);
	
	code[ 0 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	code[ 1 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	code[ 2 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	code[ 3 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	code[ 4 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	code[ 5 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	code[ 6 ] = big_word( 0x83C0 );  // DIVS.W  D0,D1
	
	code[ 10 ] = big_word( 0x4848 );  // BKPT #0
	
	const uint32_t divide_by_zero_address = 1024 + sizeof (uint16_t) * 10;
	
	vectors[4] = big_longword( divide_by_zero_address );
	
	emulator emu( mc68000, mem, sizeof mem );
	
	emu.reset();
	
	emu.regs.nzvc = 0;
	
	emu.regs.d[0] = 0x00000001;
	emu.regs.d[1] = 0x00000000;
	
	
	// 0 / 1 = 0
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00000000 );
	
	ok_if( emu.regs.nzvc == 0x4 );
	
	
	// 20 / 1 = 20
	
	emu.regs.d[1] = 0x0000014;  // 20
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0000014 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 20 / 2 = 10
	
	emu.regs.d[0] = 0x0000002;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0000000A );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 10 / 3 = 3r1
	
	emu.regs.d[0] = 0x0000003;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00010003 );
	
	ok_if( emu.regs.nzvc == 0x0 );
	
	
	// 65539 / 1 = *OVERFLOW*
	
	emu.regs.d[0] = 0x00000001;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x00010003 );
	
	ok_if( emu.regs.nzvc == 0x2 );
	
	
	// 65539 / -4 = -16384r3
	
	emu.regs.d[0] = 0x0000FFFC;
	
	emu.step();
	
	ok_if( emu.regs.d[1] == 0x0003C000 );
	
	ok_if( emu.regs.nzvc == 0x8 );
	
	
	// 245760 / 0 = *EXCEPTION*
	
	emu.regs.nzvc = 0x1;
	
	emu.regs.d[0] = 0x00000000;
	
	emu.step();
	
	ok_if( emu.regs.pc == divide_by_zero_address );
	
	ok_if( emu.regs.d[1] == 0x0003C000 );
	
	ok_if( !(emu.regs.nzvc & 0x1) );  // C is always cleared
	
}

int main( int argc, char** argv )
{
	tap::start( "v68k-div", n_tests );
	
	divu();
	
	divs();
	
	return 0;
}

