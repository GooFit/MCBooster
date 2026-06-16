// Provides Catch2's main(). Kept in its own TU so the (slow) Catch2 main is
// compiled once and linked into every test executable.
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
