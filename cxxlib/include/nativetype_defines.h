#pragma once

// Native types

#if (!defined(_BASETSD_H_) || !defined(_WIN))
typedef signed char				INT8, *PINT8;
typedef signed short			INT16, *PINT16;
typedef signed int				INT32, *PINT32;
typedef signed long long int	INT64, *PINT64;
typedef unsigned char			UINT8, *PUINT8;
typedef unsigned short			UINT16, *PUINT16;
typedef unsigned int			UINT32, *PUINT32;
typedef unsigned long long int	UINT64, *PUINT64;
#endif

// Type limits

#ifndef 	INT8_MIN
#	define 	INT8_MIN		(-127 - 1)
#endif
#ifndef 	INT8_MAX
#	define 	INT8_MAX		127
#endif
#ifndef 	UINT8_MAX
#	define 	UINT8_MAX		0xff
#endif

#ifndef 	INT16_MIN
#	define 	INT16_MIN		(-32767 - 1)
#endif
#ifndef 	INT16_MAX
#	define 	INT16_MAX		32767
#endif
#ifndef 	UINT16_MAX
#	define 	UINT16_MAX		0xffff
#endif

#ifndef 	INT_MIN
#	define 	INT_MIN			(-2147483647L - 1)
#endif
#ifndef 	INT_MAX
#	define 	INT_MAX			2147483647L
#endif
#ifndef 	UINT_MAX
#	define 	UINT_MAX		0xffffffffUL
#endif

#ifndef 	INT64_MIN
#	define 	INT64_MIN		(-9223372036854775807LL - 1)
#endif
#ifndef 	INT64_MAX
#	define 	INT64_MAX		9223372036854775807LL
#endif
#ifndef 	UINT64_MAX
#	define 	UINT64_MAX		0xffffffffffffffffULL
#endif

#ifndef SIZE_MAX
#	if defined(_WIN64) || defined(__LP64__) || defined(_LP64) || (__WORDSIZE == 64)
#		define SIZE_MAX UINT64_MAX
#	else
#		define SIZE_MAX UINT_MAX
#	endif
#endif
