# icon pack

file( GLOB ICONPACK_ICON_FILES "icons/*.ico" )

set(ICONPACK_SOURCE_FILES
	icon_pack.rc
)

add_library(icon_pack SHARED
	${ICONPACK_ICON_FILES}
	${ICONPACK_SOURCE_FILES}
	${PROJECT_CONFIG_FILE}
)

# resource-only DLL
set_target_properties(icon_pack PROPERTIES LINK_FLAGS "/NOENTRY")
SET_PROPERTY(TARGET icon_pack PROPERTY FOLDER "gui")

# gui

set( AUT2EXEHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND AUT2EXEHINT "${H}/AutoIt3/Aut2Exe" )
endforeach()
find_program( AUT2EXE "Aut2Exe" HINT ${AUT2EXEHINT} )

set( CUE2MKCGUI "${CMAKE_CURRENT_BINARY_DIR}/\$(Configuration)/cue2mkcgui${CMAKE_EXECUTABLE_SUFFIX}" )
set( CUE2MKCGUI "${CMAKE_CURRENT_BINARY_DIR}/\$(Configuration)/cue2mkcgui${CMAKE_EXECUTABLE_SUFFIX}" PARENT_SCOPE )

set( CUE2MKCGUI_SOURCES cue2mkcgui.au3 )

add_custom_command( 
	OUTPUT ${CUE2MKCGUI}
	DEPENDS ${CUE2MKCGUI_SOURCES}
	COMMAND
		${AUT2EXE}
		/in ${CUE2MKCGUI_SOURCES}
		/out ${CUE2MKCGUI}
		/icon icons/cd_mka.ico
		/nopack
		$<$<EQUAL:${EX_PLATFORM},64>:/x64>
		/gui
		/fileversion ${PROJECT_VERSION_STR4}
		/productversion ${PROJECT_VERSION_STR4}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target( cue2mkcgui SOURCES ${CUE2MKCGUI_SOURCES} DEPENDS ${CUE2MKCGUI} )
add_dependencies( cue2mkcgui icon_pack)
SET_PROPERTY(TARGET cue2mkcgui PROPERTY FOLDER "gui")

