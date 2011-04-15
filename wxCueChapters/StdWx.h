/*
	StdWx.h
*/

#ifndef _STD_WX_H
#define _STD_WX_H

#ifdef WIN32
#include <targetver.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
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
#include <wx/datetime.h>
#include <wx/dynlib.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>

#include <FLAC++/all.h>
#include <wavpack.h>

#include <objbase.h>

extern wxXmlNode* const wxNullXmlNode;
extern wxXmlDocument* const wxNullXmlDocument;

#endif // _STD_WX_H

