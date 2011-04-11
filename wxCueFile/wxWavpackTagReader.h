/*
	wxWavpackTagReader.h
*/

#ifndef _WX_WAVPACK_TAG_READER_H_
#define _WX_WAVPACK_TAG_READER_H_

#ifndef _WX_WAVPACK_STREAM_H_
#include "wxWavpackStream.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
class wxArrayCueTag;
#endif

class wxWavpackTagReader :public wxObject
{

public:

	wxWavpackTagReader(void);
	~wxWavpackTagReader(void);

	static bool ReadTags( const wxString&, wxArrayCueTag& );
	static bool ReadCueSheetTag( const wxString&, wxString& );

protected:

	class wxNullOutputStream :public wxOutputStream
	{
		public:

		wxNullOutputStream() {}

		protected:

		virtual size_t OnSysWrite(const void *WXUNUSED(buffer), size_t bufsize ) { return bufsize; }
	};

};

#endif
