#
# make-workdir.cmake
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.23)

FUNCTION(DownloadPkgSha1 UrlBase FileName Sha1Hash StatusMsg)
	CMAKE_PATH(APPEND CUE2MKC_DLDIR ${FileName} OUTPUT_VARIABLE PkgPath)
	
	IF(EXISTS ${PkgPath})
		FILE(SHA1 ${PkgPath} PkgHash)
		IF(NOT(${PkgHash} STREQUAL ${Sha1Hash}))
			MESSAGE(STATUS "[DL] ${FileName}: expected SHA-1: ${Sha1Hash}, computed SHA-1: ${PkgHash}")
			MESSAGE(STATUS "[DL] Invalid SHA-1 for file ${FileName}")
			FILE(REMOVE ${PkgPath})
		ELSE()
			MESSAGE(VERBOSE "[DL] File ${FileName} already downloaded")
			RETURN()
		ENDIF()
	ENDIF()
	
	MESSAGE(STATUS "[DL] ${StatusMsg}")
	MESSAGE(VERBOSE "[DL] ${UrlBase}/${FileName}")
	FILE(DOWNLOAD ${UrlBase}/${FileName} ${PkgPath}
		EXPECTED_HASH SHA1=${Sha1Hash}
		INACTIVITY_TIMEOUT 60
		TIMEOUT 300
	)
ENDFUNCTION()

SET(WXWIDGETS_VERSION "3.2.0")

SET(CUE2MKC_WORKDIR ${CMAKE_SOURCE_DIR}/..)
CMAKE_PATH(ABSOLUTE_PATH CUE2MKC_WORKDIR NORMALIZE)

SET(INSTALL_MSVC ON)
SET(INSTALL_MINGW64 ON)

MESSAGE(STATUS "[CFG] Workdir: ${CUE2MKC_WORKDIR}")
MESSAGE(STATUS "[CFG] Install MSVC: ${INSTALL_MSVC}")
MESSAGE(STATUS "[CFG] Install MinGW64: ${INSTALL_MINGW64}")

CMAKE_PATH(APPEND CUE2MKC_WORKDIR download OUTPUT_VARIABLE CUE2MKC_DLDIR)

# downloading
SET(URL_WXWIDGETS "http://github.com/wxWidgets/wxWidgets/releases/download/v${WXWIDGETS_VERSION}")
DownloadPkgSha1(${URL_WXWIDGETS} wxWidgets-${WXWIDGETS_VERSION}-headers.7z "75b5271d1a3f08f32557c7a8ca1782310ee279b4" "wxWidgets headers")
DownloadPkgSha1(${URL_WXWIDGETS} wxWidgets-${WXWIDGETS_VERSION}-docs-html.tar.bz2 "9c9caa3b3ce30b7f8b1e30b7a6cc70353b21761d" "wxWidgets docs")

IF(INSTALL_MSVC)
	DownloadPkgSha1(${URL_WXWIDGETS} wxMSW-${WXWIDGETS_VERSION}_vc14x_x64_Dev.7z "65ad095d125dea942b9a74339e0476d14a53e6af" "wxWidgets dev libraries [MSVC x64]")
	DownloadPkgSha1(${URL_WXWIDGETS} wxMSW-${WXWIDGETS_VERSION}_vc14x_x64_ReleaseDLL.7z "c5164b52771aac9973123cfdbeecc5fe8606faba" "wxWidgets libraries [MSVC x64]")
	DownloadPkgSha1(${URL_WXWIDGETS} wxMSW-${WXWIDGETS_VERSION}_vc14x_x64_ReleasePDB.7z "61549c7f12ec7c3398160be3445476a812987f9f" "wxWidgets PDBs [MSVC x64]")
ENDIF()

IF(INSTALL_MINGW64)
	SET(URL_MINGW64 "http://github.com/niXman/mingw-builds-binaries/releases/download/12.1.0-rt_v10-rev3")
	DownloadPkgSha1(${URL_MINGW64} x86_64-12.1.0-release-win32-seh-rt_v10-rev3.7z "3618baf9bb90c7c4d7b2bb419bc680995531d9cc" "MinGW64 runtime")
	DownloadPkgSha1(${URL_WXWIDGETS} wxMSW-${WXWIDGETS_VERSION}_gcc1210_x64_Dev.7z "26a58b3dc1135163921910b69e0ac94f2cbd18a0" "wxWidgets dev libraries [MinGW64]")
	DownloadPkgSha1(${URL_WXWIDGETS} wxMSW-${WXWIDGETS_VERSION}_gcc1210_x64_ReleaseDLL.7z "ca15f4ddc1d9ebf68a6a7764eda60a6369285b2d" "wxWidgets libraries [MinGW64]")
ENDIF()

# extracting

CMAKE_PATH(APPEND CUE2MKC_WORKDIR wx-widgets OUTPUT_VARIABLE CUE2MKC_WXDIR)

CMAKE_PATH(APPEND CUE2MKC_WXDIR include msvc wx setup.h OUTPUT_VARIABLE WX_TEST_FILE)
IF(NOT EXISTS ${WX_TEST_FILE})
	MESSAGE(STATUS "[EXR] wxWidgets headers")
	FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxWidgets-${WXWIDGETS_VERSION}-headers.7z DESTINATION ${CUE2MKC_WXDIR})
ENDIF()

CMAKE_PATH(APPEND CUE2MKC_WXDIR docs index.html OUTPUT_VARIABLE WX_TEST_FILE)
IF(NOT EXISTS ${WX_TEST_FILE})
	MESSAGE(STATUS "[EXR] wxWidgets docs")
	FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxWidgets-${WXWIDGETS_VERSION}-docs-html.tar.bz2 DESTINATION ${CUE2MKC_WXDIR})
	FILE(RENAME ${CUE2MKC_WXDIR}/wxWidgets-${WXWIDGETS_VERSION}-docs-html ${CUE2MKC_WXDIR}/docs NO_REPLACE)
ENDIF()

IF(INSTALL_MSVC)
	CMAKE_PATH(APPEND CUE2MKC_WXDIR lib vc14x_x64_dll wxmsw32ud_core.lib OUTPUT_VARIABLE MSVC_TEST_FILE)
	IF(NOT EXISTS ${MSVC_TEST_FILE})
		MESSAGE(STATUS "[EXR] wxWidgets debug libraries [MSVC x64]")
		FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxMSW-${WXWIDGETS_VERSION}_vc14x_x64_Dev.7z DESTINATION ${CUE2MKC_WXDIR})
	ENDIF()
	
	CMAKE_PATH(APPEND CUE2MKC_WXDIR lib vc14x_x64_dll wxmsw32u_core_vc14x_x64.dll OUTPUT_VARIABLE MSVC_TEST_FILE)
	IF(NOT EXISTS ${MSVC_TEST_FILE})
		MESSAGE(STATUS "[EXR] wxWidgets libraries [MSVC x64]")
		FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxMSW-${WXWIDGETS_VERSION}_vc14x_x64_ReleaseDLL.7z DESTINATION ${CUE2MKC_WXDIR})
	ENDIF()
	
	CMAKE_PATH(APPEND CUE2MKC_WXDIR lib vc14x_x64_dll wxmsw32u_core_vc14x_x64.pdb OUTPUT_VARIABLE MSVC_TEST_FILE)
	IF(NOT EXISTS ${MSVC_TEST_FILE})
		MESSAGE(STATUS "[EXR] wxWidgets PDBs [MSVC x64]")
		FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxMSW-${WXWIDGETS_VERSION}_vc14x_x64_ReleasePDB.7z DESTINATION ${CUE2MKC_WXDIR})
	ENDIF()	
ENDIF()

IF(INSTALL_MINGW64)
	CMAKE_PATH(APPEND CUE2MKC_WORKDIR mingw64 OUTPUT_VARIABLE CUE2MKC_MW64DIR)
	IF(NOT EXISTS ${CUE2MKC_MW64DIR})
		MESSAGE(STATUS "[EXR] MinGW64 runtime")
		FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/x86_64-12.1.0-release-win32-seh-rt_v10-rev3.7z DESTINATION ${CUE2MKC_WORKDIR})
	ENDIF()
	
	CMAKE_PATH(APPEND CUE2MKC_WXDIR lib gcc1210_x64_dll libwxbase32ud.a OUTPUT_VARIABLE MW64_TEST_FILE)
	IF(NOT EXISTS ${MW64_TEST_FILE})
		MESSAGE(STATUS "[EXR] wxWidgets libraries [MinGW64]")
		FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxMSW-${WXWIDGETS_VERSION}_gcc1210_x64_Dev.7z DESTINATION ${CUE2MKC_WXDIR})
	ENDIF()
	
	MESSAGE(DEBUG "[EXR] wxWidgets libraries #1 [MinGW64]")
	FILE(ARCHIVE_EXTRACT INPUT ${CUE2MKC_DLDIR}/wxMSW-${WXWIDGETS_VERSION}_gcc1210_x64_ReleaseDLL.7z DESTINATION ${CUE2MKC_WXDIR})
ENDIF()

# MINGW toolchain

IF(INSTALL_MINGW64)
	CMAKE_PATH(APPEND CUE2MKC_MW64DIR toolchain.cmake OUTPUT_VARIABLE MW64_TOOLCHAIN_FILE)
	IF(NOT EXISTS ${MW64_TOOLCHAIN_FILE})
		MESSAGE(STATUS "[CFGF] Toolchain: ${MW64_TOOLCHAIN_FILE}")
		FILE(WRITE ${MW64_TOOLCHAIN_FILE} "\
#
# MinGW64 CMake toolchain
#
set(MINGW64_BASE \"${CUE2MKC_MW64DIR}\")
set(CMAKE_MAKE_PROGRAM \${MINGW64_BASE}/bin/mingw32-make.exe)
set(CMAKE_C_COMPILER \${MINGW64_BASE}/bin/gcc.exe)
set(CMAKE_CXX_COMPILER \${MINGW64_BASE}/bin/g++.exe)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)		
")
	ENDIF()
ENDIF()

# wxWidgets configuration

SET(WX_VER_COMPACT 32)
IF(INSTALL_MSVC)
	CMAKE_PATH(APPEND CUE2MKC_WXDIR msvc-config.cmake OUTPUT_VARIABLE MSVC_CFG_USE_FILE)
	IF(NOT EXISTS ${MSVC_CFG_USE_FILE})
	MESSAGE(STATUS "[CFGF] MSVC ${MSVC_CFG_USE_FILE}")
	FILE(CONFIGURE OUTPUT ${MSVC_CFG_USE_FILE}
		CONTENT [=[
#
# wxWidgets configuration - MSVC
#
SET(WXWIDGETS_VER_COMPACT @WX_VER_COMPACT@)
SET(WXWIDGETS_DLL_SUFFIX "vc14x_x64.dll")

#libraries
SET(WX_adv ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_adv.lib)
SET(WX_advd ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_adv.lib)
SET(WX_aui ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_aui.lib)
SET(WX_auid ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_aui.lib)
SET(WX_base ${wxWidgets_LIB_DIR}/wxbase32u.lib)
SET(WX_based ${wxWidgets_LIB_DIR}/wxbase32ud.lib)
SET(WX_core ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_core.lib)
SET(WX_cored ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_core.lib)
SET(WX_expat ${wxWidgets_LIB_DIR}/wxexpat.lib)
SET(WX_expatd ${wxWidgets_LIB_DIR}/wxexpatd.lib)
SET(WX_gl ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_gl.lib)
SET(WX_gld ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_gl.lib)
SET(WX_html ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_html.lib)
SET(WX_htmld ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_html.lib)
SET(WX_jpeg ${wxWidgets_LIB_DIR}/wxjpeg.lib)
SET(WX_jpegd ${wxWidgets_LIB_DIR}/wxjpegd.lib)
SET(WX_media ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_media.lib)
SET(WX_mediad ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_media.lib)
SET(WX_net ${wxWidgets_LIB_DIR}/wxbase32u_net.lib)
SET(WX_netd ${wxWidgets_LIB_DIR}/wxbase32ud_net.lib)
SET(WX_png ${wxWidgets_LIB_DIR}/wxpng.lib)
SET(WX_pngd ${wxWidgets_LIB_DIR}/wxpngd.lib)
SET(WX_propgrid ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_propgrid.lib)
SET(WX_propgridd ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_propgrid.lib)
SET(WX_qa ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_qa.lib)
SET(WX_qad ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_qa.lib)
SET(WX_regex ${wxWidgets_LIB_DIR}/wxregexu.lib)
SET(WX_regexd ${wxWidgets_LIB_DIR}/wxregexud.lib)
SET(WX_ribbon ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_ribbon.lib)
SET(WX_ribbond ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_ribbon.lib)
SET(WX_richtext ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_richtext.lib)
SET(WX_richtextd ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_richtext.lib)
SET(WX_scintilla ${wxWidgets_LIB_DIR}/wxscintilla.lib)
SET(WX_scintillad ${wxWidgets_LIB_DIR}/wxscintillad.lib)
SET(WX_stc ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_stc.lib)
SET(WX_stcd ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_stc.lib)
SET(WX_tiff ${wxWidgets_LIB_DIR}/wxtiff.lib)
SET(WX_tiffd ${wxWidgets_LIB_DIR}/wxtiffd.lib)
SET(WX_webview ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_webview.lib)
SET(WX_webviewd ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_webview.lib)
SET(WX_xml ${wxWidgets_LIB_DIR}/wxbase32u_xml.lib)
SET(WX_xmld ${wxWidgets_LIB_DIR}/wxbase32ud_xml.lib)
SET(WX_xrc ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u_xrc.lib)
SET(WX_xrcd ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}ud_xrc.lib)
SET(WX_zlib ${wxWidgets_LIB_DIR}/wxzlib.lib)
SET(WX_zlibd ${wxWidgets_LIB_DIR}/wxzlibd.lib)
SET(wxWidgets_USE_REL_AND_DBG ON)		
]=] @ONLY)		
	ENDIF()
ENDIF()

IF(INSTALL_MINGW64)
	CMAKE_PATH(APPEND CUE2MKC_WXDIR mingw64-config.cmake OUTPUT_VARIABLE MW64_CFG_USE_FILE)
	IF(NOT EXISTS ${MW64_CFG_USE_FILE})
	MESSAGE(STATUS "[CFGF] MinGW64 ${MW64_CFG_USE_FILE}")
	FILE(CONFIGURE OUTPUT ${MW64_CFG_USE_FILE}
		CONTENT [=[
#
# wxWidgets configuration - MinGW64
#
SET(WXWIDGETS_VER_COMPACT @WX_VER_COMPACT@)
SET(WXWIDGETS_DLL_SUFFIX "gcc1210_x64.dll")

#libraries
SET(WX_adv ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_adv.a)
SET(WX_advd ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_adv.a)
SET(WX_aui ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_aui.a)
SET(WX_auid ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_aui.a)
SET(WX_base ${wxWidgets_LIB_DIR}/libwxbase32u.a)
SET(WX_based ${wxWidgets_LIB_DIR}/libwxbase32ud.a)
SET(WX_core ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_core.a)
SET(WX_cored ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_core.a)
SET(WX_expat ${wxWidgets_LIB_DIR}/libwxexpat.a)
SET(WX_expatd ${wxWidgets_LIB_DIR}/libwxexpatd.a)
SET(WX_gl ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_gl.a)
SET(WX_gld ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_gl.a)
SET(WX_html ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_html.a)
SET(WX_htmld ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_html.a)
SET(WX_jpeg ${wxWidgets_LIB_DIR}/libwxjpeg.a)
SET(WX_jpegd ${wxWidgets_LIB_DIR}/libwxjpegd.a)
SET(WX_media ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_media.a)
SET(WX_mediad ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_media.a)
SET(WX_net ${wxWidgets_LIB_DIR}/libwxbase32u_net.a)
SET(WX_netd ${wxWidgets_LIB_DIR}/libwxbase32ud_net.a)
SET(WX_png ${wxWidgets_LIB_DIR}/libwxpng.a)
SET(WX_pngd ${wxWidgets_LIB_DIR}/libwxpngd.a)
SET(WX_propgrid ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_propgrid.a)
SET(WX_propgridd ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_propgrid.a)
SET(WX_qa ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_qa.a)
SET(WX_qad ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_qa.a)
SET(WX_regex ${wxWidgets_LIB_DIR}/libwxregexu.a)
SET(WX_regexd ${wxWidgets_LIB_DIR}/libwxregexud.a)
SET(WX_ribbon ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_ribbon.a)
SET(WX_ribbond ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_ribbon.a)
SET(WX_richtext ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_richtext.a)
SET(WX_richtextd ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_richtext.a)
SET(WX_scintilla ${wxWidgets_LIB_DIR}/libwxscintilla.a)
SET(WX_scintillad ${wxWidgets_LIB_DIR}/libwxscintillad.a)
SET(WX_stc ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_stc.a)
SET(WX_stcd ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_stc.a)
SET(WX_tiff ${wxWidgets_LIB_DIR}/libwxtiff.a)
SET(WX_tiffd ${wxWidgets_LIB_DIR}/libwxtiffd.a)
SET(WX_webview ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_webview.a)
SET(WX_webviewd ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_webview.a)
SET(WX_xml ${wxWidgets_LIB_DIR}/libwxbase32u_xml.a)
SET(WX_xmld ${wxWidgets_LIB_DIR}/libwxbase32ud_xml.a)
SET(WX_xrc ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}u_xrc.a)
SET(WX_xrcd ${wxWidgets_LIB_DIR}/libwxmsw${WXWIDGETS_VER_COMPACT}ud_xrc.a)
SET(WX_zlib ${wxWidgets_LIB_DIR}/libwxzlib.a)
SET(WX_zlibd ${wxWidgets_LIB_DIR}/libwxzlibd.a)
SET(wxWidgets_USE_REL_AND_DBG OFF)		
]=] @ONLY)		
	ENDIF()
ENDIF()

# presets file: CMakeUserPresets.json

CMAKE_PATH(APPEND CMAKE_SOURCE_DIR CMakeUserPresets.json OUTPUT_VARIABLE PRESETS_FILE)
IF(NOT EXISTS ${PRESETS_FILE})
	MESSAGE(STATUS "[CFGF] Preset ${PRESETS_FILE}")
	FILE(CONFIGURE OUTPUT ${PRESETS_FILE}
		CONTENT [=[
{
  "version": 4,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 23,
    "patch": 0
  },
  "configurePresets": [
    {
		"name": "cfg-cue2mkc",
		"hidden": true,
		"installDir": "${sourceDir}/../install",
		"environment": {
			"CUE2MKC_WORKDIR": "@CUE2MKC_WORKDIR@"
		},
		"condition": {
			"type": "equals",
			"lhs": "${hostSystemName}",
			"rhs": "Windows"
		},
		"cacheVariables": {
			"wxWidgets_ROOT_DIR": {
				"type": "PATH",
				"value": "@CUE2MKC_WXDIR@"
			}
		}
	},
    {
		"name": "cfg-cue2mkc-mingw64",
		"hidden": true,
		"inherits": "cfg-cue2mkc",
		"environment": {
			"MINGW64_BASE": "@CUE2MKC_MW64DIR@"
		},
		"cacheVariables": {
			"CMAKE_MAKE_PROGRAM": {
				"type": "FILEPATH",
				"value": "@CUE2MKC_MW64DIR@/bin/mingw32-make.exe"
			},
			"wxWidgets_LIB_DIR": {
				"type": "PATH",
				"value": "@CUE2MKC_WXDIR@/lib/gcc1210_x64_dll"
			},
			"WXWIDGETS_CFG_FILE": {
				"type": "FILEPATH",
				"value": "@MW64_CFG_USE_FILE@"
			}
		},
		"generator": "MinGW Makefiles",
		"toolchainFile": "@MW64_TOOLCHAIN_FILE@"
	},
    {
		"name": "cfg-cue2mkc-msvc",
		"hidden": true,
		"inherits": "cfg-cue2mkc",
		"cacheVariables": {
			"wxWidgets_LIB_DIR": {
				"type": "PATH",
				"value": "@CUE2MKC_WXDIR@/lib/vc14x_x64_dll"
			},
			"WXWIDGETS_CFG_FILE": {
				"type": "FILEPATH",
				"value": "@MSVC_CFG_USE_FILE@"
			}
		},
		"generator": "Visual Studio 17 2022",
		"toolset": "v143,host=x64"
	},	
	{
		"name": "cfg-msvc-x64",
		"displayName": "Microsoft Visual Studio 2022 x64",
		"inherits": "cfg-cue2mkc-msvc",
		"binaryDir": "${sourceDir}/../build-msvc-x64"
	},
	{
		"name": "cfg-mingw64-release",
		"displayName": "MinGW64",
		"inherits": "cfg-cue2mkc-mingw64",
		"binaryDir": "${sourceDir}/../build-mingw64-release",
		"cacheVariables": {
			"CMAKE_BUILD_TYPE": {
				"type": "STRING",
				"value": "MinSizeRel"
			}
		}
	},
		{
		"name": "cfg-mingw64-debug",
		"displayName": "MinGW64",
		"inherits": "cfg-cue2mkc-mingw64",
		"binaryDir": "${sourceDir}/../build-mingw64-debug",
		"cacheVariables": {
			"CMAKE_BUILD_TYPE": {
				"type": "STRING",
				"value": "Debug"
			}
		}
	}
  ],
  "buildPresets": [
	{
		"name": "build-mingw64-release",
		"configurePreset": "cfg-mingw64-release",
		"targets": ["isetup"],
		"cleanFirst": true,
		"jobs": 4
	},
	{
		"name": "build-mingw64-debug",
		"configurePreset": "cfg-mingw64-debug",
		"targets": ["isetup"],
		"cleanFirst": false,
		"jobs": 4
	},
	{
		"name": "build-msvc-release-x64",
		"configurePreset": "cfg-msvc-x64",
		"configuration": "MinSizeRel",
		"targets": ["isetup"],
		"cleanFirst": true
	},
	{
		"name": "build-msvc-debug-x64",
		"configurePreset": "cfg-msvc-x64",
		"configuration": "Debug",
		"targets": ["isetup"],
		"cleanFirst": false
	}	
  ]
}
]=] @ONLY)
ENDIF()
