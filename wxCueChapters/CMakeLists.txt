file(GLOB CUECHAPTERS_HEADER_FILES "*.h")
get_filename_component(CUECHAPTERS_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
file(GLOB CUECHAPTERS_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${CUECHAPTERS_DIR_NAME}/*.h")

source_group("Resource Files" FILES wxCueChapters.rc)
source_group("CMake Files" FILES cmake/ff-scan.cmake cmake/dr-scan.cmake cmake/mkcover.cmake)

set(CUECHAPTERS_SOURCE_FILES
	wxApp.cpp
	wxConfiguration.cpp
	wxCuePointsRenderer.cpp
	wxInputFile.cpp
	wxMkvmergeOptsRenderer.cpp
	wxPrimitiveRenderer.cpp
	wxFfMetadataRenderer.cpp
	wxCMakeScriptRenderer.cpp
	wxFfmpegCMakeScriptRenderer.cpp
	wxXmlCueSheetRenderer.cpp
	wxTemporaryFilesProvider.cpp
	wxFileNameCorrector.cpp
	cmake/dr-scan.cmake
	cmake/mkcover.cmake
)

ADD_EXECUTABLE(wxCueChapters
	${CUECHAPTERS_SOURCE_FILES}
	${CUECHAPTERS_HEADER_FILES}
	wxCueChapters.rc
	${PROJECT_CONFIG_FILE}
	${CUECHAPTERS_PUBLIC_HEADER_FILES}
)

ConfigureWxMatroska(wxCueChapters)
ConfigureWxWidgetsUsage(wxCueChapters)
ConfigureSourceEncoding(wxCueChapters)
SET_PROPERTY(TARGET wxCueChapters PROPERTY OUTPUT_NAME cue2mkc)
TARGET_LINK_LIBRARIES(wxCueChapters
	PRIVATE
		wxConsoleApp
		wxEncodingDetection
		wxCmdTools
		wxCueFile
		TagLib
		wxWEBPHandler
		nlohmann_json::nlohmann_json
)

SET(MANIFEST_DEFS wxUSE_RC_MANIFEST=1 wxUSE_DPI_AWARE_MANIFEST=0)

SET_SOURCE_FILES_PROPERTIES(wxCueChapters.rc PROPERTIES COMPILE_DEFINITIONS "${MANIFEST_DEFS}")
#TARGET_LINK_OPTIONS(wxCueChapters PRIVATE "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<CONFIG:Debug,RelWithDebInfo>>>:-s>")
#TARGET_LINK_OPTIONS(wxCueChapters PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-municode>")

IF(DEFINED WXWIDGETS_CFG_FILE)
	ADD_CUSTOM_COMMAND(TARGET wxCueChapters POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:wxCueChapters> $<TARGET_FILE_DIR:wxCueChapters>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different cmake/dr-scan.cmake cmake/mkcover.cmake $<TARGET_FILE_DIR:wxCueChapters>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different app.tags.json $<TARGET_FILE_DIR:wxCueChapters>/$<TARGET_FILE_BASE_NAME:wxCueChapters>.tags.json
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		COMMAND_EXPAND_LISTS
	)
ELSE()
	ADD_CUSTOM_COMMAND(TARGET wxCueChapters POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
			${wxWidgets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_${WXWIDGETS_DLL_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
			${wxWidgets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_xml_${WXWIDGETS_DLL_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
			${wxWidgets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}wxmsw${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_core_${WXWIDGETS_DLL_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
			$<TARGET_FILE_DIR:wxCueChapters>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different cmake/dr-scan.cmake cmake/mkcover.cmake $<TARGET_FILE_DIR:wxCueChapters>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different app.tags.json $<TARGET_FILE_DIR:wxCueChapters>/$<TARGET_FILE_BASE_NAME:wxCueChapters>.tags.json
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	)
ENDIF()

IF(DEFINED MINGW_CFG_FILE)
	ADD_CUSTOM_COMMAND(TARGET wxCueChapters POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:GNU_gcc,IMPORTED_LOCATION> $<TARGET_PROPERTY:GNU_stdcxx,IMPORTED_LOCATION> $<TARGET_FILE_DIR:wxCueChapters>
	)
ENDIF()

SET_PROPERTY(TARGET wxCueChapters PROPERTY FOLDER cli)

INSTALL(TARGETS wxCueChapters DESTINATION $<LOWER_CASE:$<CONFIG>>)
INSTALL(FILES cmake/dr-scan.cmake cmake/mkcover.cmake
	DESTINATION $<LOWER_CASE:$<CONFIG>>
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
INSTALL(FILES app.tags.json
	RENAME $<TARGET_FILE_BASE_NAME:wxCueChapters>.tags.json
	DESTINATION $<LOWER_CASE:$<CONFIG>>
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
)
