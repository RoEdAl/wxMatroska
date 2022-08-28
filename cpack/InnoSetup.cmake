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

MESSAGE(VERBOSE "Build config: ${CPACK_BUILD_CONFIG}")
MESSAGE(VERBOSE "Component name: ${CPACK_COMPONENTS_ALL}")
MESSAGE(VERBOSE "Package name: ${CPACK_PACKAGE_FILE_NAME}")

SET(SETUP_FILES_DIR ${CPACK_TEMPORARY_DIRECTORY}/${CPACK_COMPONENTS_ALL})
IF(NOT EXISTS ${SETUP_FILES_DIR})
	MESSAGE(FATAL_ERROR "Wrong setup files directory: ${SETUP_FILES_DIR}")
ENDIF()

SET(MAIN_EXE_PATH ${SETUP_FILES_DIR}/cue2mkc.exe)
IF(DEFINED ENV{SOURCE_DATE_EPOCH})
	STRING(TIMESTAMP TOUCH_DATE "%Y-%m-%d" UTC)
	STRING(TIMESTAMP TOUCH_TIME "%H:%M" UTC)
ELSE()
	FILE(TIMESTAMP ${MAIN_EXE_PATH} TOUCH_DATE "%Y-%m-%d" UTC)
	FILE(TIMESTAMP ${MAIN_EXE_PATH} TOUCH_TIME "%H:%M" UTC)
ENDIF()

SET(ISETUP_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/setup.iss)

UNSET(ISETUP_ARGS)
IF(NOT ${CPACK_BUILD_CONFIG} STREQUAL Debug)
	LIST(APPEND ISETUP_ARGS "/Q")
ENDIF()

MESSAGE(VERBOSE "Working directory: ${SETUP_FILES_DIR}")
MESSAGE(VERBOSE "Output: ${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.exe")
MESSAGE(VERBOSE "Script: ${ISETUP_SCRIPT}")
MESSAGE(STATUS "InnoSetup: create ${CPACK_PACKAGE_FILE_NAME}.exe [${TOUCH_DATE} ${TOUCH_TIME}: ${CPACK_BUILD_CONFIG}]")

EXECUTE_PROCESS( 
	COMMAND ${ISCC}
		/o${CPACK_TOPLEVEL_DIRECTORY}
		/f${CPACK_PACKAGE_FILE_NAME}
		/dCue2MkcFilesDir=${SETUP_FILES_DIR}
		/dCue2MkcBase=${CPACK_PACKAGE_NAME}
		/dCue2MkcExe=${MAIN_EXE_PATH}
		/dSetupIconFile=${CMAKE_CURRENT_LIST_DIR}/../gui/icons/cd_mka.ico
		/dCue2MkcExeArch=x64
		/dCue2MkcTouchDate=${TOUCH_DATE}
		/dCue2MkcTouchTime=${TOUCH_TIME}
		/dCMakeVersion=3.24.1
		/dCMakeChecksum=c1b17431a16337d517f7ba78c7067b6f143a12686cb8087f3dd32f3fa45f5aae
		/dMKVToolNixVersion=70.0.0
		/dMKVToolNixChecksum=11d93865550d0af2c4069226185f3aab747976e23ef72f2f76fed5f4cc5976e4
		/dFfMpegVersion=5.1
		/dFfMpegChecksum=2e9910937a8626061b78261a00fa807c9a8c1aad0b878b3e9a8a411176f818ae
		/dImgMagickVersion=7.1.0
		/dImgMagickChecksum=2feca79db7a5ee54d396f0afb132b68b2172d15efee6a6e322abf4abfffd5515
		/dMuPdfVersion=1.20.0
		/dMuPdfChecksum=a847eb233fcbbbb8b8fe1425bc57c3701542036b71dc32188d622fba94b0d4e0		
		${ISETUP_ARGS}
		${ISETUP_SCRIPT}
	ENCODING UTF-8
	COMMAND_ERROR_IS_FATAL ANY
	WORKING_DIRECTORY ${CPACK_TEMPORARY_DIRECTORY}
)

SET(CPACK_EXTERNAL_BUILT_PACKAGES ${CPACK_TOPLEVEL_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.exe)
