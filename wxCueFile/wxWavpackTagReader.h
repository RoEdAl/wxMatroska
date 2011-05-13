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

class wxWavpackTagReader:
	protected wxAbstractMetaDataReader
{
protected:

	wxWavpackTagReader( void );

protected:

	class wxWavpackContext
	{
protected:

		WavpackContext* m_pContext;

public:

		static WavpackContext* const Null;

		wxWavpackContext():
			m_pContext( Null ){}
		wxWavpackContext( WavpackContext* pContext ):
			m_pContext( pContext ){}
		~wxWavpackContext()
		{
			if ( this->operator bool() )
			{
				WavpackCloseFile( m_pContext );
			}
		}

		WavpackContext* Close()
		{
			if ( this->operator bool() )
			{
				WavpackContext* pRet = WavpackCloseFile( m_pContext );
				m_pContext = Null;
				return pRet;
			}
			else
			{
				return Null;
			}
		}

		int GetNumTagItems();
		size_t GetTagItemIndexed( int, wxWritableCharBuffer&, wxWritableCharBuffer&, wxString&, wxString& );
		size_t GetTagItem( const wxString&, wxWritableCharBuffer& );

		operator bool() const
		{
			return ( m_pContext != Null );
		}
	};

public:

	static bool ReadTags( const wxString&, wxArrayCueTag& );
	static bool ReadCueSheetTag( const wxString&, wxString& );
};

#endif