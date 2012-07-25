/*
 * wxFlacMetaDataReader.cpp
 */

#include "StdWx.h"
#include <wxCueFile/wxCueComponent.h>
#include "wxFlacMetaDataReader.h"

// ===============================================================================

wxFlacMetaDataReader::wxFlacMetaDataReader( void ):
	m_pVorbisComment( ( FLAC::Metadata::VorbisComment* )NULL ),
	m_pCueSheet( ( FLAC::Metadata::CueSheet* )NULL ),
	m_pStreamInfo( ( FLAC::Metadata::StreamInfo* )NULL )
{}

wxFlacMetaDataReader::~wxFlacMetaDataReader( void )
{
	if ( HasVorbisComment() )
	{
		delete m_pVorbisComment;
	}

	if ( HasCueSheet() )
	{
		delete m_pCueSheet;
	}

	if ( HasStreamInfo() )
	{
		delete m_pStreamInfo;
	}
}

bool wxFlacMetaDataReader::HasVorbisComment() const
{
	return ( m_pVorbisComment != (FLAC::Metadata::VorbisComment*)NULL );
}

bool wxFlacMetaDataReader::HasCueSheet() const
{
	return ( m_pCueSheet != (FLAC::Metadata::CueSheet*)NULL );
}

bool wxFlacMetaDataReader::HasStreamInfo() const
{
	return ( m_pStreamInfo != (FLAC::Metadata::StreamInfo*)NULL );
}

const FLAC::Metadata::VorbisComment& wxFlacMetaDataReader::GetVorbisComment() const
{
	wxASSERT( HasVorbisComment() );
	return *m_pVorbisComment;
}

const FLAC::Metadata::StreamInfo& wxFlacMetaDataReader::GetStreamInfo() const
{
	wxASSERT( HasStreamInfo() );
	return *m_pStreamInfo;
}

bool wxFlacMetaDataReader::GetCueSheetFromVorbisComment( wxString& sCueSheet ) const
{
	wxASSERT( HasVorbisComment() );
	const FLAC::Metadata::VorbisComment& vorbisComment = GetVorbisComment();
	unsigned int						 numComments   = vorbisComment.get_num_comments();
	for ( unsigned int i = 0; i < numComments; i++ )
	{
		FLAC::Metadata::VorbisComment::Entry entry = vorbisComment.get_comment( i );
		wxString							 sEntryName( wxString::FromUTF8( entry.get_field_name(), entry.get_field_name_length() ) );

		if ( sEntryName.CmpNoCase( wxCueTag::Name::CUESHEET ) == 0 )
		{
			sCueSheet = wxString::FromUTF8( entry.get_field_value(), entry.get_field_value_length() );
			return true;
		}
	}

	return false;
}

void wxFlacMetaDataReader::ReadVorbisComments( wxArrayCueTag& comments ) const
{
	wxASSERT( HasVorbisComment() );
	comments.Empty();
	const FLAC::Metadata::VorbisComment& vorbisComment = GetVorbisComment();
	unsigned int						 numComments   = vorbisComment.get_num_comments();
	for ( unsigned int i = 0; i < numComments; i++ )
	{
		FLAC::Metadata::VorbisComment::Entry entry = vorbisComment.get_comment( i );
		wxString							 sEntryName( wxString::FromUTF8( entry.get_field_name(), entry.get_field_name_length() ) );
		wxString							 sEntryValue( wxString::FromUTF8( entry.get_field_value(), entry.get_field_value_length() ) );
		wxCueTag							 comment( wxCueTag::TAG_MEDIA_METADATA, sEntryName, sEntryValue );
		comments.Add( comment );
	}
}

const FLAC::Metadata::CueSheet& wxFlacMetaDataReader::GetCueSheet() const
{
	wxASSERT( HasCueSheet() );
	return *m_pCueSheet;
}

void wxFlacMetaDataReader::metadata_callback( const::FLAC__StreamMetadata* metadata )
{
	switch ( metadata->type )
	{
		case FLAC__METADATA_TYPE_STREAMINFO:
		{
			wxASSERT( !HasStreamInfo() );
			m_pStreamInfo = new FLAC::Metadata::StreamInfo( (::FLAC__StreamMetadata*)metadata, true );
			break;
		}

		case FLAC__METADATA_TYPE_VORBIS_COMMENT:
		{
			wxASSERT( !HasVorbisComment() );
			m_pVorbisComment = new FLAC::Metadata::VorbisComment( (::FLAC__StreamMetadata*)metadata, true );
			break;
		}

		case FLAC__METADATA_TYPE_CUESHEET:
		{
			wxASSERT( !HasCueSheet() );
			m_pCueSheet = new FLAC::Metadata::CueSheet( (::FLAC__StreamMetadata*)metadata, true );
			break;
		}
	}
}

bool wxFlacMetaDataReader::ReadMetadata( const wxString& sFlacFile )
{
	wxFileInputStream is( sFlacFile );

	if ( !is.IsOk() )
	{
		return false;
	}

	wxNullOutputStream os;

	FlacDecoder decoder( is, os, *this );
	decoder.set_md5_checking( true );
	decoder.set_metadata_respond_all();
	::FLAC__StreamDecoderInitStatus status = decoder.init();

	if ( status != FLAC__STREAM_DECODER_INIT_STATUS_OK )
	{
		wxString sStatusStr( FLAC__StreamDecoderInitStatusString[ status ] );
		wxLogWarning( _( "Fail to initialize FLAC decoder %d %s" ), (int)status, sStatusStr );
		return false;
	}

	bool process = decoder.process_until_end_of_metadata();

	if ( !process )
	{
		wxLogWarning( _( "Fail to process FLAC metadata" ) );
		decoder.finish();
		return false;
	}

	decoder.finish();
	m_sFlacFile = sFlacFile;
	return true;
}

const wxString& wxFlacMetaDataReader::GetFlacFile() const
{
	return m_sFlacFile;
}

