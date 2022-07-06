/*
 *      StdWx.h
 */

#ifndef _STD_WX_H
#define _STD_WX_H

#include <StdWx.h>

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
#include <wx/dir.h>
#include <wx/base64.h>

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

#include <FmtSpec.h>

#endif  // _STD_WX_H

