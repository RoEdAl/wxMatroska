/*
   wxConfiguration.h
 */

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

enum DRAWING_MODE
{
	DRAWING_MODE_SIMPLE,
	DRAWING_MODE_RASTER1,
	DRAWING_MODE_RASTER2,
	DRAWING_MODE_POLY,
	DRAWING_MODE_AUDIO
};

class wxConfiguration:
	public wxObject
{
	wxDECLARE_DYNAMIC_CLASS( wxConfiguration );

protected:

	DRAWING_MODE m_eDrawingMode;

	wxFileName m_inputFile;
	wxFileName m_outputFile;

	wxUint32 m_nWidth;
	wxUint32 m_nHeight;

	wxColour m_clrFrom;
	wxColour m_clrTo;
	bool	 m_bDrawWithGradient;

	wxColour m_clrBackground;

	wxImageResolution m_imageResolutionUnits;
	wxSize			  m_imageResolution;
	wxUint16		  m_nImageQuality;

	int m_nImageColorDepth;

	bool	  m_bLogarithmicScale;
	bool	  m_bLogarithmicColorGradient;
	wxFloat32 m_fLogBase;

	bool	 m_bMultiChannel;
	wxUint16 m_nColumnNumber;
	wxSize	 m_margins;

	bool m_bPowerMix;

	wxUint16 m_nFrequency;

protected:

	bool ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal );
	bool ReadNegatableSwitchValueAndNegate( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal );
	bool ParseColourString( const wxString&, wxColour& );
	void GetDefaultsFromDisplay();
	static bool ConvertStringToDrawingMode( const wxString&, DRAWING_MODE& );

	static wxString GetDrawingModeAsText( DRAWING_MODE );
	static wxString GetDrawingModeTexts();
	static wxString GetSwitchAsText( bool );

	struct DRAWING_MODE_DESC
	{
		DRAWING_MODE drawingMode;
		const wxChar* description;
	};

	static const DRAWING_MODE_DESC DrawingModeDesc[];
	static const size_t			   DrawingModeDescSize;

private:

	void add_margin( wxRect2DInt& ) const;

public:

	const wxFileName& GetInputFile() const;
	wxFileName GetOutputFile() const;

	wxUint32 GetWidth() const;
	wxUint32 GetHeight() const;

	const wxColour& GetColourFrom() const;
	const wxColour& GetColourTo() const;
	bool DrawWithGradient() const;

	const wxColour& GetBackgroundColor() const;

	wxImageResolution GetImageResolutionUnits() const;
	const wxSize& GetImageResolution() const;
	wxUint16 GetImageQuality() const;

	int GetImageColorDepth() const;

	bool UseLogarithmicScale() const;
	bool UseLogarithmicColorPalette() const;
	wxFloat32 GetLogarithmBase() const;

	bool MultiChannel() const;
	wxUint16 GetNumberOfColumns() const;
	wxSize GetMargins() const;

	wxRect2DInt GetDrawerRect() const;

	wxRect2DInt GetDrawerRect( wxUint16, wxUint16 ) const;

	bool PowerMix() const;

	DRAWING_MODE GetDrawingMode() const;
	wxUint16 GetFrequency() const;

public:

	wxConfiguration( void );

	void AddCmdLineParams( wxCmdLineParser& );
	bool Read( const wxCmdLineParser& );
};

#endif // _WX_CONFIGURATION_H

