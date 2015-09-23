#pragma once

/// Export definition, and enabled if cxxlib_EXPORTS has been defined
///
#ifdef _WIN
#	ifdef cxxlib_EXPORTS
#		define CXXLIB_EXPORT __declspec(dllexport)
#	else
#		define CXXLIB_EXPORT __declspec(dllimport)
#	endif
#else
#	if defined(cxxlib_EXPORTS) && __GNUC__ >= 4
#		define CXXLIB_EXPORT __attribute__((visibility("default")))
#	else
#		define CXXLIB_EXPORT
#	endif
#endif

/// Export internal symbols only for testing usage
///
#ifdef _WIN
#	ifdef cxxlib_internal_EXPORTS
#		define CXXLIB_INTERNAL_EXPORT __declspec(dllexport)
#	else
#		define CXXLIB_INTERNAL_EXPORT __declspec(dllimport)
#	endif
#else
#	if defined(cxxlib_internal_EXPORTS) && __GNUC__ >= 4
#		define CXXLIB_INTERNAL_EXPORT __attribute__((visibility("default")))
#	else
#		define CXXLIB_INTERNAL_EXPORT
#	endif
#endif
