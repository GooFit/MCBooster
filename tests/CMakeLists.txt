# Self-contained test build for MCBooster.
#
# MCBooster is header-only, so this project pulls in its two test-only
# dependencies (Thrust and Catch2) via FetchContent and never touches the
# ROOT/TCLAP example build at the repo root. Run it standalone:
#
#   cmake -S tests -B build-tests
#   cmake --build build-tests
#   ctest --test-dir build-tests --output-on-failure
#
# The backend is selected with -DMCBOOSTER_BACKEND=CPP|OMP (default CPP), so CI
# can exercise the library with no CUDA toolkit installed.

cmake_minimum_required(VERSION 3.14)
project(MCBoosterTests CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MCBOOSTER_BACKEND "CPP" CACHE STRING "Thrust backend for the tests: CPP or OMP")
set_property(CACHE MCBOOSTER_BACKEND PROPERTY STRINGS CPP OMP)

include(FetchContent)

# Thrust: pinned to the exact commit GooFit vendors for its host/OMP builds
# (extern/thrust in GooFit#379; THRUST_VERSION 100803). The CCCL 3.0 path GooFit
# uses only applies to its CUDA 13 builds, which these CPU/OMP tests do not take.
FetchContent_Declare(
  thrust
  GIT_REPOSITORY https://github.com/NVIDIA/thrust.git
  GIT_TAG 8551c97870cd722486ba7834ae9d867f13e299ad)
# Populate only -- Thrust 1.8.3 ships a legacy CMakeLists we do not want to run.
FetchContent_GetProperties(thrust)
if(NOT thrust_POPULATED)
  FetchContent_Populate(thrust)
endif()

# Catch2 v2: the last single-header, C++11-friendly line. Pinned to the same tag
# GooFit vendors (extern/catch2 in GooFit#379).
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.9
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(catch2)

# INTERFACE target carrying everything needed to compile against the library.
add_library(mcbooster INTERFACE)
target_include_directories(mcbooster INTERFACE "${PROJECT_SOURCE_DIR}/.." "${thrust_SOURCE_DIR}")
target_compile_definitions(mcbooster INTERFACE MCBOOSTER_BACKEND=${MCBOOSTER_BACKEND})

if(MCBOOSTER_BACKEND STREQUAL "OMP")
  find_package(OpenMP REQUIRED)
  target_link_libraries(mcbooster INTERFACE OpenMP::OpenMP_CXX)
  target_compile_definitions(
    mcbooster INTERFACE THRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP
                        THRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_CPP)
else()
  target_compile_definitions(
    mcbooster INTERFACE THRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CPP
                        THRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_CPP)
endif()

message(STATUS "MCBooster tests backend: ${MCBOOSTER_BACKEND}")

# Catch2's main(), compiled once and shared across the test executables.
add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)

enable_testing()

set(MCBOOSTER_TESTS test_vector4r test_generate test_evaluate)
foreach(test IN LISTS MCBOOSTER_TESTS)
  add_executable(${test} ${test}.cpp)
  target_link_libraries(${test} PRIVATE mcbooster catch_main)
  add_test(NAME ${test} COMMAND ${test})
endforeach()
