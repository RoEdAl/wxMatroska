cmake_minimum_required(VERSION 3.0)
include(ExternalProject)

MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "$(IntDir)/${PrecompiledBasename}.pch")
    SET(Sources ${${SourcesVar}})

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")
    SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")  
    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF(MSVC)
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)

project(wxmatroska VERSION 1.4.6.0)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

# Test 32/64 bits
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    SET( EX_PLATFORM 64 )
    SET( EX_PLATFORM_NAME "x64" )
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	SET( EX_PLATFORM 32 )
    SET( EX_PLATFORM_NAME "x86" )
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
message(STATUS "Target platform: ${EX_PLATFORM_NAME}")

set(PROJECT_VERSION_STR "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} ${EX_PLATFORM_NAME}")
set(PROJECT_VERSION_STR4 "${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH}, ${PROJECT_VERSION_TWEAK}")
set(PROJECT_VERSION_STR4_DOTTED ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_VERSION_TWEAK})

set(PROJECT_CONFIG_FILE ${PROJECT_BINARY_DIR}/app_config.h)

configure_file(
	${PROJECT_SOURCE_DIR}/app_config.h.in
	${PROJECT_CONFIG_FILE}
)

include_directories( ${PROJECT_BINARY_DIR} )

# installation directory of external libraries

SET(EXT_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/extlib.install/\$(Configuration)")
# set_property(DIRECTORY PROPERTY EP_STEP_TARGETS configure build)
set_property( DIRECTORY PROPERTY EP_BASE "extlib" )

message( External projects dir: ${EXT_INSTALL_PREFIX} )

ExternalProject_Add(
	project_zlib
	URL "http://zlib.net/zlib-1.2.8.tar.gz"
	URL_MD5 "44d667c142d7cda120332623eab69f40"
	CMAKE_ARGS -DASM686=OFF -DAMD64=OFF -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_PREFIX}
)

SET_PROPERTY(TARGET project_zlib PROPERTY FOLDER "extlib")

#ExternalProject_Add(
#	PNG
#	URL "ftp://ftp.simplesystems.org/pub/libpng/png/src/libpng16/libpng-1.6.14.tar.gz"
#	URL_MD5 "2101b3de1d5f348925990f9aa8405660"
#	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_PREFIX}
#)

ExternalProject_Add (
	project_taglib
	GIT_REPOSITORY "https://github.com/taglib/taglib"
	DEPENDS project_zlib
	CMAKE_ARGS -DENABLE_STATIC=ON -DENABLE_STATIC_RUNTIME=OFF -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_PREFIX}
)

SET_PROPERTY(TARGET project_taglib PROPERTY FOLDER "extlib")

# ExternalProject_Add (
#	project_mozjpeg
#	GIT_REPOSITORY "https://github.com/mozilla/mozjpeg"
#	DEPENDS project_zlib
#	CMAKE_ARGS -DWITH_JPEG7=OFF -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_PREFIX} -DNASM=C:/Users/Normal/AppData/Local/nasm/nasm.exe
# )

# SET_PROPERTY(TARGET project_mozjpeg PROPERTY FOLDER "extlib")

#ExternalProject_Add (
#	HARU
#	GIT_REPOSITORY "https://github.com/libharu/libharu"
#	DEPENDS ZLIB PNG
#	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_PREFIX}
#)

#source_group("Include files" REGULAR_EXPRESSION "*([.]h|[.]hpp)$" )

add_definitions(-DTAGLIB_STATIC)

add_library(TagLib STATIC IMPORTED)
set_property(TARGET TagLib PROPERTY IMPORTED_LOCATION ${EXT_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}tag${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(TagLib project_taglib) 

# add_library(MozJpeg STATIC IMPORTED)
# set_property(TARGET MozJpeg PROPERTY IMPORTED_LOCATION ${EXT_INSTALL_PREFIX}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}jpeg-static${CMAKE_STATIC_LIBRARY_SUFFIX})
# add_dependencies(MozJpeg project_mozjpeg) 

IF(MSVC)
set(wxWidgets_CONFIGURATION mswu)
ENDIF()

find_package(wxWidgets COMPONENTS core base zlib png xml REQUIRED)
include(${wxWidgets_USE_FILE})

SET(WXMATROSKA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

include_directories( ${EXT_INSTALL_PREFIX}/include ${WXMATROSKA_INCLUDE_DIR} )

add_subdirectory(wxConsoleApp)
add_subdirectory(wxEncodingDetection)
add_subdirectory(wxCueFile)
add_subdirectory(wxCueChapters)
add_subdirectory(gui)
add_subdirectory(setup)

