/*
	wxWavpackTagReader.h
*/

#ifndef _WX_WAVPACK_TAG_READER_H_
#define _WX_WAVPACK_TAG_READER_H_

#ifndef _WX_ABSTRACT_META_DATA_READER_H_
#include "wxAbstractMetaDataReader.h"
#endif

#ifndef _WX_WAVPACK_STREAM_H_
#include "wxWavpackStream.h"
#endif

#ifndef _WX_CUE_COMPONENT_H
class wxArrayCueTag;
#endif

class wxWavpackTagReader :protected wxAbstractMetaDataReader
{

protected:

	wxWavpackTagReader(void);

public:

	static bool ReadTags( const wxString&, wxArrayCueTag& );
	static bool ReadCueSheetTag( const wxString&, wxString& );
};

#endif
