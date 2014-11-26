
set( AUT2EXEHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND AUT2EXEHINT "${H}/AutoIt3/Aut2Exe" )
endforeach()
find_program( AUT2EXE "Aut2Exe" HINT ${AUT2EXEHINT} )

set( CUE2MKCGUI ${CMAKE_CURRENT_BINARY_DIR}/cue2mkcgui${CMAKE_EXECUTABLE_SUFFIX} )
set( CUE2MKCGUI ${CMAKE_CURRENT_BINARY_DIR}/cue2mkcgui${CMAKE_EXECUTABLE_SUFFIX} PARENT_SCOPE )

file( GLOB CUE2MKCGUI_ICON_FILES "icons/*.ico" )
set( CUE2MKCGUI_SOURCES cue2mkcgui.au3 ${CUE2MKCGUI_ICON_FILES} )

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
