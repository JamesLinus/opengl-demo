file( GLOB SRC_FILES "src/*.cpp" )
file( GLOB INC_FILES "src/*.hpp" )

include_directories( "src" )

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

add_executable( shadowmap ${SRC_FILES} ${INC_FILES} ${COMMON_SOURCE_FILES} ${COMMON_INCLUDE_FILES} )

target_link_libraries(
	shadowmap
	${SDL2_LIBRARIES}
	sfml-graphics sfml-window sfml-system
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${ASSIMP_LIBRARIES}
)

add_custom_target(
	copyfiles_shadowmap
	ALL
	COMMAND ${CMAKE_COMMAND} -E echo "-- Copying shadowmap files."
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:shadowmap>
)
