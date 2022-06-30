/*
 * wxMkvmergeOptsRenderer.h
 */

#ifndef _WX_MKVMERGE_OPTS_RENDERER_H_
#define _WX_MKVMERGE_OPTS_RENDERER_H_

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

#ifndef _WX_CMAKE_SCRIPT_RENDERER_H_
#include "wxCMakeScriptRenderer.h"
#endif

class wxMkvmergeOptsRenderer: public wxCMakeScriptRenderer
{
    wxDECLARE_NO_COPY_CLASS(wxMkvmergeOptsRenderer);

    public:

    wxMkvmergeOptsRenderer(const wxConfiguration&);

    void RenderOptions(const wxInputFile&, const wxCueSheet&, const wxString&, const wxFileName&, const wxFileName&, const wxFileName&, wxFileName&, wxMatroskaAttachment&, wxFileName&);
    void RenderScript(const wxInputFile&, const wxCueSheet&, const wxString&, const wxFileName&, const wxFileName&, const wxMatroskaAttachment&, const wxFileName&);
    bool Save(const wxFileName&);
};

#endif

