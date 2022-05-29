/*
 * StdWx.h
 */

#ifndef _STD_WX_H
#define _STD_WX_H

#ifdef WIN32
#include <targetver.h>
#endif

#ifdef NDEBUG
#define wxDEBUG_LEVEL 0
#else
#define wxDEBUG_LEVEL 1
#endif

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/log.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/mstream.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>

#include <mlang.h>
#include <shlwapi.h>
#endif  // _STD_WX_H

