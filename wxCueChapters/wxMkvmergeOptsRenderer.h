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

class wxMkvmergeOptsRenderer:
    public wxPrimitiveRenderer
{
    public:

    wxFileName m_matroskaOptsFile;
    typedef nlohmann::basic_json<
        std::map ,
        std::vector ,
        std::string ,
        bool ,
        wxInt64 ,
        wxUint64 ,
        wxDouble ,
        std::allocator ,
        nlohmann::adl_serializer ,
        std::vector< wxByte >
    > json;

    protected:

    void render_attachments(json& opts, const wxArrayMatroskaAttachment&) const;
    static wxString get_mapping_str( const wxCueSheet& );

    public:

    wxMkvmergeOptsRenderer( const wxConfiguration& );

    void RenderDisc( const wxInputFile& , const wxCueSheet& );
    bool Save();
    const wxFileName& GetMkvmergeOptsFile() const;
};

#endif

