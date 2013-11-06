cmake_minimum_required( VERSION 2.8 )

set( CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wextra -pedantic -O3" )

file( GLOB COMMON_SOURCE_FILES "common/src/*.cpp" )
file( GLOB COMMON_INCLUDE_FILES "common/include/*.hpp" )

include_directories( "common/include" )

add_subdirectory( shadowmap )
add_subdirectory( cubemap )
add_subdirectory( cubeshadow )

add_custom_target(
	copyfiles
	ALL
	COMMAND ${CMAKE_COMMAND} -E echo "-- Copying common files."
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/common/resources" "${CMAKE_CURRENT_BINARY_DIR}/common"
)
add_dependencies( copyfiles shadowmap cubemap cubeshadow )
