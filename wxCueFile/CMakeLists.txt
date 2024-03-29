FILE(GLOB CUEFILE_HEADER_FILES "*.h")
GET_FILENAME_COMPONENT(CUEFILE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
FILE(GLOB CUEFILE_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${CUEFILE_DIR_NAME}/*.h")

SET(CUEFILE_SOURCE_FILES
	wxCueComponent.cpp
	wxCueSheet.cpp
	wxCueSheetContent.cpp
	wxCueSheetReader.cpp
	wxCueSheetRenderer.cpp
	wxCueTag.cpp
	wxDataFile.cpp
	wxDuration.cpp
	wxEllipsizer.cpp
	wxIndex.cpp
	wxReduntantSpacesRemover.cpp
	wxSamplingInfo.cpp
	wxTagSynonims.cpp
	wxTextCueSheetRenderer.cpp
	wxTrack.cpp
	wxTrailingSpacesRemover.cpp
	wxUnquoter.cpp
	wxCoverFile.cpp
	wxMd5.cpp
	wxRomanNumeralsConv.cpp
	wxDashesCorrector.cpp
	wxStringCorrector.cpp
	wxNumberFullStopCorrector.cpp
	wxSmallLetterParenthesizedCorrector.cpp
	wxAsciiToUnicode.cpp
	wxStringProcessor.cpp
	wxRegExStringProcessor.cpp
)

ADD_LIBRARY(wxCueFile STATIC
	${CUEFILE_SOURCE_FILES}
	${CUEFILE_HEADER_FILES}
	${CUEFILE_PUBLIC_HEADER_FILES}
)

ConfigureWxMatroska(wxCueFile)
ConfigureWxWidgetsUsage(wxCueFile)
ConfigureSourceEncoding(wxCueFile)
SET_PROPERTY(TARGET wxCueFile PROPERTY OUTPUT_NAME CueFile)
TARGET_LINK_LIBRARIES(wxCueFile
	PUBLIC
		wxEncodingDetection
		wxWEBPHandler
		TagLib
		nlohmann_json::nlohmann_json
)
SET_PROPERTY(TARGET wxCueFile PROPERTY FOLDER "cli")