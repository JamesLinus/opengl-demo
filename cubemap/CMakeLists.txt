file( GLOB SRC_FILES "src/*.cpp" )
file( GLOB INC_FILES "src/*.hpp" )

include_directories( "src" )

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)

include_directories(
	${GLEW_INCLUDE_DIRS}
	${ASSIMP_INCLUDE_DIRS}
)

add_executable( cubemap ${SRC_FILES} ${INC_FILES} ${COMMON_SOURCE_FILES} ${COMMON_INCLUDE_FILES} )

target_link_libraries(
	cubemap
	sfml-graphics sfml-window sfml-system
	${OPENGL_LIBRARIES}
	${GLEW_LIBRARIES}
	${ASSIMP_LIBRARIES}
)

add_custom_target(
	copyfiles_cubemap
	ALL
	COMMAND ${CMAKE_COMMAND} -E echo "-- Copying cubemap files."
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:cubemap>
)
