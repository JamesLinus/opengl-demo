cmake_minimum_required( VERSION 2.8 )

project( opengl-demo )

set( CMAKE_CXX_FLAGS "-g -std=c++11 -Wall -Wextra -pedantic -O3" )

file( GLOB SRC_FILES "src/*.cpp" )
file( GLOB INC_FILES "src/*.hpp" )

include_directories( "src" )

add_executable( opengl-demo ${SRC_FILES} ${INC_FILES} )

target_link_libraries(
	opengl-demo
	sfml-window sfml-system
	GL GLEW
)
