/*
 * wxConfiguration.h
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

		DRAWING_MODE		 m_eDrawingMode;
		wxFileName			 m_inputFile;
		wxFileName			 m_outputFile;
		wxString			 m_sDefImageExt;
		DrawerSettings		 m_drawerSettings;
		wxImageResolution	 m_imageResolutionUnits;
		wxSize				 m_imageSize;
		wxSize				 m_imageResolution;
		wxUint16			 m_nImageQuality;
		int					 m_nImageColorDepth;
		bool				 m_bMultiChannel;
		wxUint16			 m_nColumnNumber;
		wxSize				 m_margins;
		bool				 m_bPowerMix;
		wxFileName			 m_cuePointsFile;
		bool				 m_bGenerateCuePoints;
		Interval			 m_interval;
		bool				 m_bUseMLang;
		bool				 m_bAnimation;
		AnimationSettings	 m_animationSettings;
		wxImageResizeQuality m_eResizeQuality;
		wxFileName			 m_ffmpegDir;
		wxFileName			 m_cmdTemplate;
		bool				 m_bDeleteTemporaryFiles;

	protected:

		bool ReadNegatableSwitchValue( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal );
		bool ReadNegatableSwitchValueAndNegate( const wxCmdLineParser& cmdLine, const wxString& name, bool& switchVal );
		bool ParseColourString( const wxString&, wxColour& );
		void GetDefaultsFromDisplay();
		static bool ConvertStringToDrawingMode( const wxString&, DRAWING_MODE& );

		static wxString GetDrawingModeAsText( DRAWING_MODE );
		static bool ConvertStringToCompositionMode( const wxString&, wxCompositionMode& );

		static bool ConvertStringToResizeQuality( const wxString&, wxImageResizeQuality& );

		static wxString GetResizeQualityAsText( wxImageResizeQuality );
		static wxString GetResizeQualityTexts();

		static wxString GetCompositionModeAsText( wxCompositionMode );
		static wxString GetDrawingModeTexts();
		static wxString GetCompositionModeTexts();
		static wxString GetSwitchAsText( bool );

		struct DRAWING_MODE_DESC
		{
			DRAWING_MODE drawingMode;
			const wxChar* description;
		};

		static const DRAWING_MODE_DESC DrawingModeDesc[];
		static const size_t			   DrawingModeDescSize;

		struct COMPOSITION_MODE_DESC
		{
			wxCompositionMode compositionMode;
			const wxChar* description;
		};

		static const COMPOSITION_MODE_DESC CompositionModeDesc[];
		static const size_t				   CompositionModeDescSize;

		struct RESIZE_QUALITY_DESC
		{
			wxImageResizeQuality resizeQuality;
			const wxChar* description;
		};

		static const RESIZE_QUALITY_DESC ResizeQualityDesc[];
		static const size_t				 ResizeQualityDescSize;

		static const wxChar CMD_TEMPLATE[];

	private:

		void add_margin( wxRect2DInt&, bool, bool, bool, bool ) const;

	public:

		const wxFileName& GetInputFile() const;
		wxFileName GetOutputFile() const;
		wxString GetDefaultImageExt() const;
		const DrawerSettings& GetDrawerSettings() const;
		const wxSize&		  GetImageSize() const;
		wxImageResolution GetImageResolutionUnits() const;
		const wxSize& GetImageResolution() const;
		wxUint16 GetImageQuality() const;
		wxUint16 GetPngCompressionLevel() const;
		int GetImageColorDepth() const;
		bool MultiChannel() const;
		wxUint16 GetNumberOfColumns() const;
		wxSize GetMargins() const;
		wxRect2DInt GetDrawerRect() const;

		void GetDrawerRects( wxUint16, wxRect2DIntArray & ) const;
		bool PowerMix() const;
		DRAWING_MODE GetDrawingMode() const;
		wxString GetDrawingModeAsText() const;
		bool HasCuePointsFile() const;
		const wxFileName& GetCuePointsFile() const;
		bool ReadCuePoints( wxTimeSpanArray& ) const;
		bool GenerateCuePoints() const;
		bool GenerateCuePoints( const wxTimeSpan&, wxTimeSpanArray& ) const;
		bool UseMLang() const;
		wxString GetCompositionModeAsText() const;
		bool CreateAnimation() const;
		const AnimationSettings& GetAnimationSettings() const;
		wxImageResizeQuality GetResizeQuality() const;
		const wxFileName& GetFfmpegDir() const;
		wxFileName GetGetCommandTemplateFile() const;
		bool DeleteTemporaryFiles() const;

	public:

		wxConfiguration( void );

		void AddCmdLineParams( wxCmdLineParser& ) const;
		bool Read( const wxCmdLineParser& );

		static const wxSystemColour COLOR_BACKGROUND;
		static const wxSystemColour COLOR_BACKGROUND2;
};

#endif	// _WX_CONFIGURATION_H

