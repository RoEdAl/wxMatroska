/*
 * wxCuePointsRenderer.h
 */

#ifndef _WX_CUE_POINTS_RENDERER_H_
#define _WX_CUE_POINTS_RENDERER_H_

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_CUE_SHEET_H_
class wxCueSheet;
#endif

#ifndef _WX_INPUT_FILE_H_
class wxInputFile;
#endif

#ifndef _WX_DATA_FILE_H_
class wxArrayFileName;
#endif

#ifndef _WX_TEXT_OUTPUT_STREAM_ON_STRING_H_
#include <wxCueFile/wxTextOutputStreamOnString.h>
#endif

#ifndef _WX_PRIMITIVE_RENDERER_H_
#include "wxPrimitiveRenderer.h"
#endif

class wxCuePointsRenderer:
    public wxPrimitiveRenderer
{
    public:

    wxCuePointsRenderer(const wxConfiguration&);

    void RenderDisc(const wxCueSheet&);
    bool Save(const wxFileName&);
    const wxFileName& GetFileName() const;
};

#endif

