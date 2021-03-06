#
# InnoSetup.cmake
#
FOREACH(H ${CMAKE_SYSTEM_PREFIX_PATH})
	LIST(APPEND ISCCHINT "${H}/Inno Setup 6")
ENDFOREACH()
FIND_PROGRAM(ISCC "iscc" HINT ${ISCCHINT} NO_CACHE)
IF(NOT ISCC)
	MESSAGE(FATAL_ERROR "InnoSetup not found")
ENDIF()

#CMAKE_PATH(APPEND CPACK_TOPLEVEL_DIRECTORY ${CPACK_PACKAGE_FILE_NAME}.json OUTPUT_VARIABLE JSON_DESC_FILE)
CMAKE_PATH(APPEND CPACK_TOPLEVEL_DIRECTORY ${CPACK_PACKAGE_FILE_NAME} pkg-desc.json OUTPUT_VARIABLE JSON_DESC_FILE)
IF(NOT EXISTS ${JSON_DESC_FILE})
	MESSAGE(FATAL_ERROR "Unable to find ${JSON_DESC_FILE}")
ENDIF()

FILE(READ "${JSON_DESC_FILE}" JSON_DESC)
IF(NOT JSON_DESC)
	MESSAGE(FATAL_ERROR "Unable to read JSON file: ${JSON_DESC_FILE}")
ENDIF()

STRING(JSON CFG_BUILD_CONFIG GET ${JSON_DESC} buildConfig)
IF(NOT CFG_BUILD_CONFIG)
	MESSAGE(FATAL_ERROR "Unknown build config")
ENDIF()
MESSAGE(VERBOSE "Build config: ${CFG_BUILD_CONFIG}")

STRING(JSON CFG_PACKAGE_NAME GET ${JSON_DESC} packageName)
MESSAGE(VERBOSE "Package name: ${CFG_PACKAGE_NAME}")

SET(SETUP_FILES_DIR ${CPACK_TEMPORARY_DIRECTORY}/${CFG_PACKAGE_NAME})
IF(NOT EXISTS ${SETUP_FILES_DIR})
	MESSAGE(FATAL_ERROR "Wrong setup files directory: ${SETUP_FILES_DIR}")
ENDIF()

IF(DEFINED ENV{SOURCE_DATE_EPOCH})
	STRING(TIMESTAMP TOUCH_DATE "%Y-%m-%d" UTC)
	STRING(TIMESTAMP TOUCH_TIME "%H:%M" UTC)
ELSE()
	FILE(TIMESTAMP ${SETUP_FILES_DIR}/cue2mkc.exe TOUCH_DATE "%Y-%m-%d" UTC)
	FILE(TIMESTAMP ${SETUP_FILES_DIR}/cue2mkc.exe TOUCH_TIME "%H:%M" UTC)
ENDIF()

SET(ISETUP_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/setup.iss)

UNSET(ISETUP_ARGS)
IF(${CFG_BUILD_CONFIG} STREQUAL Debug)
	SET(SETUP_FN ${CPACK_PACKAGE_FILE_NAME}-dbg)
ELSE()
	SET(SETUP_FN ${CPACK_PACKAGE_FILE_NAME})
	LIST(APPEND ISETUP_ARGS "/Q")
ENDIF()

MESSAGE(VERBOSE "Working directory: ${SETUP_FILES_DIR}")
MESSAGE(VERBOSE "Output: ${CPACK_TOPLEVEL_DIRECTORY}/${SETUP_FN}.exe")
MESSAGE(VERBOSE "Script: ${ISETUP_SCRIPT}")
MESSAGE(STATUS "InnoSetup: create ${SETUP_FN}.exe [${TOUCH_DATE} ${TOUCH_TIME}: ${CFG_BUILD_CONFIG}]")

EXECUTE_PROCESS( 
	COMMAND ${ISCC}
		/o${CPACK_TOPLEVEL_DIRECTORY}
		/f${SETUP_FN}
		/dCue2MkcFilesDir=${SETUP_FILES_DIR}
		/dCue2MkcBase=${CFG_PACKAGE_NAME}
		/dCue2MkcExe=${SETUP_FILES_DIR}/cue2mkc.exe
		/dSetupIconFile=${CMAKE_CURRENT_LIST_DIR}/../gui/icons/cd_mka.ico
		/dCue2MkcExeArch=x64
		/dCue2MkcTouchDate=${TOUCH_DATE}
		/dCue2MkcTouchTime=${TOUCH_TIME}
		${ISETUP_ARGS}
		${ISETUP_SCRIPT}
	ENCODING UTF-8
	COMMAND_ERROR_IS_FATAL ANY
	WORKING_DIRECTORY ${CPACK_TEMPORARY_DIRECTORY}
)

SET(CPACK_EXTERNAL_BUILT_PACKAGES ${CPACK_TOPLEVEL_DIRECTORY}/${SETUP_FN}.exe)
