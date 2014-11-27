file( GLOB ICONPACK_ICON_FILES "icons/*.ico" )

# icon pack
set(ICONPACK_SOURCE_FILES
	icon_pack.rc
)

add_library(icon_pack SHARED
	${ICONPACK_ICON_FILES}
	${ICONPACK_SOURCE_FILES}
)

set_target_properties(icon_pack PROPERTIES LINK_FLAGS "/NOENTRY")

# gui

set( AUT2EXEHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND AUT2EXEHINT "${H}/AutoIt3/Aut2Exe" )
endforeach()
find_program( AUT2EXE "Aut2Exe" HINT ${AUT2EXEHINT} )

set( CUE2MKCGUI ${CMAKE_CURRENT_BINARY_DIR}/cue2mkcgui${CMAKE_EXECUTABLE_SUFFIX} )
set( CUE2MKCGUI ${CMAKE_CURRENT_BINARY_DIR}/cue2mkcgui${CMAKE_EXECUTABLE_SUFFIX} PARENT_SCOPE )

set( CUE2MKCGUI_SOURCES cue2mkcgui.au3 )

add_custom_command( 
	OUTPUT ${CUE2MKCGUI}
	DEPENDS ${CUE2MKCGUI_SOURCES}
	COMMAND
		${AUT2EXE}
		/in cue2mkcgui.au3
		/out ${CUE2MKCGUI}
		/gui
		/nopack
		$<$<EQUAL:${EX_PLATFORM},64>:/x64>
		/icon icons/cd_mka.ico
		/fileversion ${PROJECT_VERSION_STR4}
		/productversion ${PROJECT_VERSION_STR4}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Compiling AutoIt script ${EX_PLATFORM_NAME}"
)

add_custom_target( cue2mkcgui SOURCES ${CUE2MKCGUI_SOURCES} DEPENDS ${CUE2MKCGUI} )
add_dependencies(cue2mkc icon_pack)
