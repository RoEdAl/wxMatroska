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

TARGET_LINK_LIBRARIES(wxCueChapters PRIVATE
	wxConsoleApp
	wxEncodingDetection
	wxCmdTools
	wxCueFile
	TagLib
	wxWEBPHandler
	nlohmann_json::nlohmann_json
	${wxWidgets_LIBRARIES}
)

#TARGET_LINK_OPTIONS(wxCueChapters PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-municode>")

ADD_CUSTOM_COMMAND(TARGET wxCueChapters POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_${WXWIDGETS_DLL_SUFFIX} $<TARGET_FILE_DIR:wxCueChapters>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_xml_${WXWIDGETS_DLL_SUFFIX} $<TARGET_FILE_DIR:wxCueChapters>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_core_${WXWIDGETS_DLL_SUFFIX} $<TARGET_FILE_DIR:wxCueChapters>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/cmake/dr-scan.cmake $<TARGET_FILE_DIR:wxCueChapters>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/cmake/mkcover.cmake $<TARGET_FILE_DIR:wxCueChapters>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/app.tags.json $<TARGET_FILE_DIR:wxCueChapters>/$<TARGET_FILE_BASE_NAME:wxCueChapters>.tags.json
	COMMAND ${CMAKE_COMMAND} -E create_hardlink $<TARGET_FILE:wxCueChapters> $<TARGET_FILE_DIR:wxCueChapters>/$<TARGET_FILE_PREFIX:wxCueChapters>cue2mkc$<TARGET_FILE_SUFFIX:wxCueChapters>
)

IF(DEFINED MINGW64_BASE)
	ADD_CUSTOM_COMMAND(TARGET wxCueChapters POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW64_BASE}/x86_64-w64-mingw32/lib/libstdc++-6.dll $<TARGET_FILE_DIR:wxCueChapters>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW64_BASE}/x86_64-w64-mingw32/lib/libgcc_s_seh-1.dll $<TARGET_FILE_DIR:wxCueChapters>
	)
ENDIF()

SET_PROPERTY(TARGET wxCueChapters PROPERTY FOLDER cli)

ADD_EXECUTABLE(cue2mkc IMPORTED)
SET_TARGET_PROPERTIES(cue2mkc PROPERTIES
	IMPORTED_LOCATION $<TARGET_FILE_DIR:wxCueChapters>/$<TARGET_FILE_PREFIX:wxCueChapters>cue2mkc$<TARGET_FILE_SUFFIX:wxCueChapters>
	FOLDER cli
)

