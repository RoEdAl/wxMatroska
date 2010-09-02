/*
	wxXmlCueSheetRenderer.h
*/

#ifndef _WX_XML_CUE_SHEET_RENDERER_H_
#define _WX_XML_CUE_SHEET_RENDERER_H_

#ifndef _WX_CUE_SHEET_RENDERER_H_
#include <wxCueSheetRenderer.h>
#endif

#ifndef _WX_INPUT_FILE_H_
class wxInputFile;
#endif

#ifndef _WX_CONFIGURATION_H_
class wxConfiguration;
#endif

#ifndef _WX_UNQUOTER_H_
#include "wxUnquoter.h"
#endif

class wxXmlCueSheetRenderer :public wxCueSheetRenderer
{
	DECLARE_CLASS(wxXmlCueSheetRenderer)

protected:

	wxXmlDocument* m_pXmlDoc;
	wxXmlNode* m_pChapterAtom;
	wxXmlNode* m_pFirstChapterAtom;
	wxXmlNode* m_pPrevChapterAtom;
	wxXmlNode* m_pEditionEntry;

	wxXmlDocument* m_pXmlTags;
	wxXmlNode* m_pTags;

	const wxConfiguration& m_cfg;
	wxInputFile m_inputFile;
	wxString m_sOutputFile;
	wxString m_sTagsFile;

	wxULongLong m_offset;
	size_t m_nTotalParts;
	wxSamplingInfo m_si;

protected:

	virtual bool OnPreRenderDisc( const wxCueSheet& );
	virtual bool OnPreRenderTrack( const wxCueSheet&, const wxTrack& );
	virtual bool OnRenderTrack( const wxCueSheet&, const wxTrack& );
	virtual bool OnPostRenderTrack( const wxCueSheet&, const wxTrack& );
	virtual bool OnPostRenderDisc( const wxCueSheet& );

	virtual bool OnRenderPreGap( const wxCueSheet&, const wxTrack&, const wxIndex& );
	virtual bool OnRenderPostGap( const wxCueSheet&, const wxTrack&, const wxIndex& );
	virtual bool OnRenderIndex( const wxCueSheet&, const wxTrack&, const wxIndex& );

protected:

	static wxULongLong GenerateUID();
	void AddCdTextInfo( const wxCueComponent&, wxXmlNode* );
	wxXmlNode* AddDiscTags(
		const wxCueSheet&,
		wxXmlNode*,
		const wxULongLong&,
		int = 50 );
	wxXmlNode* AppendDiscTags(
		const wxCueSheet&,
		wxXmlNode*,
		long = 50 );
	wxXmlNode* SetTotalParts(
		wxXmlNode*,
		long = 50 );
	wxXmlNode* AddTrackTags(
		const wxTrack&,
		const wxULongLong&,
		wxXmlNode*,
		int = 30 );

	wxXmlNode* AddChapterTimeStart( wxXmlNode*, const wxIndex& ) const;

	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxTrack& ) const;
	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxIndex& ) const;
	wxXmlNode* AddChapterTimeEnd( wxXmlNode*, const wxULongLong& ) const;

	wxXmlNode* AddIdxChapterAtom( wxXmlNode*, const wxIndex& ) const;

public:

	static wxXmlCueSheetRenderer* const Null;

	wxXmlCueSheetRenderer(const wxConfiguration&, const wxInputFile& );
	virtual ~wxXmlCueSheetRenderer(void);
	void SetInputFile( const wxInputFile& );

	wxXmlDocument* GetXmlDoc() const;
	wxXmlDocument* GetXmlTags() const;

	const wxString& GetOutputFile() const;
	const wxString& GetTagsFile() const;

	bool SaveXmlDoc();
	bool IsOffsetValid() const;
};

#endif
