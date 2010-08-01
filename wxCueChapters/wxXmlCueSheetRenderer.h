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

	const wxConfiguration& m_cfg;
	wxInputFile m_inputFile;
	wxString m_sOutputFile;

protected:

	virtual bool OnPreRenderDisc( const wxCueSheet& );
	virtual bool OnPreRenderTrack( const wxTrack& );
	virtual bool OnPostRenderTrack( const wxTrack& );
	virtual bool OnPostRenderDisc( const wxCueSheet& );

	virtual bool OnRenderPreGap( const wxTrack&, const wxIndex& );
	virtual bool OnRenderPostGap( const wxTrack&, const wxIndex& );
	virtual bool OnRenderIndex( const wxTrack&, const wxIndex& );

public:

	wxXmlCueSheetRenderer(const wxConfiguration&, const wxInputFile&, const wxString& );
	virtual ~wxXmlCueSheetRenderer(void);

	wxXmlDocument* GetXmlDoc() const;
	bool SaveXmlDoc();
};

#endif
