#
# ExternalLibraries.cmake
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.23)
SET(FETCHCONTENT_QUIET ON)
SET(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
SET(FETCHCONTENT_UPDATES_DISCONNECTED ON)
INCLUDE(FetchContent)

# ZLib

FetchContent_Declare(
	zlib
	URL http://zlib.net/zlib-1.2.12.tar.gz
	URL_HASH SHA256=91844808532e5ce316b3c010929493c0244f3d37593afd6de04f71821d5136d9
	PATCH_COMMAND ${CMAKE_COMMAND} -DSCRIPT_FILE=${CMAKE_BINARY_DIR}/_deps/zlib-src/CMakeLists.txt -P ${CMAKE_SOURCE_DIR}/cmp0074.cmake
)

SET(ASM686 OFF CACHE BOOL "" FORCE)
SET(AMD64 OFF CACHE BOOL "" FORCE)
SET(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(zlib)
MESSAGE(STATUS "ZLib source dir: ${zlib_SOURCE_DIR}")

FetchContent_Declare(
	taglib
	URL http://github.com/taglib/taglib/releases/download/v1.12/taglib-1.12.tar.gz
	URL_HASH SHA256=7fccd07669a523b07a15bd24c8da1bbb92206cb19e9366c3692af3d79253b703
	PATCH_COMMAND ${CMAKE_COMMAND} -DTAGLIB_DIR=${CMAKE_BINARY_DIR}/_deps/taglib-src -P ${CMAKE_SOURCE_DIR}/PatchTagLib.cmake
)

FetchContent_Declare(
	webp
	URL http://storage.googleapis.com/downloads.webmproject.org/releases/webp/libwebp-1.2.2.tar.gz
	URL_HASH SHA256=7656532F837AF5F4CEC3FF6BAFE552C044DC39BF453587BD5B77450802F4AEE6
)

FetchContent_Declare(json
	URL http://github.com/nlohmann/json/releases/download/v3.10.5/json.tar.xz
	URL_HASH SHA256=344BE97B757A36C5B180F1C8162F6C5F6EBD760B117F6E64B77866E97B217280
)

# take them all

SET(DBUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
SET(ENABLE_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
SET(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
SET(BUILD_BINDINGS OFF CACHE BOOL "" FORCE)
SET(ZLIB_USE_STATIC_LIBS ON CACHE BOOL "" FORCE)
SET(ZLIB_SOURCE ${zlib_SOURCE_DIR} CACHE PATH "" FORCE)

SET(WEBP_ENABLE_SIMD OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_ANIM_UTILS OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_CWEBP OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_DWEBP OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_GIF2WEBP OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_IMG2WEBP OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_VWEBP OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_WEBPINFO OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_LIBWEBPMUX OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_WEBPMUX OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
SET(WEBP_BUILD_WEBP_JS OFF CACHE BOOL "" FORCE)
SET(WEBP_USE_THREAD ON CACHE BOOL "" FORCE)
SET(WEBP_NEAR_LOSSLESS ON CACHE BOOL "" FORCE)
SET(WEBP_ENABLE_SWAP_16BIT_CSP OFF CACHE BOOL "" FORCE)
SET(WEBP_UNICODE ON CACHE BOOL "" FORCE)
SET(WEBP_BITTRACE 0 CACHE STRING "" FORCE)

FetchContent_MakeAvailable(taglib webp json)

# make aliases

FILE(COPY_FILE ${zlib_BINARY_DIR}/zconf.h ${zlib_SOURCE_DIR}/zconf.h)

ADD_LIBRARY(ZLib ALIAS zlibstatic)
ADD_LIBRARY(TagLib ALIAS tag)
ADD_LIBRARY(WebP ALIAS webp)
ADD_LIBRARY(WebPDemux ALIAS webpdemux)
ADD_LIBRARY(WebPDecoder ALIAS webpdecoder)

SET(FETCHED_LIBS docs example minigzip tag uninstall webp webpencode webpdecode webputilsdecode webpdecoder webpdemux webpdsp webpdspdecode webputils zlib zlibstatic)
FOREACH(L IN LISTS FETCHED_LIBS)
	SET_PROPERTY(TARGET ${L} PROPERTY EXCLUDE_FROM_ALL ON)
	SET_PROPERTY(TARGET ${L} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD ON)
	SET_PROPERTY(TARGET ${L} PROPERTY FOLDER FetchContent)
ENDFOREACH()

# are we done?
