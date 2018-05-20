#pragma once

// expected definition arguments from cmake:
// - PROGRAM_NAME: name of the program/test
// - either COMPILE_TESTS or COMPILE_MAIN: compile the unit tests or the main()?

// Check arguments
#ifndef PROGRAM_NAME
#error PROGRAM_NAME not set!
#endif
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

#ifdef COMPILE_TESTS
#define MAIN unused_main
#else
#define MAIN main
#define BOOST_TEST_NO_MAIN
#endif

#define BOOST_TEST_MODULE PROGRAM_NAME
#include <boost/test/included/unit_test.hpp>
