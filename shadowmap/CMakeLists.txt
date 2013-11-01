file( GLOB SRC_FILES "src/*.cpp" )
file( GLOB INC_FILES "src/*.hpp" )

include_directories( "src" )

add_executable( shadowmap ${SRC_FILES} ${INC_FILES} )

target_link_libraries(
	shadowmap
	sfml-graphics sfml-window sfml-system
	GL GLEW
	assimp
)

add_custom_command(
	TARGET shadowmap
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E echo "Copying stuff!"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resources" $<TARGET_FILE_DIR:shadowmap>
)
