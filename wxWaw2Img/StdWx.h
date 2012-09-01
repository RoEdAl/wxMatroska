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
 * declares method GetClassInfo
 * so when Winuser.h is included method GetClassInfo is renamed to
 *    GetClassInfoW.
 * That's why we define NOUSER.
 */
#define WIN32_LEAN_AND_MEAN
#define NOUSER

// #define NOGDI
// #define NOMB
// #define NOCOMM

/*
 * Dummy definition of MSG (LPMSG) to make
 *
 * oleidl.h
 * ole2.h
 *
 * happy.
 */
typedef struct tagMSG
{} MSG, * LPMSG;

#endif

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/string.h>
#include <wx/tokenzr.h>
#include <wx/arrstr.h>
#include <wx/file.h>
#include <wx/app.h>
#include <wx/apptrait.h>
#include <wx/log.h>
#include <wx/platinfo.h>
#include <wx/settings.h>
#include <wx/cmdline.h>
#include <wx/regex.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/mstream.h>
#include <wx/datetime.h>
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/scopedptr.h>
#include <wx/scopedarray.h>
#include <wx/sharedptr.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/dcmemory.h>
#include <wx/geometry.h>
#include <wx/graphics.h>
#include <wx/display.h>
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
#include <wx/msw/enhmeta.h>
#endif
#endif
#include <wx/html/htmprint.h>
#include <wx/dcgraph.h>

#include <math.h>

#ifdef WIN32
#include <objbase.h>
#endif

#endif	// _STD_WX_H

