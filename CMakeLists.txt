cmake_minimum_required(VERSION 3.10)
project(PLSearchCPP)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add source files
set(SOURCES
    src/pvar.cpp
    src/p_lessthan.cpp
    src/pred.cpp
    src/engine.cpp
)

# Create a static library
add_library(pl_search STATIC ${SOURCES})

# Enable testing
enable_testing()

# Add Catch2
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.7
)
FetchContent_MakeAvailable(catch2)

# Include Catch2
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

# Add test files
add_executable(test_pvar tests/test_pvar.cpp)
target_link_libraries(test_pvar Catch2::Catch2 pl_search)

add_executable(test_engine tests/test_engine.cpp)
target_link_libraries(test_engine Catch2::Catch2 pl_search)

include(CTest)
catch_discover_tests(test_pvar)
catch_discover_tests(test_engine)