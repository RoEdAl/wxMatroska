/*
	wxSamplingInfo.h
*/

#ifndef _WX_SAMPLING_INFO_H_
#define _WX_SAMPLING_INFO_H_

#ifndef _WX_INDEX_H_
#include "wxIndex.h"
#endif

class wxSamplingInfo :public wxObject
{
	wxDECLARE_DYNAMIC_CLASS(wxSamplingInfo);

public:

	static const wxTimeSpan wxInvalidDuration;
	static const wxULongLong wxInvalidNumberOfFrames;

	wxSamplingInfo(void);
	wxSamplingInfo(const wxSamplingInfo&);
	wxSamplingInfo(unsigned long, unsigned short, unsigned short);
	wxSamplingInfo( const FLAC::Metadata::StreamInfo& );

	bool IsOK(bool = false) const;

	wxSamplingInfo& operator=( const wxSamplingInfo& );

	unsigned long GetSamplingRate() const;
	unsigned short GetNumberOfChannels() const;
	unsigned short GetBitsPerSample() const;

	wxSamplingInfo& SetSamplingRate( unsigned long );
	wxSamplingInfo& SetNumberOfChannels( unsigned short );
	wxSamplingInfo& SetBitsPerSample( unsigned short );

	wxSamplingInfo& Assign( unsigned long, unsigned short, unsigned short );
	wxSamplingInfo& Assign( const FLAC::Metadata::StreamInfo& );
	wxSamplingInfo& SetDefault();

	wxULongLong GetNumberOfFramesFromBytes( const wxULongLong& ) const;
	wxTimeSpan GetDuration( wxULongLong );

	void GetNumberOfCdFrames( wxULongLong, wxULongLong&, wxUint32& ) const;
	wxULongLong GetNumberOfCdFrames( wxULongLong ) const;

	wxString GetSamplesStr( wxULongLong ) const;
	wxString GetCdFramesStr( wxULongLong ) const;

	wxULongLong GetFramesFromCdFrames( wxULongLong ) const;
	wxULongLong GetIndexOffset( const wxIndex& ) const;
	wxIndex ConvertIndex( const wxIndex& ) const;
	wxIndex ConvertIndex( const wxIndex&, wxULongLong, bool ) const;
	wxString GetIndexOffsetStr( const wxIndex& ) const;
	wxString GetIndexOffsetFramesStr( const wxIndex& ) const;

	wxString ToString() const;

protected:

	unsigned long m_nSamplingRate;
	unsigned short m_nNumChannels;
	unsigned short m_nBitsPerSample;

protected:

	void copy( const wxSamplingInfo& );

};

#endif
