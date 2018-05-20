#pragma once

// Check arguments
#ifndef COMPILE_TESTS
#ifndef COMPILE_MAIN
#error Either COMPILE_TESTS or COMPILE_MAIN must be set!
#endif
#endif

#ifdef COMPILE_TESTS
#ifdef COMPILE_MAIN
#error COMPILE_TESTS and COMPILE_MAIN must not be set together!
#endif
#endif

//
// Take care that there is only one main()
//

#ifdef COMPILE_TESTS
#define MAIN unused_main
#define CATCH_CONFIG_MAIN
#else
#define MAIN main
#define CATCH_CONFIG_DISABLE
#endif

#include <catch.hpp>
