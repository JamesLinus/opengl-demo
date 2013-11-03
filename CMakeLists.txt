cmake_minimum_required( VERSION 2.8 )

set( CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wextra -pedantic -O3" )

file( GLOB COMMON_SOURCE_FILES "common/src/*.cpp" )
file( GLOB COMMON_INCLUDE_FILES "common/include/*.hpp" )

include_directories( "common/include" )

add_subdirectory( shadowmap )
add_subdirectory( cubemap )
