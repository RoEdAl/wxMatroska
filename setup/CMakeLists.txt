
set( ISCCHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND ISCCHINT "${H}/Inno Setup 5" )
endforeach()
find_program( ISCC "iscc" HINT ${ISCCHINT} )

set( ISETUP_NAME cue2mkc_${EX_PLATFORM_NAME} )
set( ISETUP ${CMAKE_CURRENT_BINARY_DIR}/${CONFIG}/${ISETUP_NAME}${CMAKE_EXECUTABLE_SUFFIX} )

set( ISETUP_SOURCES setup.iss )

add_custom_command( 
	OUTPUT ${ISETUP}
	DEPENDS ${ISETUP_SOURCES}
	COMMAND ${ISCC}
		/o${CMAKE_CURRENT_BINARY_DIR}
		/f${ISETUP_NAME}
		/dCue2MkcExe=$<TARGET_FILE:cue2mkc>
		/dCue2MkcExeGui=${CUE2MKCGUI}
		/dCue2MkcExeArch=${EX_PLATFORM_NAME}
		/Q
		${ISETUP_SOURCES}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS cue2mkcgui cue2mkcgui
	COMMENT "Creating setup ${EX_PLATFORM_NAME}"
)

add_custom_target( isetup SOURCES ${ISETUP_SOURCES} ../cue/ISO-639-2_utf-8.txt ../wxCueChapters/license.txt DEPENDS ${ISETUP} )
add_dependencies( isetup cue2mkc cue2mkcgui )
