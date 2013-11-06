file( GLOB SRC_FILES "src/*.cpp" )
file( GLOB INC_FILES "src/*.hpp" )

include_directories( "src" )

add_executable( cubeshadow ${SRC_FILES} ${INC_FILES} ${COMMON_SOURCE_FILES} ${COMMON_INCLUDE_FILES} )

target_link_libraries(
	cubeshadow
	sfml-graphics sfml-window sfml-system
	GL GLEW
	assimp
)

add_custom_target(
	copyfiles_cubeshadow
	ALL
	COMMAND ${CMAKE_COMMAND} -E echo "-- Copying cubeshadow files."
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:cubeshadow>
)
