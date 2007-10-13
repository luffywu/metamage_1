/*	=============
 *	StackCrawl.cc
 *	=============
 */

#include "Backtrace/StackCrawl.hh"

#include "Backtrace/MemoryLimit.hh"


namespace Backtrace
{
	
	struct StackFrame68K
	{
		StackFrame68K*  next;
		ReturnAddr68K   returnAddr;
	};
	
	struct StackFramePPC
	{
		StackFramePPC*  next;
		const void*     savedCR;
		ReturnAddrPPC   returnAddr;
	};
	
	struct StackFrameX86
	{
		StackFrameX86*   next;
		ReturnAddrMachO  returnAddr;
	};
	
	
#ifdef __MC68K__
	
	typedef StackFrame68K StackFrame;
	
	#pragma parameter __D0 GetA6
	
	inline char* GetA6() = { 0x200e };
	
	inline const StackFrame68K* GetTopFrame()  { return (const StackFrame68K*) GetA6(); }
	
#endif

#ifdef __POWERPC__
	
	typedef StackFramePPC StackFrame;
	
	#ifdef __MWERKS__
		
		static asm char *GetSP( void )
		{
			mr		r3,r1
			blr
		}
		
	#endif
	
	#ifdef __GNUC__
		
		static char *GetSP( void )
		{
			__asm__( "mr r3,r1; blr" );
		}
		
	#endif
	
	inline const StackFramePPC* GetTopFrame()  { return ( (const StackFramePPC*) GetSP() )->next; }
	
#endif
	
#ifdef __i386__
	
	typedef StackFrameX86 StackFrame;
	
	static char *GetEBP( void )
	{
		__asm__( "mov  %ebp,%eax" );
	}
	
	inline const StackFrameX86* GetTopFrame()  { return (const StackFrameX86*) GetEBP(); }
	
#endif
	
	StackFramePtr GetStackFramePointer()
	{
		return reinterpret_cast< StackFramePtr >( GetTopFrame() );
	}
	
	
	static const StackFramePPC* SwitchBackToPPCFrom68K( const StackFrame68K* frame )
	{
		const StackFramePPC* switchFrame = (const StackFramePPC*) frame;
		
		return switchFrame;
	}
	
	static const StackFrame68K* SwitchBackTo68KFromPPC( const StackFramePPC* frame )
	{
		const StackFrame68K* switchFrame = (const StackFrame68K*) ((long) frame - 1);
		
		return switchFrame;
	}
	
	static void CrawlStackPPC( unsigned level, const StackFramePPC* frame, std::vector< CallRecord >& result );
	
	static void CrawlStack68K( unsigned level, const StackFrame68K* frame, std::vector< CallRecord >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
		if ( frame >= MemoryLimit() )
		{
			return;
		}
		
		if ( *((unsigned long*) frame - 1) == 0xffffffff )
		{
			const StackFramePPC* switchFrame = SwitchBackToPPCFrom68K( frame );
			
			CrawlStackPPC( level, switchFrame, result );
			
			return;
		}
		
		ReturnAddr68K addr = frame->returnAddr;
		
		result.push_back( CallRecord( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStack68K( level + 1, frame->next, result );
	}
	
	static void CrawlStackPPC( unsigned level, const StackFramePPC* frame, std::vector< CallRecord >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
	#ifndef __MACH__
		
		if ( frame >= MemoryLimit() )
		{
			return;
		}
		
		if ( level > 100 )
		{
			return;
		}
		
		if ( (long) frame & 0x00000001 )
		{
			const StackFrame68K* switchFrame = SwitchBackTo68KFromPPC( frame );
			
			CrawlStack68K( level, switchFrame, result );
			
			return;
		}
		
	#endif
		
		ReturnAddrPPC addr = frame->returnAddr;
		
		result.push_back( CallRecord( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStackPPC( level + 1, frame->next, result );
	}
	
	static void CrawlStackX86( unsigned level, const StackFrameX86* frame, std::vector< CallRecord >& result )
	{
		if ( frame == NULL )
		{
			return;
		}
		
		ReturnAddrMachO addr = frame->returnAddr;
		
		result.push_back( CallRecord( addr ) );
		
		if ( frame->next < frame )
		{
			return;
		}
		
		CrawlStackX86( level, frame->next, result );
	}
	
	inline void CrawlStack( const StackFrame68K* frame, std::vector< CallRecord >& result )
	{
		CrawlStack68K( 0, frame, result );
	}
	
	inline void CrawlStack( const StackFramePPC* frame, std::vector< CallRecord >& result )
	{
		CrawlStackPPC( 0, frame, result );
	}
	
	inline void CrawlStack( const StackFrameX86* frame, std::vector< CallRecord >& result )
	{
		CrawlStackX86( 0, frame, result );
	}
	
	static std::vector< CallRecord > GetStackCrawl( const StackFrame* top )
	{
		std::vector< CallRecord > result;
		
		try
		{
			CrawlStack( top, result );
		}
		catch ( const std::bad_alloc& )
		{
		}
		
		return result;
	}
	
	std::vector< CallRecord > GetStackCrawl( StackFramePtr top )
	{
		const StackFrame* frame = reinterpret_cast< const StackFrame* >( top );
		
		return GetStackCrawl( frame );
	}
	
	std::vector< CallRecord > GetStackCrawl()
	{
		return GetStackCrawl( GetTopFrame() );
	}
	
}

