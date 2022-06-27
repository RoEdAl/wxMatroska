
set( ISCCHINT "")
foreach( H ${CMAKE_SYSTEM_PREFIX_PATH} )
	list( APPEND ISCCHINT "${H}/Inno Setup 6" )
endforeach()
find_program( ISCC "iscc" HINT ${ISCCHINT} )

set( ISETUP_NAME cue2mkc_${EX_PLATFORM_NAME} )

set(LICENSE_FILE license.md.txt )
set(LICENSE_FILE_TEMPLATE template.rtf )

set( ISETUP_SOURCE setup.iss )
set( ISETUP_SOURCES ${ISETUP_SOURCE} ${LICENSE_FILE} ${LICENSE_FILE_TEMPLATE} ../cue/ISO-639-2_utf-8.txt )

add_custom_command( 
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.rtf
	MAIN_DEPENDENCY ${LICENSE_FILE}
	DEPENDS ${LICENSE_FILE} ${LICENSE_FILE_TEMPLATE}
	COMMAND pandoc
		-i ${LICENSE_FILE}
		--template ${LICENSE_FILE_TEMPLATE}
		-s
		-t rtf
		-o ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.rtf
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command( 
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.txt
	MAIN_DEPENDENCY ${LICENSE_FILE}
	DEPENDS ${LICENSE_FILE} 
	COMMAND pandoc
		-i ${LICENSE_FILE}
		-s
		-t plain+smart
		-o ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.txt
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

get_filename_component(WXWIDGETS_DLL_SUFFIX ${wxWidgets_LIB_DIR} NAME)
string(FIND ${WXWIDGETS_DLL_SUFFIX} "_" LAST_UND_POS REVERSE)
string(SUBSTRING ${WXWIDGETS_DLL_SUFFIX} 0 ${LAST_UND_POS} WXWIDGETS_DLL_SUFFIX1)
string(SUBSTRING ${WXWIDGETS_DLL_SUFFIX} ${LAST_UND_POS} -1 WXWIDGETS_DLL_SUFFIX2)
string(REPLACE "_" "." WXWIDGETS_DLL_SUFFIX2 ${WXWIDGETS_DLL_SUFFIX2})
string(CONCAT WXWIDGETS_DLL_SUFFIX ${WXWIDGETS_DLL_SUFFIX1} ${WXWIDGETS_DLL_SUFFIX2})

source_group("Setup Files" FILES ${ISETUP_SOURCES})

add_custom_command( 
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${ISETUP_NAME}${CMAKE_EXECUTABLE_SUFFIX}
	MAIN_DEPENDENCY ${ISETUP_SOURCE}
	DEPENDS 
		${ISETUP_SOURCE}
		../cue/ISO-639-2_utf-8.txt
		../wxCueChapters/cmake/dr-scan.cmake
		../wxCueChapters/cmake/mkcover.cmake
		../wxCueChapters/app.tags.json
		${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.txt
		${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.rtf
		$<TARGET_FILE:cue2mkc>
		$<TARGET_FILE:cue2mkc-frontend>
	COMMAND ${ISCC}
		/o${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>
		/f${ISETUP_NAME}
		/dCue2MkcExe=$<TARGET_FILE:cue2mkc>
		/dCue2MkcBase=$<TARGET_FILE_BASE_NAME:cue2mkc>
		/dCue2MkcGuiExe=$<TARGET_FILE:cue2mkc-frontend>
		/dCue2MkcExeArch=${EX_PLATFORM_NAME}
		/dLicenseFileRtf=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.rtf
		/dLicenseFileTxt=${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.txt
		/dWXWidgetsLibDir=${wxWidgets_LIB_DIR}
		/dWXDllSuffix=${WXWIDGETS_DLL_SUFFIX}
		/dWXVerCompact=${WXWIDGETS_VER_COMPACT}
		/dWXDebug=$<$<CONFIG:Debug>:d>
		$<$<NOT:$<CONFIG:Debug>>:/Q>
		${ISETUP_SOURCE}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target( isetup SOURCES ${ISETUP_SOURCES} DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${ISETUP_NAME}${CMAKE_EXECUTABLE_SUFFIX} )
SET_PROPERTY(TARGET isetup PROPERTY FOLDER "setup")

add_custom_command( TARGET isetup POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/license.txt $<TARGET_FILE_DIR:cue2mkc>
	COMMENT "Copying license file"
)
