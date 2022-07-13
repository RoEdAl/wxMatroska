FILE(GLOB ENCODING_DETECTION_HEADER_FILES "*.h")
GET_FILENAME_COMPONENT(ENCODING_DETECTION_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
FILE(GLOB ENCODING_DETECTION_PUBLIC_HEADER_FILES "${WXMATROSKA_INCLUDE_DIR}/${ENCODING_DETECTION_DIR_NAME}/*.h")
SET(ENCODING_DETECTION_SOURCE_FILES
	wxEncodingDetection.cpp
	wxMLangConvertCharset.cpp
	wxMultiLanguage.cpp
	wxTextInputStreamOnString.cpp
	wxTextOutputStreamOnString.cpp
	wxTextOutputStreamWithBOM.cpp
	wxTextStreamUtils.cpp
	wxMBConvUnaccent.hpp
)

ADD_LIBRARY(wxEncodingDetection 
	${ENCODING_DETECTION_SOURCE_FILES}
	${ENCODING_DETECTION_HEADER_FILES}
	${ENCODING_DETECTION_PUBLIC_HEADER_FILES}
)

ConfigureWxMatroska(wxEncodingDetection)
ConfigureWxWidgetsUsage(wxEncodingDetection)
ConfigureSourceEncoding(wxEncodingDetection)
TARGET_LINK_LIBRARIES(wxEncodingDetection PUBLIC "$<$<CXX_COMPILER_ID:GNU>:mlang>" )
IF(MINGW)
	TARGET_COMPILE_OPTIONS(wxEncodingDetection PRIVATE "-fpermissive")
ENDIF()

SET_PROPERTY(TARGET wxEncodingDetection PROPERTY FOLDER "tool")
