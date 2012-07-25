/*
 * wxInputFile.cpp
 */

#include "StdWx.h"
#include "wxInputFile.h"

// ===============================================================================

const wxChar wxInputFile::SEPARATOR = wxT( ';' );

wxIMPLEMENT_DYNAMIC_CLASS( wxInputFile, wxObject );

// ===============================================================================

void wxInputFile::copy( const wxInputFile& inputFile )
{
	m_inputFile = inputFile.m_inputFile;
	m_dataFile	= inputFile.m_dataFile;
}

wxInputFile::wxInputFile( const wxInputFile& inputFile )
{
	copy( inputFile );
}

wxInputFile& wxInputFile::operator =( const wxInputFile& inputFile )
{
	copy( inputFile );
	return *this;
}

wxInputFile::wxInputFile( const wxString& sInputFile )
{
	wxStringTokenizer tokenizer( sInputFile, SEPARATOR );

	if ( tokenizer.HasMoreTokens() )
	{
		m_inputFile.Assign( tokenizer.GetNextToken() );
	}

	while ( tokenizer.HasMoreTokens() )
	{
		wxFileName dataFile( tokenizer.GetNextToken() );
		m_dataFile.Add( dataFile );
	}
}

wxInputFile::wxInputFile( void )
{}

bool wxInputFile::IsOk() const
{
	return m_inputFile.IsOk();
}

const wxFileName& wxInputFile::GetInputFile() const
{
	return m_inputFile;
}

bool wxInputFile::HasDataFiles() const
{
	return !m_dataFile.IsEmpty();
}

const wxArrayFileName& wxInputFile::GetDataFiles() const
{
	return m_dataFile;
}

void wxInputFile::GetDataFiles( wxArrayDataFile& dataFile, wxDataFile::FileType fileType ) const
{
	dataFile.Clear();
	for ( size_t i = 0, nCount = m_dataFile.GetCount(); i < nCount; i++ )
	{
		wxDataFile df( m_dataFile[ i ].GetFullPath(), fileType );
		dataFile.Add( df );
	}
}

wxInputFile& wxInputFile::SetInputFile( const wxFileName& fileName )
{
	m_inputFile = fileName;
	return *this;
}

wxInputFile& wxInputFile::SetDataFiles( const wxArrayFileName& dataFile )
{
	m_dataFile.Clear();
	WX_APPEND_ARRAY( m_dataFile, dataFile );
	return *this;
}

wxString wxInputFile::ToString( bool bLongPath ) const
{
	wxString s;

	if ( m_inputFile.IsOk() )
	{
		s.Append( bLongPath ? m_inputFile.GetFullPath() : m_inputFile.GetFullName() );
		s.Append( SEPARATOR );
	}

	for ( size_t i = 0, nCount = m_dataFile.GetCount(); i < nCount; i++ )
	{
		s.Append( bLongPath ? m_dataFile[ i ].GetFullPath() : m_dataFile[ i ].GetFullName() );
		s.Append( SEPARATOR );
	}

	if ( s.Length() > 0 )
	{
		s.Truncate( s.Length() - 1 );
	}

	return s;
}

