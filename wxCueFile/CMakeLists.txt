file(GLOB CUEFILE_HEADER_FILES "*.h")
get_filename_component(CUEFILE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
file(GLOB CUEFILE_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${CUEFILE_DIR_NAME}/*.h")

set(CUEFILE_SOURCE_FILES
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
)

ADD_MSVC_PRECOMPILED_HEADER("StdWx.h" "StdWx.cpp" CUEFILE_SOURCE_FILES )

add_library(wxCueFile
	${CUEFILE_SOURCE_FILES}
	${CUEFILE_HEADER_FILES}
	${CUEFILE_PUBLIC_HEADER_FILES}
)

#set_property(TARGET wxCueFile PROPERTY CXX_STANDARD 11)
#set_property(TARGET wxCueFile PROPERTY CXX_EXTENSIONS TRUE)
#set_property(TARGET wxCueFile PROPERTY CXX_STANDARD_REQUIRED TRUE)

add_dependencies(wxCueFile TagLib)
