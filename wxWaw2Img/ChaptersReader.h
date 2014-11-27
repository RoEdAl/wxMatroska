/*
 *      ChaptersReader.h
 */

#ifndef _CHAPTERS_READER_H_
#define _CHAPTERS_READER_H_

class ChaptersReader
{
	public:

		ChaptersReader( void );

		bool Read( ChaptersArray&, const wxFileName&, bool );

	protected:

		wxRegEx m_reMsf;
		wxRegEx m_reMsms;
		wxRegEx m_reHmsms;

	protected:

		bool ParseChapterPosition( const wxString&, ChapterDesc& );
};
#endif

