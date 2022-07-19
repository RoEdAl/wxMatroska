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

CMAKE_PATH(APPEND CPACK_TOPLEVEL_DIRECTORY ${CPACK_PACKAGE_FILE_NAME}.json OUTPUT_VARIABLE JSON_DESC_FILE)
IF(NOT EXISTS ${JSON_DESC_FILE})
	MESSAGE(FATAL_ERROR "Unable to find ${JSON_DESC_FILE}")
ENDIF()
EXECUTE_PROCESS(
	COMMAND ${CMAKE_COMMAND} -E cat ${CPACK_PACKAGE_FILE_NAME}.json
	OUTPUT_VARIABLE JSON_DESC ECHO_OUTPUT_VARIABLE
	RESULT_VARIABLE JSON_DESC_RES
	ENCODING UTF-8
	COMMAND_ERROR_IS_FATAL ANY
	WORKING_DIRECTORY ${CPACK_TOPLEVEL_DIRECTORY}
	COMMAND_ECHO STDERR
)

MESSAGE(STATUS "JSON result: ${JSON_DESC_RES}")
MESSAGE(VERBOSE "JSON file: ${JSON_DESC_FILE}")
FILE(READ "${JSON_DESC_FILE}" JSON_DESC)
IF(NOT JSON_DESC)
	MESSAGE(FATAL_ERROR "Unable to read JSON file: ${JSON_DESC_FILE}")
ENDIF()
MESSAGE(STATUS "JSON: ${JSON_DESC}")
STRING(JSON CFG_BUILD_CONFIG GET ${JSON_DESC} buildConfig)
IF(NOT CFG_BUILD_CONFIG)
	MESSAGE(FATAL_ERROR "Unknown build config")
ENDIF()
STRING(TOLOWER ${CFG_BUILD_CONFIG} CFG_BUILD_CONFIG)
MESSAGE(VERBOSE "Build config: ${CFG_BUILD_CONFIG}")

STRING(JSON CFG_PACKAGE_NAME GET ${JSON_DESC} packageName)
MESSAGE(VERBOSE "Package name: ${CFG_PACKAGE_NAME}")

STRING(JSON CFG_PACKAGE_VERSION GET ${JSON_DESC} packageVersion)
MESSAGE(VERBOSE "Package version: ${CFG_PACKAGE_VERSION}")

SET(SETUP_FILES_DIR ${CPACK_TEMPORARY_DIRECTORY}/cue2mkc/${BUILD_CONFIG})
IF(NOT EXISTS ${SETUP_FILES_DIR})
	MESSAGE(FATAL_ERROR "Wrong setup files directory: ${SETUP_FILES_DIR}")
ENDIF()

MESSAGE(VERBOSE "Working directory: ${SETUP_FILES_DIR}")
MESSAGE(VERBOSE "Output: ${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.exe")

SET(ISETUP_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/setup.iss)

IF(${BUILD_CONFIG} STREQUAL debug)
	UNSET(ISETUP_QUIET)
ELSE()
	SET(ISETUP_QUIET "/Q")
ENDIF()

EXECUTE_PROCESS( 
	COMMAND ${ISCC}
		/o${CPACK_TOPLEVEL_DIRECTORY}
		/f${CPACK_PACKAGE_FILE_NAME}
		/dCue2MkcFilesDir=${SETUP_FILES_DIR}
		/dCue2MkcBase=${CFG_PACKAGE_NAME}
		/dCue2MkcExe=${SETUP_FILES_DIR}/cue2mkc.exe
		/dSetupIconFile=${CMAKE_CURRENT_LIST_DIR}/../gui/icons/cd_mka.ico
		/dCue2MkcExeArch=x64
		${ISETUP_QUIET}
		${ISETUP_SCRIPT}
	ENCODING UTF-8
	COMMAND_ERROR_IS_FATAL ANY
	WORKING_DIRECTORY ${CPACK_TEMPORARY_DIRECTORY}
)

SET(CPACK_EXTERNAL_BUILT_PACKAGES ${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.exe)
