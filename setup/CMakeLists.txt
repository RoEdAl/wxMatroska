
set( ISCCHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND ISCCHINT "${H}/Inno Setup 5" )
endforeach()
find_program( ISCC "iscc" HINT ${ISCCHINT} )

set( ISETUP_NAME cue2mkc_${EX_PLATFORM_NAME} )
set( ISETUP_DIR "${CMAKE_CURRENT_BINARY_DIR}/\$(Configuration)" )
set( ISETUP "${ISETUP_DIR}/${ISETUP_NAME}${CMAKE_EXECUTABLE_SUFFIX}" )

set(LICENSE_FILE license.md.txt )
set(PROJECT_LICENSE_FILE_TXT ${CMAKE_CURRENT_BINARY_DIR}/license.txt )
set(PROJECT_LICENSE_FILE_RTF ${CMAKE_CURRENT_BINARY_DIR}/license.rtf )

set( ISETUP_SOURCE setup.iss )
set( ISETUP_SOURCES ${ISETUP_SOURCE} ${LICENSE_FILE} ../cue/ISO-639-2_utf-8.txt )

add_custom_command( 
	OUTPUT ${PROJECT_LICENSE_FILE_RTF}
	MAIN_DEPENDENCY ${LICENSE_FILE}
	DEPENDS ${LICENSE_FILE} 
	COMMAND pandoc
		-i ${LICENSE_FILE}
		-s --smart
		-o ${PROJECT_LICENSE_FILE_RTF}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command( 
	OUTPUT ${PROJECT_LICENSE_FILE_TXT}
	MAIN_DEPENDENCY ${LICENSE_FILE}
	DEPENDS ${LICENSE_FILE} 
	COMMAND pandoc
		-i ${LICENSE_FILE}
		-s --smart
		-t plain
		-o ${PROJECT_LICENSE_FILE_TXT}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command( 
	OUTPUT ${ISETUP}
	MAIN_DEPENDENCY ${ISETUP_SOURCE}
	DEPENDS ${ISETUP_SOURCES} ${PROJECT_LICENSE_FILE_TXT} ${PROJECT_LICENSE_FILE_RTF}
	COMMAND ${ISCC}
		/o${ISETUP_DIR}
		/f${ISETUP_NAME}
		/dCue2MkcExe=$<TARGET_FILE:cue2mkc>
		/dCue2MkcExeGui=${CUE2MKCGUI}
		/dCue2MkcExeArch=${EX_PLATFORM_NAME}
		/dLicenseFileRtf=${PROJECT_LICENSE_FILE_RTF}
		/dLicenseFileTxt=${PROJECT_LICENSE_FILE_TXT}
		/dIconPackDll=$<TARGET_FILE:icon_pack>
		/Q
		${ISETUP_SOURCE}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target( isetup SOURCES ${ISETUP_SOURCES} DEPENDS ${ISETUP} )
add_dependencies( isetup cue2mkc cue2mkcgui icon_pack )
SET_PROPERTY(TARGET isetup PROPERTY FOLDER "setup")

add_custom_command( TARGET isetup POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_LICENSE_FILE_TXT} $<TARGET_FILE_DIR:cue2mkc>
	COMMENT "Copying license file"
)
