#pragma once

/// Printing out the values of predefined macros at compile time by using #pragma message
/// Usage: 
///		#pragma message(EVAL(CXXLIB_EXPORT))
///
#define VALUE(_V_)					#_V_
#define EVAL(_V_)					#_V_ "=" VALUE(_V_)

/// Concatenating two values as a name
///
#define EXPANDED_CONCAT(_A_, _B_)	_A_##_B_
#define CONCAT(_A_, _B_)			EXPANDED_CONCAT(_A_, _B_)
