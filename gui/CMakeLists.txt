
set( AUT2EXEHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND AUT2EXEHINT "${H}/AutoIt3/Aut2Exe" )
endforeach()
find_program( AUT2EXE "Aut2Exe" HINT ${AUT2EXEHINT} )

set( CUE2MKCGUI32 ${CMAKE_CURRENT_BINARY_DIR}/cue2mkcgui.exe )
set( CUE2MKCGUI64 ${CMAKE_CURRENT_BINARY_DIR}/cue2mkcgui64.exe )

file( GLOB CUE2MKCGUI_ICON_FILES "icons/*.ico" )
set( CUE2MKCGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cue2mkcgui.au3 ${CUE2MKCGUI_ICON_FILES} )

add_custom_command( 
	OUTPUT ${CUE2MKCGUI32}
	DEPENDS ${CUE2MKCGUI_SOURCES}
	COMMAND ${AUT2EXE} /in cue2mkcgui.au3 /out ${CUE2MKCGUI32} /gui /nopack /icon icons/cd_mka.ico /fileversion ${PROJECT_VERSION_STR4} /productversion ${PROJECT_VERSION_STR4}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Compiling AutoIt script x86" )

add_custom_command( 
	OUTPUT ${CUE2MKCGUI64}
	DEPENDS ${CUE2MKCGUI_SOURCES}
	COMMAND ${AUT2EXE} /in cue2mkcgui.au3 /out ${CUE2MKCGUI64} /gui /nopack /x64 /icon icons/cd_mka.ico /fileversion ${PROJECT_VERSION_STR4} /productversion ${PROJECT_VERSION_STR4}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Compiling AutoIt script x64" )

add_custom_target( cue2mkcgui SOURCES ${CUE2MKCGUI_SOURCES} DEPENDS ${CUE2MKCGUI32} ${CUE2MKCGUI64} )
