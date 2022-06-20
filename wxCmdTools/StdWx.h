/*
 * StdWx.h
 */

#ifndef _STD_WX_H
#define _STD_WX_H

#include <StdWx.h>

#ifdef WIN32
#include <targetver.h>
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef NDEBUG
#define wxDEBUG_LEVEL 0
#else
#define wxDEBUG_LEVEL 1
#endif

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/string.h>
#include <wx/log.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/stdpaths.h>

#endif  // _STD_WX_H

