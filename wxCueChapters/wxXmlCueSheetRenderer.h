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

class wxXmlCueSheetRenderer :public wxCueSheetRenderer
{
	DECLARE_CLASS(wxXmlCueSheetRenderer)

protected:

	wxXmlDocument* m_pXmlDoc;
	wxXmlNode* m_pChapterAtom;
	wxXmlNode* m_pPrevChapterAtom;
	wxXmlNode* m_pEditionEntry;

	wxXmlDocument* m_pXmlTags;
	wxXmlNode* m_pTags;

	const wxConfiguration& m_cfg;
	wxInputFile m_inputFile;
	wxString m_sOutputFile;
	wxString m_sTagsFile;

	wxRegEx m_reCommentMeta;

protected:

	virtual bool OnPreRenderDisc( const wxCueSheet& );
	virtual bool OnPreRenderTrack( const wxTrack& );
	virtual bool OnRenderTrack( const wxTrack& );
	virtual bool OnPostRenderTrack( const wxTrack& );
	virtual bool OnPostRenderDisc( const wxCueSheet& );

	virtual bool OnRenderPreGap( const wxTrack&, const wxIndex& );
	virtual bool OnRenderPostGap( const wxTrack&, const wxIndex& );
	virtual bool OnRenderIndex( const wxTrack&, const wxIndex& );

protected:

	static wxULongLong GenerateUID();
	void AddCdTextInfo( const wxCueComponent&, wxXmlNode* );
	wxXmlNode* AddDiscTags(
		const wxCueSheet&,
		wxXmlNode*,
		const wxULongLong&,
		int = 50 );
	wxXmlNode* AddTrackTags(
		const wxTrack&,
		const wxULongLong&,
		wxXmlNode*,
		int = 30 );

public:

	wxXmlCueSheetRenderer(const wxConfiguration&, const wxInputFile& );
	virtual ~wxXmlCueSheetRenderer(void);

	wxXmlDocument* GetXmlDoc() const;
	wxXmlDocument* GetXmlTags() const;

	const wxString& GetOutputFile() const;
	const wxString& GetTagsFile() const;

	bool SaveXmlDoc();
};

#endif
