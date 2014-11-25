file(GLOB CUECHAPTERS_HEADER_FILES "*.h")
get_filename_component(CUECHAPTERS_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
file(GLOB CUECHAPTERS_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${CUECHAPTERS_DIR_NAME}/*.h")

set(PROJECT_LICENSE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/license.txt )

set(CUECHAPTERS_SOURCE_FILES
	wxApp.cpp
	wxConfiguration.cpp
	wxCuePointsRenderer.cpp
	wxInputFile.cpp
	wxMkvmergeOptsRenderer.cpp
	wxPrimitiveRenderer.cpp
	wxXmlCueSheetRenderer.cpp
	wxCueChapters.rc
	license.txt
)

ADD_MSVC_PRECOMPILED_HEADER("StdWx.h" "StdWx.cpp" CUECHAPTERS_SOURCE_FILES )
add_executable(cue2mkc
	${CUECHAPTERS_SOURCE_FILES}
	${CUECHAPTERS_HEADER_FILES}
	${PROJECT_CONFIG_FILE}
	${CUECHAPTERS_PUBLIC_HEADER_FILES}
)

add_dependencies(cue2mkc wxConsoleApp wxEncodingDetection wxCueFile TagLib)
target_link_libraries(cue2mkc ${wxWidgets_LIBRARIES} Shlwapi.lib
	wxConsoleApp
	wxEncodingDetection
	wxCueFile
	TagLib
)

add_custom_command( TARGET cue2mkc POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_LICENSE_FILE} $<TARGET_FILE_DIR:cue2mkc>
	COMMENT "Copying license file"
)
