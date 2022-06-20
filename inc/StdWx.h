/*
 *  StdWx.h
 */
 
#ifndef _COMMON_STD_WX_H_
#define _COMMON_STD_WX_H_
 
#ifndef WXINTL_NO_GETTEXT_MACRO

#ifndef wxNO_IMPLICIT_WXSTRING_ENCODING
    #define _wxS(s)                               wxGetTranslation(wxS(s))
#else
    #define _wxS(s)                               wxGetTranslation(wxString::FromUTF8(s))
#endif

#endif

#endif
