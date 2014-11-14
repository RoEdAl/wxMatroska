file(GLOB CUECHAPTERS_HEADER_FILES "*.h")
get_filename_component(CUECHAPTERS_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
file(GLOB CUECHAPTERS_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${CUECHAPTERS_DIR_NAME}/*.h")

set(CUECHAPTERS_SOURCE_FILES
	wxApp.cpp
	wxConfiguration.cpp
	wxCuePointsRenderer.cpp
	wxInputFile.cpp
	wxMkvmergeOptsRenderer.cpp
	wxPrimitiveRenderer.cpp
	wxXmlCueSheetRenderer.cpp
	wxCueChapters.rc
)

ADD_MSVC_PRECOMPILED_HEADER("StdWx.h" "StdWx.cpp" CUECHAPTERS_SOURCE_FILES )
add_executable(cue2mkc
	${CUECHAPTERS_SOURCE_FILES}
	${CUECHAPTERS_HEADER_FILES}
	${CUECHAPTERS_PUBLIC_HEADER_FILES}
)

add_dependencies(cue2mkc wxConsoleApp wxEncodingDetection wxCueFile TagLib)
target_link_libraries(cue2mkc ${wxWidgets_LIBRARIES} Shlwapi.lib
	wxConsoleApp
	wxEncodingDetection
	wxCueFile
	TagLib
)