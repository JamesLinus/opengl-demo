cmake_minimum_required(VERSION 2.6)
project(skeletalanimation)

file(GLOB SOURCE_FILES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
)

add_executable(skeletalanimation ${SOURCE_FILES})

target_link_libraries(
	skeletalanimation
	${SDL2_LIBRARIES}
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${ASSIMP_LIBRARIES}
)
