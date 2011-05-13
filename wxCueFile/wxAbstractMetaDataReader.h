/*
   wxAbstractMetaDataReader.h
 */

#ifndef _WX_ABSTRACT_META_DATA_READER_H_
#define _WX_ABSTRACT_META_DATA_READER_H_

class wxAbstractMetaDataReader:
	public wxObject
{
public:

	class wxNullOutputStream:
		public wxOutputStream
	{
public:

		wxNullOutputStream(){}

protected:

		virtual size_t OnSysWrite( const void* WXUNUSED( buffer ), size_t bufsize ){ return bufsize; }
	};
};

#endif