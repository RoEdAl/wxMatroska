/*
	wxDataFile.h
*/

#ifndef _WX_DATA_FILE_H_
#define _WX_DATA_FILE_H_

#ifndef _WX_SAMPLING_INFO_H_
class wxSamplingInfo;
#endif

class wxDataFile :public wxObject
{
	wxDECLARE_DYNAMIC_CLASS(wxDataFile);

public:

	typedef enum _FileType
    {
        BINARY, MOTOROLA, AIFF, WAVE, MP3
    } FileType;

protected:

	typedef struct _FILE_TYPE_STR
	{
		FileType ftype;
		const wxChar* szName;
	} FILE_TYPE_STR;

	static FILE_TYPE_STR FileTypeString[];
	static size_t FileTypeStringSize;

protected:

	wxFileName m_fileName;
	FileType m_ftype;
	wxUint64 m_nNumberOfSamples;

protected:

	void copy(const wxDataFile&);

	static wxULongLong GetNumberOfFramesFromBinary( const wxFileName&, const wxSamplingInfo& si );
	static bool GetFromMediaInfo( const wxFileName&, wxULongLong&, wxSamplingInfo& );

public:

	wxString GetFileName() const;
	wxString GetFullPath() const;

	FileType GetFileType() const;
	wxString GetFileTypeAsString() const;

	bool HasNumberOfSamples() const;
	wxUint64 GetNumberOfSamples() const;

	bool IsEmpty() const;
	wxDataFile& Assign( const wxString&, FileType = BINARY);
	wxDataFile& Assign( const wxFileName&, FileType = BINARY);

public:

	wxDataFile(void);
	wxDataFile(const wxDataFile&);
	wxDataFile(const wxString&, FileType = BINARY);
	wxDataFile(const wxFileName&, FileType = BINARY);

	wxDataFile& operator=(const wxDataFile&);
	void Clear();

	bool FindFile( wxFileName&, const wxString& = wxEmptyString ) const;
	bool FileExists( const wxString& = wxEmptyString ) const;

	bool GetInfo( wxSamplingInfo&, wxULongLong&, const wxString& = wxEmptyString ) const;
	bool CalculateNumberOfSamples( wxSamplingInfo&, const wxString& = wxEmptyString );

	wxTimeSpan GetDuration( const wxString& = wxEmptyString ) const; // wxInvalidDuration if duration cannot be calculated

	static wxString FileTypeToString( FileType );
	static bool StringToFileType( const wxString&, FileType& );
	static wxString GetFileTypeRegExp();
};

WX_DECLARE_OBJARRAY( wxDataFile, wxArrayDataFile );

#endif // _WX_DATA_FILE_H_
