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
