/*
 *      StdWx.h
 */

#ifndef _STD_WX_H
#define _STD_WX_H

#include <StdWx.h>

#ifdef WIN32
#include <targetver.h>
#define WIN32_LEAN_AND_MEAN	//
#endif

#ifdef NDEBUG
#define wxDEBUG_LEVEL 0
#else
#define wxDEBUG_LEVEL 1
#endif

#include <wx/object.h>
#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/log.h>
#include <wx/dynarray.h>
#include <wx/arrstr.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/mstream.h>
#include <wx/regex.h>
#include <wx/hashmap.h>
#include <wx/tokenzr.h>
#include <wx/datetime.h>
#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include <wx/dir.h>
#include <wx/base64.h>

#if defined( __WIN64__ )
#define wxSizeTFmtSpec wxLongLongFmtSpec
#else
#define wxSizeTFmtSpec
#endif

#ifdef NDEBUG
#define ENQUOTED_STR_FMT "\u201C%s\u201D"
#else
#define ENQUOTED_STR_FMT "\"%s\""
#endif

// TagLib
#ifndef NDEBUG
#include <taglib/tdebuglistener.h>
#endif
#include <taglib/tpropertymap.h>
#include <taglib/fileref.h>
#include <taglib/wavproperties.h>
#include <taglib/aiffproperties.h>
#include <taglib/apetag.h>
#include <taglib/wavpackfile.h>
#include <taglib/wavpackproperties.h>
#include <taglib/flacproperties.h>
#include <taglib/flacpicture.h>
#include <taglib/flacfile.h>
// #include <mpegproperties.h>

#include <optional>
#include <wxJson.h>

#endif  // _STD_WX_H

