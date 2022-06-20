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

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/combobox.h>
#include <wx/textctrl.h>
#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/collpane.h>
#include <wx/frame.h>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/sstream.h>
#include <wx/txtstrm.h>
#include <wx/clipbrd.h>
#include <wx/process.h>
#include <wx/utils.h>
#include <wx/timer.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/filedlg.h>
#include <wx/dirdlg.h>
#include <wx/dnd.h>
#include <wx/regex.h>
#include <wx/mstream.h>
#include <wx/dcscreen.h>
#include <wx/wupdlock.h>

#include <wxEncodingDetection/wxTextOutputStreamOnString.h>

#endif  // _STD_WX_H

