/*
 *      StdWx.h
 */

#ifndef _STD_WX_H
#define _STD_WX_H

#ifdef WIN32
#include <targetver.h>
#define WIN32_LEAN_AND_MEAN																																																																																																																																																																																																																																																																//
#endif

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
#include <wx/mimetype.h>
#include <wx/base64.h>

#if defined( __WIN64__ )
#define wxSizeTFmtSpec wxLongLongFmtSpec
#else
#define wxSizeTFmtSpec
#endif

// TagLib
#ifdef __WXDEBUG__
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
#endif	// _STD_WX_H

