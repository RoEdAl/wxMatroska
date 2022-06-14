/*
 * StdWx.h
 */

#ifndef _STD_WX_H
#define _STD_WX_H

#ifdef WIN32
#include <targetver.h>

 /*
  * When Winuser.h is defined GetClassInfo is is a macro defined as:
  *
  * #define GetClassInfo GetClassInfoW
  *
  * wxWidgets macros such as:
  *
  * wxDECLARE_..._CLASS
  *
  * declares method GetClassInfo so when Winuser.h is included method GetClassInfo is renamed to GetClassInfoW. That's why we define NOUSER.
  */
#define WIN32_LEAN_AND_MEAN

#define NOUSER
#define NOGDI
#define NOMB
#define NOCOMM

  /*
   * Dummy definition of MSG (LPMSG) to make
   *
   * oleidl.h ole2.h
   *
   * happy.
   */
typedef struct tagMSG
{
} MSG, * LPMSG;
#endif

#ifdef NDEBUG
#define wxDEBUG_LEVEL 0
#else
#define wxDEBUG_LEVEL 1
#endif

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/file.h>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/platinfo.h>
#include <wx/cmdline.h>
#include <wx/regex.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/mstream.h>
#include <wx/datetime.h>
#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

#ifndef NDEBUG
#include <taglib/tdebuglistener.h>
#endif

#if defined( __WIN64__ )
#define wxSizeTFmtSpec wxLongLongFmtSpec
namespace
{
    inline wxTextOutputStream& WriteSizeT(wxTextOutputStream& stream, size_t c)
    {
        stream.Write64(c);
        return stream;
    }
}
#else
#define wxSizeTFmtSpec
namespace
{
    inline wxTextOutputStream& WriteSizeT(wxTextOutputStream& stream, size_t c)
    {
        stream.Write32(c);
        return stream;
    }
}
#endif

#ifdef WIN32
#include <objbase.h>
#endif

#include <optional>
#include <nlohmann/json.hpp>

typedef nlohmann::basic_json<
    std::map,
    std::vector,
    std::string,
    bool,
    wxInt64,
    wxUint64,
    wxDouble,
    std::allocator,
    nlohmann::adl_serializer,
    std::vector< wxByte >
> wxJson;

#endif  // _STD_WX_H

