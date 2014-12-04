
set( ISCCHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND ISCCHINT "${H}/Inno Setup 5" )
endforeach()
find_program( ISCC "iscc" HINT ${ISCCHINT} )

set( ISETUP_NAME cue2mkc_${EX_PLATFORM_NAME} )
set( ISETUP_DIR "${CMAKE_CURRENT_BiNARY_DIR}/\$(Configuration)" )
set( ISETUP "${ISETUP_DIR}/${ISETUP_NAME}${CMAKE_EXECUTABLE_SUFFIX}" )

set( ISETUP_SOURCES setup.iss )

add_custom_command( 
	OUTPUT ${ISETUP}
	DEPENDS ${ISETUP_SOURCES}
	COMMAND ${ISCC}
		/o${ISETUP_DIR}
		/f${ISETUP_NAME}
		/dCue2MkcExe=$<TARGET_FILE:cue2mkc>
		/dCue2MkcExeGui=${CUE2MKCGUI}
		/dCue2MkcExeArch=${EX_PLATFORM_NAME}
		/dIconPackDll=$<TARGET_FILE:icon_pack>
		/Q
		${ISETUP_SOURCES}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS cue2mkc cue2mkcgui icon_pack
)

add_custom_target( isetup SOURCES ${ISETUP_SOURCES} ../cue/ISO-639-2_utf-8.txt ../wxCueChapters/license.txt DEPENDS ${ISETUP} )
SET_PROPERTY(TARGET isetup PROPERTY FOLDER "setup")
