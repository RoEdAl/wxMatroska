file(GLOB CUECHAPTERS_HEADER_FILES "*.h")
get_filename_component(CUECHAPTERS_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
file(GLOB CUECHAPTERS_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${CUECHAPTERS_DIR_NAME}/*.h")

source_group("Resource Files" FILES wxCueChapters.rc)

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
	ff-scan.cmake
)

ADD_EXECUTABLE(cue2mkc
	${CUECHAPTERS_SOURCE_FILES}
	${CUECHAPTERS_HEADER_FILES}
	wxCueChapters.rc
	${PROJECT_CONFIG_FILE}
	${CUECHAPTERS_PUBLIC_HEADER_FILES}
)

TARGET_PRECOMPILE_HEADERS(cue2mkc PRIVATE StdWx.h)
TARGET_LINK_LIBRARIES(cue2mkc PRIVATE ${wxWidgets_LIBRARIES} Shlwapi.lib
	wxConsoleApp
	wxEncodingDetection
	wxCmdTools
	wxCueFile
	TagLib
	wxWEBPHandler
	nlohmann_json::nlohmann_json
)

add_custom_command( TARGET cue2mkc POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/ff-scan.cmake $<TARGET_FILE_DIR:cue2mkc>
	COMMENT "Copying ff-scan.cmake script"
)

SET_PROPERTY(TARGET cue2mkc PROPERTY FOLDER "cli")
