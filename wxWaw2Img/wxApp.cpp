/*
 * wxApp.cpp
 */
#include "StdWx.h"
#include <sndfile.h>
#include <wxEncodingDetection/wxEncodingDetection.h>
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "Interval.h"
#include "DrawerSettings.h"
#include "AnimationSettings.h"
#include "wxConfiguration.h"
#include "wxApp.h"

#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerColumnPainter.h"
#include "WaveDrawerRaster1.h"
#include "WaveDrawerRaster2.h"
#include "WaveDrawerPoly.h"
#include "WaveDrawerSimple.h"
#include "AudioRenderer.h"

#include "MultiChannelWaveDrawer.h"
#include "MCChainWaveDrawer.h"
#include "ArrayWaveDrawer.h"
#include "MCGraphicsContextWaveDrawer.h"
#include "ChannelMixer.h"

#include "SoundFile.h"
#include "NinePatchBitmap.h"
#include "MemoryGraphicsContext.h"

// ===============================================================================

const wxChar wxMyApp::APP_NAME[]		= wxS( "wav2img" );
const wxChar wxMyApp::APP_VERSION[]		= wxS( "1.0" );

// ===============================================================================

const wxChar wxMyApp::CMD_FFMPEG[]				= wxS( "FFMPEG" );
const wxChar wxMyApp::CMD_AUDIO[]				= wxS( "AUDIO" );
const wxChar wxMyApp::CMD_INPUT[]				= wxS( "INPUT" );
const wxChar wxMyApp::CMD_INPUT_OVERLAY[]		= wxS( "INPUT_OVERLAY" );
const wxChar wxMyApp::CMD_INPUT_DURATION[]		= wxS( "INPUT_DURATION" );
const wxChar wxMyApp::CMD_INPUT_FRAMES[]		= wxS( "INPUT_FRAMES" );
const wxChar wxMyApp::CMD_INPUT_RATE[]			= wxS( "INPUT_RATE" );
const wxChar wxMyApp::CMD_OUTPUT[]				= wxS( "OUTPUT" );
const wxChar wxMyApp::CMD_KEY_FRAMES[]			= wxS( "KEY_FRAMES" );

const wxChar wxMyApp::BACKGROUND_IMG[]		= wxS( "background.png" );

// ===============================================================================

wxIMPLEMENT_APP_CONSOLE( wxMyApp );

wxMyApp::wxMyApp( void )
{}

static wxString get_libsndfile_version()
{
	wxString v;

	{
		wxStringTypeBufferLength< char > vl( v, 128 );
		int								 nRes = sf_command( NULL, SFC_GET_LIB_VERSION, vl, 128 );
		vl.SetLength( nRes );
	}

	return v;
}

static wxString get_format_name( int nFormat )
{
	SF_FORMAT_INFO fm_info;
	fm_info.format = nFormat;

	int nRes = sf_command( NULL, SFC_GET_FORMAT_INFO, &fm_info, sizeof(SF_FORMAT_INFO) );

	return wxString( fm_info.name );
}

void wxMyApp::InfoVersion( wxMessageOutput& out )
{
	out.Printf( _( "Application version: %s" ), APP_VERSION );
	wxString::Format( _( "Author: %s" ), APP_AUTHOR );
	out.Output( _( "License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php" ) );
	out.Printf( _( "wxWidgets version: %d.%d.%d. Copyright \u00A9 1992-2008 Julian Smart, Robert Roebling, Vadim Zeitlin and other members of the wxWidgets team" ), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER );
	out.Printf( _( "libsndfile version: %s. Copyright \u00A9 1999-2011 Erik de Castro Lopo" ), get_libsndfile_version() );
	out.Printf( _( "Operating system: %s" ), wxPlatformInfo::Get().GetOperatingSystemDescription() );
}

void wxMyApp::InfoColourFormat( wxMessageOutput& out )
{
	out.Output( _( "Color format specification:" ) );
	out.Output( _( "\tCSS sytntax: RGB(176,45,235)" ) );
	out.Output( _( "\tCSS syntax with alpha: RGBA(176,45,235,0.7)" ) );
	out.Output( _( "\tHTML syntax (no alpha): #AABBFF" ) );
	out.Output( _( "\tcolor name: yellow, transparent etc." ) );
}

void wxMyApp::InfoCuePointFormat( wxMessageOutput& out )
{
	out.Output( _( "Cue points format:" ) );
	out.Output( _( "\tMM::SS:FF - minutes, seconds and CD frames (1/75 s)" ) );
	out.Output( _( "\tMM::SS.YYY - minutes, seconds and miliseconds" ) );
	out.Output( _( "\ts[.www] - seconds [with optional partial part]" ) );
	out.Output( _( "Examples:" ) );
	out.Output( _( "\t05:01:65 # 5 minutes, one second and 65 CD frames" ) );
	out.Output( _( "\t15:23.456 # 15 minutes, 23 seconds and 456 miliseconds" ) );
	out.Output( _( "\t545 # 545 seconds" ) );
	out.Output( _( "\t10345.67 # 10345 seconds and 670 miliseconds" ) );
}

void wxMyApp::InfoCmdLineTemplate( wxMessageOutput& out )
{
	out.Output( _( "Command line template replacements:" ) );
	out.Printf( _( "\t$%s$: path to ffmpeg executable" ), CMD_FFMPEG );
	out.Printf( _( "\t$%s$: path to audio file" ), CMD_AUDIO );
	out.Printf( _( "\t$%s$: path to background image file or sequence of images" ), CMD_INPUT );
	out.Printf( _( "\t$%s$: path to overlay image file or sequence of images" ), CMD_INPUT_OVERLAY );
	out.Printf( _( "\t$%s$: input duration in seconds" ), CMD_INPUT_DURATION );
	out.Printf( _( "\t$%s$: number of input frames" ), CMD_INPUT_FRAMES );
	out.Printf( _( "\t$%s$: input stream framerate" ), CMD_INPUT_RATE );
	out.Printf( _( "\t$%s$: path to output file" ), CMD_OUTPUT );
	out.Printf( _( "\t$%s$: cue points list" ), CMD_KEY_FRAMES );
	out.Output( _( "Empty lines and lines beginning with # character are ignored." ) );
	out.Output( _( "All other lines are concatenated to one-liner command." ) );
}

void wxMyApp::InfoSystemSettings( wxMessageOutput& out )
{
	wxRect dplRect	 = wxGetClientDisplayRect();
	int	   nDepth	 = wxDisplayDepth();
	wxSize res		 = wxGetDisplayPPI();
	wxSize dplSizeMm = wxGetDisplaySizeMM();

	out.Printf( _( "Display size (pixels): %dx%d (%dx%d)" ), dplRect.width, dplRect.height, dplRect.x, dplRect.y );
	out.Printf( _( "Display color depth (bits): %d" ), nDepth );
	out.Printf( _( "Display resolution (pixels/inch): %dx%d" ), res.x, res.y );
	out.Printf( _( "Display size (milimeters): %dx%d" ), dplSizeMm.GetWidth(), dplSizeMm.GetHeight() );
	out.Printf( _( "Background color: %s" ), wxSystemSettings::GetColour( wxConfiguration::COLOR_BACKGROUND ).GetAsString() );
	out.Printf( _( "Second background color: %s" ), wxSystemSettings::GetColour( wxConfiguration::COLOR_BACKGROUND2 ).GetAsString() );
}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	MyAppConsole::OnInitCmdLine( cmdline );
	cmdline.SetLogo( _( "This application draws a waveform from audio file." ) );
	m_cfg.AddCmdLineParams( cmdline );
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& cmdline )
{
	if ( !MyAppConsole::OnCmdLineParsed( cmdline ) )
	{
		return false;
	}

	if ( !m_cfg.Read( cmdline ) )
	{
		return false;
	}

	wxLogInfo( _( "%s ver. %s" ), GetAppDisplayName(), APP_VERSION );
	return true;
}

bool wxMyApp::OnInit()
{
	SetAppName( APP_NAME );

	m_pColourDatabase.reset( new wxColourDatabase() );
	wxTheColourDatabase = m_pColourDatabase.get();
	InitImageHandlers();


	if ( !MyAppConsole::OnInit() )
	{
		return false;
	}

	return true;
}

void wxMyApp::InitImageHandlers()
{
	wxImage::AddHandler( new wxPNGHandler() );
	wxImage::AddHandler( new wxJPEGHandler() );
	wxImage::AddHandler( new wxTIFFHandler() );
}

static void read_audio_samples( SoundFile& soundFile, MultiChannelWaveDrawer& waveDrawer )
{
	SNDFILE* sndfile = soundFile.GetHandle();

	/*
	 * double d;
	 *
	 * if ( !sf_command( m_soundFile.GetHandle(), SFC_CALC_NORM_SIGNAL_MAX, &d, sizeof(d) ) )
	 * {
	 *      wxLogMessage( _("Normalized value: %f"), d );
	 * }
	 * else
	 * {
	 *      d = 1;
	 * }
	 */

	int		   nChannels   = soundFile.GetInfo().channels;
	int		   nSamplerate = soundFile.GetInfo().samplerate;
	sf_count_t nBlock	   = nChannels * nSamplerate;

	wxFloatArray block( new float[ nBlock ] );

	sf_count_t nCount = sf_readf_float( sndfile, block.get(), nSamplerate );

	while ( nCount > 0 )
	{
		waveDrawer.ProcessFames( block.get(), nCount );
		nCount = sf_readf_float( sndfile, block.get(), nSamplerate );
	}

	soundFile.Close();
}

static WaveDrawer* create_wave_drawer( DRAWING_MODE eMode, const wxConfiguration& cfg, wxUint64 nNumberOfSamples, wxGraphicsContext* gc, const wxRect2DInt& rc, bool bUseCuePoints, const wxTimeSpanArray& cuePoints )
{
	switch ( eMode )
	{
		case DRAWING_MODE_SIMPLE:
		{
			return new SimpleWaveDrawer( nNumberOfSamples, gc, rc, cfg.GetDrawerSettings(), bUseCuePoints, cuePoints );
		}

		case DRAWING_MODE_RASTER1:
		{
			return new Raster1WaveDrawer( nNumberOfSamples, gc, rc, cfg.GetDrawerSettings(), bUseCuePoints, cuePoints );
		}

		case DRAWING_MODE_RASTER2:
		{
			return new Raster2WaveDrawer( nNumberOfSamples, gc, rc, cfg.GetDrawerSettings(), bUseCuePoints, cuePoints );
		}

		case DRAWING_MODE_POLY:
		{
			return new PolyWaveDrawer( nNumberOfSamples, gc, rc, cfg.GetDrawerSettings(), bUseCuePoints, cuePoints );
		}
	}

	wxASSERT( false );
	return NULL;
}

static McChainWaveDrawer* create_wave_drawer( const wxConfiguration& cfg, const SF_INFO& sfInfo, bool bUseCuePoints, const wxTimeSpanArray& cuePoints )
{
	if ( sfInfo.frames <= 0 )
	{
		wxLogError( _( "Unknown length of audio source" ) );
		return NULL;
	}

	const DrawerSettings& drawerSettings = cfg.GetDrawerSettings();

	wxUint64 nSamples		= sfInfo.frames;
	wxUint16 nChannels		= sfInfo.channels;
	wxUint32 nSamplerate	= sfInfo.samplerate;
	wxUint32 nTrackDuration = nSamples / nSamplerate;

	if ( ( nSamples % nSamplerate ) != wxULL( 0 ) )	// round up
	{
		nTrackDuration += 1u;
	}

	MultiChannelWaveDrawer* pMcwd = NULL;

	switch ( cfg.GetDrawingMode() )
	{
		case DRAWING_MODE_AUDIO:
		{
			ArrayWaveDrawer* pAwd = new ArrayWaveDrawer( 1 );

			pAwd->AddDrawer( new AudioRenderer( nSamples, cfg.GetImageSize().GetWidth(), drawerSettings.UseLogarithmicScale(), drawerSettings.GetLogarithmBase(), nSamplerate ) );
			pMcwd = pAwd;
			break;
		}

		case DRAWING_MODE_SIMPLE:
		case DRAWING_MODE_RASTER1:
		case DRAWING_MODE_RASTER2:
		case DRAWING_MODE_POLY:
		{
			const DrawerSettings& drawerSettings = cfg.GetDrawerSettings();
			wxRect2DIntArray	  drawerRects;

			if ( cfg.MultiChannel() )
			{
				McGraphicalContextWaveDrawer* pGc = new McGraphicalContextWaveDrawer( nChannels );

				cfg.GetDrawerRects( nChannels, drawerRects );

				wxGraphicsContext* gc = pGc->Initialize(
						cfg.GetImageSize(),
						cfg.GetImageColorDepth(),
						drawerSettings.GetBackgroundColour(),
						drawerRects,
						nTrackDuration );

				if ( gc == NULL )
				{
					delete pGc;
					return NULL;
				}

				for ( wxUint16 nChannel = 0; nChannel < nChannels; nChannel++ )
				{
					pGc->AddDrawer( create_wave_drawer( cfg.GetDrawingMode(), cfg, nSamples, gc, drawerRects[ nChannel ], bUseCuePoints, cuePoints ) );
				}

				pMcwd = pGc;
			}
			else
			{
				McGraphicalContextWaveDrawer* pGc = new McGraphicalContextWaveDrawer( 1 );

				drawerRects.Add( cfg.GetDrawerRect() );

				wxGraphicsContext* gc = pGc->Initialize(
						cfg.GetImageSize(),
						cfg.GetImageColorDepth(),
						drawerSettings.GetBackgroundColour(),
						drawerRects,
						nTrackDuration );

				if ( gc == NULL )
				{
					delete pGc;
					return NULL;
				}

				pGc->AddDrawer( create_wave_drawer( cfg.GetDrawingMode(), cfg, nSamples, gc, drawerRects[ 0 ], bUseCuePoints, cuePoints ) );
				pMcwd = pGc;
			}

			break;
		}
	}

	if ( cfg.MultiChannel() )
	{
		McChainWaveDrawer* pDrawer = new McChainWaveDrawer( nChannels, pMcwd );
		return pDrawer;
	}
	else
	{
		ChannelMixer* pMixer = new ChannelMixer( nChannels, pMcwd, cfg.PowerMix() );
		return pMixer;
	}
}

static void set_image_options( wxImage& img, const wxConfiguration& cfg, const wxFileName& fn )
{
	img.SetOption( wxIMAGE_OPTION_RESOLUTIONX, cfg.GetImageResolution().GetWidth() );
	img.SetOption( wxIMAGE_OPTION_RESOLUTIONY, cfg.GetImageResolution().GetHeight() );
	img.SetOption( wxIMAGE_OPTION_RESOLUTIONUNIT, cfg.GetImageResolutionUnits() );
	img.SetOption( wxIMAGE_OPTION_QUALITY, cfg.GetImageQuality() );
	img.SetOption( wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, cfg.GetPngCompressionLevel() );
	img.SetOption( wxIMAGE_OPTION_FILENAME, fn.GetName() );
}

static bool save_image( const wxFileName& fn, const wxConfiguration& cfg, wxEnhMetaFile* pEmf )
{
	wxASSERT( pEmf != NULL );

	wxLogInfo( _( "Opening metafile" ) );
	const wxSize&	imgSize = cfg.GetImageSize();
	wxEnhMetaFileDC emfDc( fn.GetFullPath(), imgSize.GetWidth(), imgSize.GetHeight(), fn.GetName() );

	if ( !emfDc.IsOk() )
	{
		wxLogError( _( "Fail to create metafile \u201C%s\u201D" ), fn.GetFullName() );
		return false;
	}

	wxLogInfo( _( "Copying image" ) );

	if ( pEmf->Play( &emfDc ) )
	{
		wxScopedPtr< wxEnhMetaFile > pClonedEmf( emfDc.Close() );
		wxASSERT( pClonedEmf );
		wxLogInfo( _( "Imag saved to file \u201C%s\u201D" ), fn.GetFullName() );
		return true;
	}
	else
	{
		wxLogError( _( "Fail to save image to file \u201C%s\u201D" ), fn.GetFullName() );
		return false;
	}
}

static wxImage draw_progress( MemoryGraphicsContext& mgc, const NinePatchBitmap& npb, const wxRect2DIntArray& rects, wxUint32 nWidth, wxImageResizeQuality eResizeQuality )
{
	{
		wxScopedPtr< wxGraphicsContext > pGc( mgc.CreateGraphicsContext() );
		pGc->SetAntialiasMode( wxANTIALIAS_NONE );
		pGc->SetInterpolationQuality( wxINTERPOLATION_NONE );
		pGc->SetCompositionMode( wxCOMPOSITION_SOURCE );
		pGc->SetPen( wxNullPen );

		{
			const wxSize& imgSize = mgc.GetSize();
			wxGraphicsPath path = pGc->CreatePath();
			path.AddRectangle( 0, 0, imgSize.GetWidth(), imgSize.GetHeight() );

			pGc->SetBrush( wxColour( 0,0,0, wxALPHA_TRANSPARENT ) );
			pGc->FillPath( path );
		}

		pGc->SetCompositionMode( wxCOMPOSITION_OVER );
		pGc->SetBrush( wxNullBrush );

		for ( wxRect2DIntArray::const_iterator i = rects.begin(), end = rects.end(); i != end; ++i )
		{
			wxRect2DInt r( *i );
			r.m_width = nWidth;

			wxImage ri = npb.GetStretchedEx( r, eResizeQuality );

			if ( ri.IsOk() )
			{
				wxGraphicsBitmap bm = pGc->CreateBitmapFromImage( ri );
				pGc->DrawBitmap( bm, r.m_x, r.m_y, r.m_width, r.m_height );
			}
		}

		if ( nWidth == 0 )
		{ // fully transparent bitmap looses transparency during conversion to wxImage
			pGc->SetBrush( wxColour( 255,255,255,1 ) );
			wxGraphicsPath path = pGc->CreatePath();
			path.AddRectangle( 0, 0, 3, 3 );
			pGc->FillPath( path );
		}
	}

	return mgc.GetImage();
}

static wxString read_cmd_file( const wxFileName& cmdFile, bool bUseMLang )
{
	wxLogInfo( _( "Opening command template file \u201C%s\u201D" ), cmdFile.GetFullName() );

	wxString							   sCPDescription;
	wxEncodingDetection::wxMBConvSharedPtr pConv( wxEncodingDetection::GetFileEncoding( cmdFile.GetFullPath(), bUseMLang, sCPDescription ) );

	if ( pConv )
	{
		wxLogInfo( _( "Detected encoding of \u201C%s\u201D file is \u201C%s\u201D" ), cmdFile.GetFullName(), sCPDescription );
	}
	else
	{
		pConv = wxEncodingDetection::GetDefaultEncoding( bUseMLang, sCPDescription );
		wxLogInfo( _( "Using default file encoding \u201C%s\u201D" ), sCPDescription );
	}

	wxFileInputStream fis( cmdFile.GetFullPath() );

	if ( !fis.IsOk() )
	{
		wxLogError( _( "Cannot open commad template file \u201C%s\u201D" ), cmdFile.GetFullName() );
		return wxEmptyString;
	}

	wxTextInputStream tis( fis, wxT( '\t' ), *pConv );
	wxString		  sCmdLine;
	while ( !tis.GetInputStream().Eof() )
	{
		wxString s( tis.ReadLine() );
		s.Trim( false ).Trim( true );

		if ( s.IsEmpty() ) { continue; }

		if ( s.StartsWith( "#" ) ) { continue; }

		sCmdLine += s;
		sCmdLine += " ";
	}

	if ( !sCmdLine.IsEmpty() )
	{
		sCmdLine.RemoveLast( 1 );
	}

	return sCmdLine;
}

static inline wxString quote_str( const wxString& s )
{
	if ( s.Contains( " " ) )
	{
		return wxString::Format( "\"%s\"", s );
	}
	else
	{
		return s;
	}
}

static inline size_t replace_str( wxString& s, const wxChar* pszReplacement, const wxString& sValue )
{
	return s.Replace( 
		wxString::Format( "$%s$", pszReplacement ),
		quote_str( sValue ) );
}

static wxString get_key_frames( const wxTimeSpanArray& cuePoints )
{
	wxASSERT( cuePoints.Count() > 1u );

	wxString s;
	for( wxTimeSpanArray::const_iterator i = cuePoints.begin(), end = --cuePoints.end(); i != end; i++ )
	{
		s += i->Format( "%S.%l" );
		s += ',';
	}

	return s.RemoveLast();
}

static bool run_ffmpeg(
	const wxFileName& workDir,
	const wxConfiguration& cfg,
	wxUint32 nNumberOfPictures,
	wxUint32 nTrackDurationSec,
	bool bUseCuePoints, const wxTimeSpanArray& cuePoints
)
{
	wxFileName fn( cfg.GetGetCommandTemplateFile() );

	if ( !fn.IsFileReadable() )
	{
		wxLogError( _( "Template file \u201C%s\u201D is not readable" ), fn.GetFullName() );
		return false;
	}

	wxString sCmdLine( read_cmd_file( fn, cfg.UseMLang() ) );

	if ( sCmdLine.IsEmpty() )
	{
		return false;
	}

	wxString sFfmpeg( "ffmpeg" );

	if ( cfg.GetFfmpegDir().IsOk() )
	{
		wxFileName ffmpeg( cfg.GetFfmpegDir().GetFullPath(), "ffmpeg" );
		sFfmpeg = ffmpeg.GetFullPath();
	}

	wxFileName fnOut( cfg.GetOutputFile() );
	if ( fnOut.IsRelative() )
	{
		if ( !fnOut.MakeAbsolute() )
		{
			wxLogWarning( _( "Cannot make path \u201C%s\u201D absolute" ), fn.GetFullPath() );
		}
	}

	replace_str( sCmdLine, wxMyApp::CMD_FFMPEG, sFfmpeg );
	replace_str( sCmdLine, wxMyApp::CMD_AUDIO, cfg.GetInputFile().GetFullPath() );
	replace_str( sCmdLine, wxMyApp::CMD_INPUT, wxString::Format( "%s%s", workDir.GetFullPath(), wxMyApp::BACKGROUND_IMG ) );
	replace_str( sCmdLine, wxMyApp::CMD_INPUT_OVERLAY, wxString::Format( "%sseq%%05d.%s", workDir.GetFullPath(), cfg.GetDefaultImageExt() ) );
	replace_str( sCmdLine, wxMyApp::CMD_INPUT_DURATION, wxString::Format( "%u", nTrackDurationSec ) );
	replace_str( sCmdLine, wxMyApp::CMD_INPUT_FRAMES, wxString::Format( "%u", nNumberOfPictures ) );
	replace_str( sCmdLine, wxMyApp::CMD_INPUT_RATE, wxString::Format( "%u/%u", nNumberOfPictures, nTrackDurationSec ) );
	replace_str( sCmdLine, wxMyApp::CMD_OUTPUT, fnOut.GetFullPath() );
	if ( bUseCuePoints )
	{
		replace_str( sCmdLine, wxMyApp::CMD_KEY_FRAMES, get_key_frames( cuePoints ) );
	}
	else
	{
		replace_str( sCmdLine, wxMyApp::CMD_KEY_FRAMES, wxEmptyString );
	}

	wxLogMessage( _( "Executing: %s" ), sCmdLine );

	wxExecuteEnv env;
	env.env[ "AV_LOG_FORCE_NOCOLOR" ] = "1";

	long nRes = wxExecute( sCmdLine, wxEXEC_SYNC | wxEXEC_NOEVENTS, (wxProcess*)NULL, &env );

	if ( nRes == -1 )
	{
		wxLogError( _( "Fail to execute ffmpeg tool" ) );
		return false;
	}
	else
	{
		if ( nRes <= 1 )
		{
			wxLogInfo( _( "ffmpeg exit code: %d" ), nRes );
			return true;
		}
		else
		{
			wxLogError( _( "ffmpeg exit code: %d" ), nRes );
			return false;
		}
	}
}

class AnimationThread :public wxThread
{
	public:

	AnimationThread(
		int nThreadNumber,
		volatile wxUint32& nWidth,
		wxUint32 nMaxWidth,
		wxAtomicInt& nErrorCounter,
		wxCriticalSection& critSect,
		const wxSize& imgSize,
		const NinePatchBitmap& npb,
		const wxRect2DIntArray& rects,
		wxImageResizeQuality eResizeQuality,
		const wxFileName& workDir,
		const wxConfiguration& cfg
	)
		:wxThread( wxTHREAD_JOINABLE ),
		m_nThreadNumber( nThreadNumber ),
		m_nWidth( nWidth ),
		m_nMaxWidth( nMaxWidth ),
		m_nErrorCounter( nErrorCounter ),
		m_critSect( critSect ),
		m_imgSize( imgSize ),
		m_mgc( imgSize, 32, true ),
		m_npb( npb ),
		m_rects( rects ),
		m_eResizeQuality( eResizeQuality ),
		m_workDir( workDir ),
		m_cfg( cfg )
	{}

	protected:

    virtual ExitCode Entry()
	{
		wxLogStderr log;
		wxLog::SetThreadActiveTarget( &log );

		wxString sExt( m_cfg.GetDefaultImageExt() );

		wxFileName fn( m_workDir );
		fn.SetExt( sExt );

		wxUint32 nWidth;
		int nSeqCounter = 0;
		while( GetNextWidth( nWidth ) )
		{
			if ( TestDestroy() )
			{
				return 0;
			}


			fn.SetName( wxString::Format( "seq%05u", nWidth ) );

			wxLogInfo( _( "[thread%d] Creating sequence file \u201C%s\u201D" ), m_nThreadNumber, fn.GetFullName() );
			wxImage img( DrawProgress( nWidth ) );

			set_image_options( img, m_cfg, fn );
			wxMemoryOutputStream mos;

			if ( img.SaveFile( mos, wxBITMAP_TYPE_PNG ) )
			{
				wxStreamBuffer* sb = mos.GetOutputStreamBuffer();
				wxFileOutputStream fos( fn.GetFullPath() );

				if ( fos.IsOk() )
				{
					fos.Write( sb->GetBufferStart(), sb->GetBufferSize() - sb->GetBytesLeft() );
					fos.Close();
				}
				else
				{
					wxLogError( _( "[thread%d] Fail to save sequence %u to file \u201C%s\u201D" ), m_nThreadNumber, nWidth, fn.GetFullName() );
					wxAtomicInc( m_nErrorCounter );
					break;
				}
			}
			else
			{
				wxLogError( _( "[thread%d] Fail to create sequence %u - cannot convert to PNG format" ), m_nThreadNumber, nWidth );
				wxAtomicInc( m_nErrorCounter );
				break;
			}

			nSeqCounter++;
		}

		wxLogInfo( _("[thread%d] %d sequences created"), m_nThreadNumber, nSeqCounter );
		return (ExitCode)nSeqCounter;
	}

	protected:

	bool GetNextWidth( wxUint32& nWidth )
	{
		wxCriticalSectionLocker locker(m_critSect);
		nWidth = m_nWidth++;
		return (nWidth < m_nMaxWidth);
	}

	inline wxImage DrawProgress( wxUint32 nWidth )
	{
		return draw_progress( m_mgc, m_npb, m_rects, nWidth, m_eResizeQuality );
	}

	protected:

	int m_nThreadNumber;
	volatile wxUint32& m_nWidth;
	wxUint32 m_nMaxWidth;
	wxAtomicInt& m_nErrorCounter;
	wxCriticalSection& m_critSect;

	wxSize m_imgSize;
	MemoryGraphicsContext m_mgc;
	NinePatchBitmap m_npb;
	wxRect2DIntArray m_rects;
	wxImageResizeQuality m_eResizeQuality;
	wxFileName m_workDir;
	const wxConfiguration& m_cfg;
};

static bool create_animation(
	const wxFileName& workDir,
	const wxConfiguration& cfg,
	const wxImage& img,
	const NinePatchBitmap& npb,
	const wxRect2DIntArray& rects,
	wxUint32 nTrackDuration,
	bool bUseCuePoints, const wxTimeSpanArray& cuePoints
)
{
	wxASSERT( rects.GetCount() > 0 );

	wxString sExt( cfg.GetDefaultImageExt() );

	wxFileName fn( workDir );
	fn.SetFullName( wxMyApp::BACKGROUND_IMG );

	{
		wxImage bimg( img );
		set_image_options( bimg, cfg, fn );

		if ( !bimg.SaveFile( fn.GetFullPath() ) )
		{
			wxLogError( _( "Fail to save background to file \u201C%s\u201D" ), fn.GetFullName() );
			return false;
		}
	}

	wxUint32 nMaxWidth = rects[ 0 ].GetSize().GetWidth();

	int nCpuCount = -1;
	if ( cfg.UseWorkerThreads() )
	{
		nCpuCount = wxThread::GetCPUCount();
	}

	if ( nCpuCount > 1 )
	{ // many threads
		volatile wxUint32 nWidth = 0u;
		wxAtomicInt nErrorCounter = 0;
		wxCriticalSection critSect;

		wxThread** ta = new wxThread*[ nCpuCount ];
		for( int i = 0; i < nCpuCount; i++ )
		{
			ta[i] = NULL;
		}

		for( int i = 0; i < nCpuCount; i++ )
		{
			wxLogInfo( _("Creating working thread %i"), i );
			ta[i] = new AnimationThread( 
				i,
				nWidth, nMaxWidth,
				nErrorCounter, critSect,
				img.GetSize(), npb, rects,
				cfg.GetResizeQuality(),
				workDir,
				cfg );

			wxThreadError e = ta[i]->Create();
			if ( e != wxTHREAD_NO_ERROR )
			{
				wxLogError( _("Fail to create thread %i - error %i"), i, (int)e );
				nErrorCounter++;
				break;
			}
		}

		if ( nErrorCounter == 0 )
		{
			wxLogInfo( _("Starting working threads") );

			for( int i = 0; i < nCpuCount; i++ )
			{
				ta[i]->Resume();
			}

			for( int i=0; i < nCpuCount; i++ )
			{
				ta[i]->Wait();
			}

			wxLogInfo( _("All working threads finished") );
		}

		for( int i = 0; i < nCpuCount; i++ )
		{
			wxDELETE( ta[i] );
		}

		wxDELETEA( ta );

		if ( nErrorCounter > 0 )
		{
			return false;
		}
	}
	else
	{ // single thread, this thread
		wxSize imgSize( img.GetSize() );

		MemoryGraphicsContext mgc( imgSize, 32, true );

		for ( wxUint32 i = 0; i < nMaxWidth; i++ )
		{
			fn.SetName( wxString::Format( "seq%05u", i ) );

			wxLogInfo( _( "Creating sequence file \u201C%s\u201D" ), fn.GetFullName() );
			wxImage aimg( draw_progress( mgc, npb, rects, i, cfg.GetResizeQuality() ) );

			set_image_options( aimg, cfg, fn );

			if ( !aimg.SaveFile( fn.GetFullPath() ) )
			{
				wxLogError( _( "Fail to save sequence %u to file \u201C%s\u201D" ), i, fn.GetFullName() );
				return false;
			}
		}
	}

	if ( cfg.RunFfmpeg() )
	{
		return run_ffmpeg( workDir, cfg, nMaxWidth, nTrackDuration, bUseCuePoints, cuePoints );
	}
	else
	{
		return true;
	}
}

static bool save_image(
	const wxFileName& fn,
	const wxConfiguration& cfg,
	const McGraphicalContextWaveDrawer& mcWaveDrawer,
	bool bUseCuePoints, const wxTimeSpanArray& cuePoints
)
{
	if ( cfg.CreateAnimation() )
	{
		wxLogMessage( _( "Creating animation sequence" ) );

		const AnimationSettings& as = cfg.GetAnimationSettings();

		NinePatchBitmap npb;

		if ( as.HasBitmap() )
		{
			wxLogInfo( _( "Loading stretching bitmap \u201C%s\u201D" ), as.GetBitmapFilename().GetFullName() );

			if ( !npb.Init( as.GetBitmapFilename().GetFullPath() ) )
			{
				wxLogError( _( "Fail to load stretching bitmap \u201C%s\u201D" ), as.GetBitmapFilename().GetFullName() );
				return false;
			}
		}
		else
		{
			if ( !npb.Init( as.GetBorderColour(), as.GetFillColour(), as.GetBorderWidth() ) )
			{
				wxLogError( _( "Fail to create fill pattern" ) );
				return false;
			}
		}

		wxImage img( mcWaveDrawer.GetBitmap() );

		wxFileName outFn( cfg.GetOutputFile() );

		wxFileName workDir;
		if ( cfg.RunFfmpeg() )
		{ // creating sequence in temporary directory
			workDir.AssignDir( wxStandardPaths::Get().GetTempDir() );
		}
		else
		{ // creating nontemporary image squence
			workDir.AssignDir( outFn.GetPath() );
			if ( workDir.IsRelative() )
			{
				if ( !workDir.MakeAbsolute() )
				{
					wxLogWarning( _( "Cannot make path \u201C%s\u201D absolute" ), workDir.GetFullPath() );
				}
			}
		}
		workDir.AppendDir( wxString::Format( "%s.wav2img", outFn.GetName() ) );

		wxLogInfo( _( "Working directory is \u201C%s\u201D" ), workDir.GetPath() );

		if ( !workDir.DirExists() )
		{
			if ( !workDir.Mkdir() )
			{
				wxLogError( _( "Fail to create working directory \u201C%s\u201D" ), workDir.GetPath() );
				return false;
			}
		}
		else
		{
			if ( !workDir.Rmdir( wxPATH_RMDIR_RECURSIVE ) )
			{
				wxLogError( _( "Fail to remove directory \u201C%s\u201D" ), workDir.GetPath() );
				return false;
			}

			if ( !workDir.Mkdir() )
			{
				wxLogError( _( "Fail to re-create working directory \u201C%s\u201D" ), workDir.GetPath() );
				return false;
			}
		}

		bool bRes = create_animation( 
			workDir,
			cfg,
			mcWaveDrawer.GetBitmap(),
			npb,
			mcWaveDrawer.GetRects(),
			mcWaveDrawer.GetTrackDuration(),
			bUseCuePoints, cuePoints
		);

		if ( cfg.RunFfmpeg() && cfg.DeleteTemporaryFiles() && !workDir.Rmdir( wxPATH_RMDIR_RECURSIVE ) )
		{
			wxLogError( _( "Fail to remove directory \u201C%s\u201D" ), workDir.GetPath() );
		}

		return bRes;
	}
	else
	{
		wxImage img( mcWaveDrawer.GetBitmap() );
		set_image_options( img, cfg, fn );

		wxLogInfo( _( "Saving image to file \u201C%s\u201D" ), fn.GetFullName() );
		bool res = img.SaveFile( fn.GetFullPath() );

		if ( res )
		{
			wxLogInfo( _( "Image saved to file \u201C%s\u201D" ), fn.GetFullName() );
			return true;
		}
		else
		{
			wxLogError( _( "Fail to save image to file \u201C%s\u201D" ), fn.GetFullName() );
			return false;
		}
	}
}

static bool save_rendered_wave( McChainWaveDrawer& waveDrawer, const wxConfiguration& cfg, bool bUseCuePoints, const wxTimeSpanArray& cuePoints )
{
	wxFileName fn( cfg.GetOutputFile() );

	if ( !fn.IsOk() )
	{
		return false;
	}

	switch ( cfg.GetDrawingMode() )
	{
		case DRAWING_MODE_AUDIO:
		{
			ArrayWaveDrawer* pAwd			= static_cast< ArrayWaveDrawer* >( waveDrawer.GetWaveDrawer() );
			AudioRenderer*	 pAudioRenderer = static_cast< AudioRenderer* >( pAwd->GetDrawer( 0 ) );
			return pAudioRenderer->GenerateAudio( fn, cfg.GetDrawerSettings().GetFrequency(), cfg.GetDrawerSettings().GetBaselinePosition() );
		}

		case DRAWING_MODE_SIMPLE:
		case DRAWING_MODE_RASTER1:
		case DRAWING_MODE_RASTER2:
		case DRAWING_MODE_POLY:
		{
			const McGraphicalContextWaveDrawer* pGc = static_cast< const McGraphicalContextWaveDrawer* >( waveDrawer.GetWaveDrawer() );
#ifdef __WXMSW__
#if wxUSE_ENH_METAFILE
			wxEnhMetaFile* pEmf = pGc->GetMetafile();

			if ( pEmf != NULL )
			{
				return save_image( fn, cfg, pEmf );
			}
			else
			{
				return save_image( fn, cfg, *pGc, bUseCuePoints, cuePoints );
			}
#endif
#else
			return save_image( fn, cfg, *pGc );
#endif
		}
	}

	return true;
}

static void html_renderer()
{
	wxArrayInt ai;
	int n;
	MemoryGraphicsContext mgc( wxSize(800,600), 32, false );

	{
		wxScopedPtr< wxGraphicsContext > pGc( mgc.CreateGraphicsContext() );
		wxGCDC dc( pGc.get() );
		wxHtmlDCRenderer wdc;
		wdc.SetDC( &dc );
		wdc.SetSize( 800, 600 );
		wdc.SetHtmlText( "<h1 ALIGN=\"RIGHT\">Hello world!</h1>" );
		n = wdc.Render( 0, 0, ai );
		pGc.release();
	}

	wxImage img( mgc.GetImage() );
	img.SaveFile( "C:/Users/Normal/Documents/Visual Studio 2010/Projects/wxMatroska/html_render.png" );
}

int wxMyApp::OnRun()
{
	switch( m_cfg.GetInfoSubject() )
	{
		case wxConfiguration::INFO_VERSION:
		InfoVersion( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_COLOUR_FORMAT:
		InfoColourFormat( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_CUE_POINT_FORMAT:
		InfoCuePointFormat( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_CMD_LINE_TEMPLATE:
		InfoCmdLineTemplate( *wxMessageOutput::Get() );
		return 0;

		case wxConfiguration::INFO_SYSTEM_SETTINGS:
		InfoSystemSettings( *wxMessageOutput::Get() );
		return 0;
	}

	wxTimeSpanArray cuePoints;
	bool			bUseCuePoints = false;

	if ( m_cfg.HasCuePointsFile() )
	{
		if ( !m_cfg.ReadCuePoints( cuePoints ) )
		{
			return 1000;
		}

		if ( cuePoints.IsEmpty() )
		{
			wxLogWarning( _( "No cue points found" ) );
			return 1001;
		}

		bUseCuePoints = true;
	}

	wxFileName inputFile( m_cfg.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		wxLogError( _( "Invalid input file \u201C%s\u201D" ), inputFile.GetFullName() );
		return 100;
	}

	wxLogMessage( _( "Processing \u201C%s\u201D, mode: %s, image size:%dx%d, color depth: %d" ),
			inputFile.GetFullName(),
			m_cfg.GetDrawingModeAsText(),
			m_cfg.GetImageSize().GetWidth(),
			m_cfg.GetImageSize().GetHeight(),
			m_cfg.GetImageColorDepth() );

	wxLogInfo( _( "Opening audio file" ) );
	SoundFile sfReader;

	if ( !sfReader.Open( inputFile.GetFullPath() ) )
	{
		wxLogError( _( "Cannot open sound file \u201C%s\u201D" ), inputFile.GetFullName() );
		return false;
	}

	const SF_INFO& sfInfo	= sfReader.GetInfo();
	wxTimeSpan	   duration = wxTimeSpan::Milliseconds( sfInfo.frames * 1000 / sfInfo.samplerate );
	wxLogMessage( _("Format: %s, samplerate: %uHz, channels: %u, duration: %s"),
		get_format_name( sfInfo.format ),
		sfInfo.samplerate,
		sfInfo.channels,
		duration.Format() );

	if ( m_cfg.HasCuePointsFile() || m_cfg.GenerateCuePoints() )
	{
		if ( m_cfg.GenerateCuePoints() )
		{
			bool bGenerated = m_cfg.GenerateCuePoints( duration, cuePoints );
			bUseCuePoints = bUseCuePoints || bGenerated;
		}

		cuePoints.Add( duration );
	}

	wxLogInfo( _( "Creating wave drawer" ) );
	wxScopedPtr< McChainWaveDrawer > pWaveDrawer( create_wave_drawer( m_cfg, sfReader.GetInfo(), bUseCuePoints, cuePoints ) );

	if ( !pWaveDrawer )
	{
		return 10;
	}

	{
		ProcessorHolder holder( *pWaveDrawer );
		wxLogInfo( _( "Drawing waveform" ) );
		read_audio_samples( sfReader, *pWaveDrawer );
	}

	wxLogInfo( _( "Waveform drawed" ) );
	return save_rendered_wave( *pWaveDrawer, m_cfg, bUseCuePoints, cuePoints ) ? 0 : 1;
}
