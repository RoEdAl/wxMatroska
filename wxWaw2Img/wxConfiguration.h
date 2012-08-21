/*
 * wxConfiguration.h
 */

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _MY_CONFIGURATION_H_
#include <wxConsoleApp/MyConfiguration.h>
#endif

enum DRAWING_MODE
{
	DRAWING_MODE_SIMPLE,
	DRAWING_MODE_RASTER1,
	DRAWING_MODE_RASTER2,
	DRAWING_MODE_POLY,
	DRAWING_MODE_AUDIO
};

class wxConfiguration:
	public MyConfiguration
{
	public:

		enum INFO_SUBJECT
		{
			INFO_NONE,
			INFO_VERSION,
			INFO_COLOUR_FORMAT,
			INFO_CUE_POINT_FORMAT,
			INFO_CMD_LINE_TEMPLATE,
			INFO_SYSTEM_SETTINGS,
			INFO_LICENSE
		};

	protected:

		INFO_SUBJECT		 m_infoSubject;
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
		bool m_bRunFfmpeg;
		wxFileName			 m_ffmpegDir;
		wxFileName			 m_cmdTemplate;
		bool				 m_bDeleteTemporaryFiles;
		bool				m_bUseWorkerThreads;

	protected:

		bool ParseColourString( const wxString&, wxColour& );
		void GetDefaultsFromDisplay();

		static bool FromString( const wxString&, DRAWING_MODE& );
		static wxString ToString( DRAWING_MODE );

		static bool FromString( const wxString&, wxCompositionMode& );
		static wxString ToString( wxCompositionMode );

		static bool FromString( const wxString&, wxImageResizeQuality& );
		static wxString ToString( wxImageResizeQuality );

		static bool FromString( const wxString&, INFO_SUBJECT& );
		static wxString ToString( INFO_SUBJECT );

		static wxString GetResizeQualityTexts();
		static wxString GetDrawingModeTexts();
		static wxString GetCompositionModeTexts();
		static wxString GetInfoSubjectTexts();

		struct DRAWING_MODE_DESC
		{
			DRAWING_MODE value;
			const wxChar* description;
		};

		static const DRAWING_MODE_DESC DrawingModeDesc[];

		struct COMPOSITION_MODE_DESC
		{
			wxCompositionMode value;
			const wxChar* description;
		};

		static const COMPOSITION_MODE_DESC CompositionModeDesc[];

		struct RESIZE_QUALITY_DESC
		{
			wxImageResizeQuality value;
			const wxChar* description;
		};

		static const RESIZE_QUALITY_DESC ResizeQualityDesc[];

		static const wxChar CMD_TEMPLATE[];

		struct INFO_SUBJECT_DESC
		{
			INFO_SUBJECT value;
			const wxChar* description;
		};

		static const INFO_SUBJECT_DESC InfoSubjectDesc[];

	private:

		void add_margin( wxRect2DInt&, bool, bool, bool, bool ) const;

	public:

		INFO_SUBJECT GetInfoSubject() const;
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
		bool RunFfmpeg() const;
		const wxFileName& GetFfmpegDir() const;
		wxFileName GetGetCommandTemplateFile() const;
		bool DeleteTemporaryFiles() const;
		bool UseWorkerThreads() const;

	public:

		wxConfiguration( void );

		void AddCmdLineParams( wxCmdLineParser& ) const;
		bool Read( const wxCmdLineParser& );

		static const wxSystemColour COLOR_BACKGROUND;
		static const wxSystemColour COLOR_BACKGROUND2;
};

#endif	// _WX_CONFIGURATION_H

