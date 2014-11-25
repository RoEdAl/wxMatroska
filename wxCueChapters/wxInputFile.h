/*
 * wxInputFile.h
 */

#ifndef _WX_INPUT_FILE_H_
#define _WX_INPUT_FILE_H_

#ifndef _WX_DATA_FILE_H_
#include <wxCueFile/wxDataFile.h>
#endif

class wxInputFile:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxInputFile );

	protected:

		wxFileName		m_inputFile;
		wxArrayFileName m_dataFile;

	protected:

		void copy( const wxInputFile& );

	public:

		static const char SEPARATOR;

		wxInputFile( void );
		wxInputFile( const wxInputFile& );
		wxInputFile( const wxString& );
		wxInputFile& operator =( const wxInputFile& );

		bool IsOk() const;
		const wxFileName&	   GetInputFile() const;
		const wxArrayFileName& GetDataFiles() const;
		void GetDataFiles( wxArrayDataFile&, wxDataFile::FileType = wxDataFile::WAVE ) const;
		bool HasDataFiles() const;

		wxInputFile& SetInputFile( const wxFileName& );
		wxInputFile& SetDataFiles( const wxArrayFileName& );

		wxString ToString( bool = false ) const;
};

#endif

