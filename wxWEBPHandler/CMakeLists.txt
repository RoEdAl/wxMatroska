#
# wxWEBPHandler
#
SET(PROJECT_NAME wxWEBPHandler)

ADD_LIBRARY(wxWEBPHandler STATIC
    imagwebp.cpp
    StdWx.h
    ../inc/wxWEBPHandler/imagwebp.h
)

ConfigureWxMatroska(wxWEBPHandler)
ConfigureWxWidgetsUsage(wxWEBPHandler)
ConfigureSourceEncoding(wxWEBPHandler)

SET_PROPERTY(TARGET wxWEBPHandler PROPERTY OUTPUT_NAME WebPHandler)
TARGET_COMPILE_DEFINITIONS(wxWEBPHandler PUBLIC wxUSE_WEBP)
TARGET_LINK_LIBRARIES(wxWEBPHandler PUBLIC WebP WebPDecoder WebPDemux)

SET_PROPERTY(TARGET wxWEBPHandler PROPERTY FOLDER "tool")