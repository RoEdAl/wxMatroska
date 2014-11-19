/////////////////////////////////////////////////////////////////////////////
// Name:        md5.h
// Purpose:     MD5 file checksum
// Author:      Francesco Montorsi
// Created:     2005/07/13
// RCS-ID:      $Id: md5.h,v 1.4 2005/10/20 16:06:01 frm Exp $
// Copyright:   (c) 2005 Francesco Montorsi
// Licence:     wxWidgets licence + RDS Data Security license
/////////////////////////////////////////////////////////////////////////////

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */
 

#ifndef _WX_MD5_H_
#define _WX_MD5_H_

// wxWidgets headers
#include <wx/buffer.h>
#include <wx/stream.h>

//! A utility class to calculate MD5 checksums from files or strings.
class wxMD5
{
public:
	wxMD5() {}
	virtual ~wxMD5() {}

public:

    // number of bytes in MD5 checksum
    static const size_t HASHBYTES;

    static wxMemoryBuffer Get( const wxFileName& );

	//! Returns the MD5 checksum for the given input stream
    static wxMemoryBuffer Get( wxInputStream &str );

	//! Returns the MD5 for the given memory buffer.
    static wxMemoryBuffer Get( const wxMemoryBuffer& );

    static wxString GeString( const wxMemoryBuffer& );

    static bool AreEqual( const wxMemoryBuffer&, const wxMemoryBuffer& );
};

#endif		// _WX_MD5_H_
