#pragma once

/// Assert macro for with more informations printed out through stderr
///
#define CXXLIB_ASSERT(_COND_)									\
	CXXLIB_ASSERT_X(_COND_, "")

#define CXXLIB_ASSERT_X(_COND_, _REASON_)						\
	CXXLIB_ASSERT_X_(_COND_, __FILE__, __LINE__, _REASON_)

#ifdef _DEBUG
#	define CXXLIB_ASSERT_X_(_COND_, _FILE_, _LINE_, _REASON_)	\
	{															\
		if (!(_COND_))											\
		{														\
			fprintf(											\
				stderr,											\
				"CXXLIB [assertion] \n"							\
				"From	: %s:%u \n"								\
				"Code	: %s \n"								\
				"Reason	: %s \n",								\
				_FILE_, _LINE_, #_COND_, _REASON_);				\
			fflush(stderr);										\
			assert(0 && #_COND_ && _REASON_);					\
		}														\
	}
#else
#	define CXXLIB_ASSERT_X_(_COND_, _FILE_, _LINE_, _REASON_)
#endif

/// Check macro for checking condition in both debug and release mode with stderr error output
///
#define CXXLIB_CHECK(_COND_, _WHERE_)							\
	CXXLIB_CHECK_X(_COND_, _WHERE_, "")

#define CXXLIB_CHECK_X(_COND_, _WHERE_, _REASON_)				\
	{															\
		if (!(_COND_))											\
			CXXLIB_FATAL_X(_WHERE_, _REASON_)					\
	}

#define CXXLIB_CHECK_X_(_COND_, _WHERE_, _FILE_, _LINE_, _REASON_)\
	{															\
		if (!(_COND_))											\
			CXXLIB_FATAL_X_(_WHERE_, _FILE_, _LINE_, _REASON_)	\
	}

/// Fatal macro for stderr error output and exit program in both debug and release mode
///
#define CXXLIB_FATAL_X(_WHERE_, _REASON_)						\
	CXXLIB_FATAL_X_(_WHERE_, __FILE__, __LINE__, _REASON_)

#ifdef _DEBUG
#	define CXXLIB_FATAL_X_(_WHERE_, _FILE_, _LINE_, _REASON_)	\
	{															\
		fprintf(												\
			stderr,												\
			"CXXLIB [Error] \n"									\
			"From	: %s:%u \n"									\
			"Where	: %s \n"									\
			"Reason	: %s \n",									\
			_FILE_, _LINE_, _WHERE_, _REASON_);					\
		fflush(stderr);											\
		assert(0 && _WHERE_ && _REASON_);						\
	}
#else
#	define CXXLIB_FATAL_X_(_WHERE_, _FILE_, _LINE_, _REASON_)	\
	{															\
		fprintf(												\
			stderr,												\
			"CXXLIB [Error] \n"									\
			"Where	: %s \n"									\
			"Reason	: %s \n",									\
			_WHERE_, _REASON_);									\
		fflush(stderr);											\
		exit(1);												\
	}
#endif
