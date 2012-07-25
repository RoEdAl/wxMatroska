/*
	CuePointsReader.h
*/

#ifndef _CUE_POINTS_READER_H_
#define _CUE_POINTS_READER_H_

class CuePointsReader
{
	public:

	CuePointsReader(void);

	bool Read( wxTimeSpanArray&, const wxFileName&, bool );

	protected:

	wxRegEx m_reMsf;
	wxRegEx m_reMsms;

	protected:

	bool ParseCuePointPosition( const wxString&, wxTimeSpan& );

};

#endif