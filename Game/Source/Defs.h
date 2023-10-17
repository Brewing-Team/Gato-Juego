#ifndef __DEFS_H__
#define __DEFS_H__

#include <stdio.h>

#ifdef __linux__
#define sprintf_s(buffer, size, format, ...) sprintf(buffer, format, __VA_ARGS__)
#include <cstdint>
#elif _WIN32

#endif

//  NULL just in case ----------------------

#ifdef NULL
#undef NULL
#endif
#define NULL  0

// Deletes a buffer
#define RELEASE( x ) \
	{						\
	if( x != NULL )		\
		{					  \
	  delete x;			\
	  x = NULL;			  \
		}					  \
	}

// Deletes an array of buffers
#define RELEASE_ARRAY( x ) \
	{							  \
	if( x != NULL )			  \
		{							\
	  delete[] x;				\
	  x = NULL;					\
		}							\
							  \
	}

#define IN_RANGE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define TO_BOOL( a )  ( (a != 0) ? true : false )

typedef unsigned int uint;
typedef unsigned char uchar;
#ifdef __linux__
//typedef uint32_t uint32;
//#define uint32 Uint32
//#define uint64 Uint64
#elif _WIN32
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif

template <class VALUE_TYPE> void SWAP(VALUE_TYPE& a, VALUE_TYPE& b)
{
	VALUE_TYPE tmp = a;
	a = b;
	b = tmp;
}

// Standard string size
#define SHORT_STR	 32
#define MID_STR	    255
#define HUGE_STR   8192

// Joins a path and file
inline const char* const PATH(const char* folder, const char* file)
{
	static char path[MID_STR];
	sprintf_s(path, MID_STR, "%s/%s", folder, file);
	return path;
}

// Performance macros
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

#endif	// __DEFS_H__