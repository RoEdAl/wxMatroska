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
SET_PROPERTY(TARGET wxCueChaptersGui PROPERTY OUTPUT_NAME cue2mkc-frontend)
TARGET_LINK_LIBRARIES(wxCueChaptersGui
	PRIVATE wxCmdTools wxEncodingDetection
)

SET(MANIFEST_DEFS wxUSE_RC_MANIFEST=1 wxUSE_DPI_AWARE_MANIFEST=2)
SET_SOURCE_FILES_PROPERTIES(wxCueChaptersGui.rc PROPERTIES COMPILE_DEFINITIONS "${MANIFEST_DEFS}")
IF(DEFINED WXWIDGETS_CFG_FILE)
	ADD_CUSTOM_COMMAND(TARGET wxCueChaptersGui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:wxCueChaptersGui> $<TARGET_FILE_DIR:wxCueChaptersGui>
		COMMAND_EXPAND_LISTS
	)
ELSE()
	ADD_CUSTOM_COMMAND(TARGET wxCueChaptersGui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different 
			${wxWidgets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_${WXWIDGETS_DLL_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
			${wxWidgets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}wxbase${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_xml_${WXWIDGETS_DLL_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
			${wxWidgets_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}wxmsw${WXWIDGETS_VER_COMPACT}u$<$<CONFIG:Debug>:d>_core_${WXWIDGETS_DLL_SUFFIX}${CMAKE_SHARED_LIBRARY_SUFFIX}
			$<TARGET_FILE_DIR:wxCueChaptersGui>
	)
ENDIF()

IF(DEFINED MINGW_CFG_FILE)
	ADD_CUSTOM_COMMAND(TARGET wxCueChaptersGui POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_PROPERTY:GNU_gcc,IMPORTED_LOCATION> $<TARGET_PROPERTY:GNU_stdcxx,IMPORTED_LOCATION> $<TARGET_FILE_DIR:wxCueChaptersGui>
	)
ENDIF()

SET_PROPERTY(TARGET wxCueChaptersGui PROPERTY FOLDER gui)
INSTALL(
	TARGETS wxCueChaptersGui
	DESTINATION cue2mkc
	COMPONENT cue2mkc
)

