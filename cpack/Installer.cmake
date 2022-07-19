#
# Installer.cmake
#

SET(CPACK_PACKAGE_NAME cue2mkc)
SET(CPACK_PACKAGE_VENDOR "Edmunt Pienkowsky")
SET(CPACK_PACKAGE_ICON ../gui/icons/cd_mka.ico)
SET(CPACK_STRIP_FILES ON)
LIST(APPEND CPACK_GENERATOR External)
LIST(APPEND CPACK_SOURCE_GENERATOR Archive)
SET(CPACK_EXTERNAL_ENABLE_STAGING ON)
SET(CPACK_PACKAGE_CHECKSUM SHA1)
SET(CPACK_COMPONENTS_ALL cue2mkc)

CMAKE_PATH(APPEND CMAKE_SOURCE_DIR cpack InnoSetup.cmake OUTPUT_VARIABLE CPACK_EXTERNAL_PACKAGE_SCRIPT)
# CPACK_EXTERNAL_BUILT_PACKAGES

INCLUDE(CPack)