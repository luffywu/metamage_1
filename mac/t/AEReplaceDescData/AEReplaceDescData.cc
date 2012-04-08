/*
	AEReplaceDescData.cc
	--------------------
*/

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mac OS
#include <AEDataModel.h>
#include <MacErrors.h>

#undef check

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 1
                              + 2
                              + 6
                              + 6
                              + 3
                              + 3
                              + 6
                              + 6
                              + !TARGET_API_MAC_CARBON * 4;


using tap::ok_if;


enum Carbon_test_mode
{
	test_default,
	test_OS_9,  // CarbonLib
	test_OS_X   // Carbon.framework
};

Carbon_test_mode global_test_mode = test_default;

static bool passed_for_mode( bool value_for_OS_9, bool value_for_OS_X )
{
	const bool consider_OS_9 = !TARGET_RT_MAC_MACHO    && !global_test_mode  ||  global_test_mode == test_OS_9;
	const bool consider_OS_X =  TARGET_API_MAC_CARBON  && !global_test_mode  ||  global_test_mode == test_OS_X;
	
	return     consider_OS_9  &&  value_for_OS_9
	       ||  consider_OS_X  &&  value_for_OS_X;
}

static void null_NULL_null_NULL()
{
	AEDesc null = { typeNull, NULL };
	
	OSErr err = AEReplaceDescData( typeNull, NULL, 0, &null );
	
	ok_if( err == noErr );
}

static void null_NULL_null_data()
{
	AEDesc desc = { typeNull, NULL };
	
	OSErr err = AEReplaceDescData( typeNull, STR_LEN( "Data" ), &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.dataHandle == NULL );
}

static void null_NULL_type_NULL()
{
	AEDesc desc = { typeNull, NULL };
	
	OSErr err = AEReplaceDescData( typeType, NULL, 0, &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeType );
	
	ok_if( desc.dataHandle != NULL );
	
	ok_if( AEGetDescDataSize( &desc ) == 0 );
	
	char buffer[] = "1234567890abcdef";
	
	err = AEGetDescData( &desc, buffer, sizeof buffer - 1 );
	
	ok_if( err == noErr );
	
	ok_if( memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0 );
	
	AEDisposeDesc( &desc );
}

static void null_NULL_type_data()
{
	AEDesc desc = { typeNull, NULL };
	
	OSErr err = AEReplaceDescData( typeType, STR_LEN( "Data" ), &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeType );
	
	ok_if( desc.dataHandle != NULL );
	
	ok_if( AEGetDescDataSize( &desc ) == 4 );
	
	char buffer[] = "1234567890abcdef";
	
	err = AEGetDescData( &desc, buffer, sizeof buffer - 1 );
	
	ok_if( err == noErr );
	
	ok_if( memcmp( buffer, STR_LEN( "Data567890abcdef" ) ) == 0 );
	
	AEDisposeDesc( &desc );
}


static void null_data_null()
{
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Type" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	desc.descriptorType = typeNull;
	
	OSErr err = AEReplaceDescData( typeNull, NULL, 0, &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeNull );
	ok_if( desc.dataHandle     == NULL     );
}

static void type_data_null()
{
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Type" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	OSErr err = AEReplaceDescData( typeNull, NULL, 0, &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeNull );
	ok_if( desc.dataHandle     == NULL     );
}

static void null_data_data()
{
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeChar, STR_LEN( "Hello world" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	desc.descriptorType = typeNull;
	
	OSType type = 'Type';
	
	OSErr err = AEReplaceDescData( typeType, &type, sizeof type, &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeType );
	ok_if( desc.dataHandle     != NULL     );
	
	ok_if( AEGetDescDataSize( &desc ) == 4 );
	
	type = 0;
	
	err = AEGetDescData( &desc, &type, sizeof type );
	
	ok_if( err == noErr );
	
	ok_if( type == 'Type' );
	
	AEDisposeDesc( &desc );
}


static void type_data_data()
{
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeChar, STR_LEN( "Hello world" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	OSType type = 'Type';
	
	OSErr err = AEReplaceDescData( typeType, &type, sizeof type, &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeType );
	ok_if( desc.dataHandle     != NULL     );
	
	ok_if( AEGetDescDataSize( &desc ) == 4 );
	
	type = 0;
	
	err = AEGetDescData( &desc, &type, sizeof type );
	
	ok_if( err == noErr );
	
	ok_if( type == 'Type' );
	
	AEDisposeDesc( &desc );
}


static void purged_handle()
{
#if !TARGET_API_MAC_CARBON
	
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeChar, STR_LEN( "Hello world" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	EmptyHandle( desc.dataHandle );
	
	OSType type = 'Type';
	
	OSErr err = AEReplaceDescData( typeType, &type, sizeof type, &desc );
	
	ok_if( err == noErr );
	
	ok_if( desc.descriptorType == typeType );
	ok_if( desc.dataHandle     != NULL     );
	
	ok_if( AEGetDescDataSize( &desc ) == 4 );
	
	AEDisposeDesc( &desc );
	
#endif
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		if ( strcmp( argv[ 1 ], "-9" ) == 0 )
		{
			global_test_mode = test_OS_9;
		}
		else if ( strcmp( argv[ 1 ], "-X" ) == 0 )
		{
			global_test_mode = test_OS_X;
		}
	}
	
	tap::start( "AEReplaceDescData", n_tests );
	
	null_NULL_null_NULL();
	
	null_NULL_null_data();
	
	null_NULL_type_NULL();
	
	null_NULL_type_data();
	
	null_data_null();
	
	type_data_null();
	
	null_data_data();
	
	type_data_data();
	
	purged_handle();
	
	return 0;
}
