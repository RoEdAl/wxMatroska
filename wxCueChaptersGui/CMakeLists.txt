FILE(GLOB CUECHAPTERS_GUI_HEADER_FILES "*.h")
file(GLOB CUECHAPTERS_ICON_FILES "../gui/icons/*.ico" )
GET_FILENAME_COMPONENT(CUECHAPTERS_GUI_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )

source_group("Resource Files" FILES wxCueChaptersGui.rc ${CUECHAPTERS_ICON_FILES})

SET(CUECHAPTERS_GUI_SOURCE_FILES
	wxApp.cpp
	wxMainFrame.cpp
	LogListBox.cpp
)

ADD_EXECUTABLE(wxCueChaptersGui WIN32
	${CUECHAPTERS_GUI_SOURCE_FILES}
	${CUECHAPTERS_GUI_HEADER_FILES}
	wxCueChaptersGui.rc
	${CUECHAPTERS_ICON_FILES}
	${PROJECT_CONFIG_FILE}
)

ConfigureWxMatroska(wxCueChaptersGui)
ConfigureWxWidgetsUsage(wxCueChaptersGui)
ConfigureSourceEncoding(wxCueChaptersGui)
#TARGET_LINK_OPTIONS(wxCueChaptersGui PRIVATE "$<$<CXX_COMPILER_ID:GNU>:-municode>")
TARGET_LINK_LIBRARIES(wxCueChaptersGui wxCmdTools wxEncodingDetection ${wxWidgets_LIBRARIES})

ADD_CUSTOM_COMMAND(TARGET wxCueChaptersGui POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_${WXWIDGETS_DLL_SUFFIX} $<TARGET_FILE_DIR:wxCueChaptersGui>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${wxWidgets_LIB_DIR}/wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_xml_${WXWIDGETS_DLL_SUFFIX} $<TARGET_FILE_DIR:wxCueChaptersGui>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${wxWidgets_LIB_DIR}/wxmsw${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_core_${WXWIDGETS_DLL_SUFFIX} $<TARGET_FILE_DIR:wxCueChaptersGui>
)

IF(DEFINED MINGW64_BASE)
	ADD_CUSTOM_COMMAND(TARGET wxCueChaptersGui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW64_BASE}/x86_64-w64-mingw32/lib/libstdc++-6.dll $<TARGET_FILE_DIR:wxCueChaptersGui>
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MINGW64_BASE}/x86_64-w64-mingw32/lib/libgcc_s_seh-1.dll $<TARGET_FILE_DIR:wxCueChaptersGui>
	)
ENDIF()

SET_PROPERTY(TARGET wxCueChaptersGui PROPERTY FOLDER "gui")
ADD_EXECUTABLE(cue2mkc-frontend ALIAS wxCueChaptersGui)
