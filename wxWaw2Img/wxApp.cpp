/*
 * wxApp.cpp
 */
#include "StdWx.h"
#include <sndfile.h>
#include "FloatArray.h"
#include "LogarithmicScale.h"
#include "Interval.h"
#include "wxConfiguration.h"
#include "wxApp.h"

#include "SampleProcessor.h"
#include "WaveDrawer.h"
#include "SampleChunker.h"
#include "WaveDrawerGraphicsContext.h"
#include "WaveDrawerRaster.h"
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

// ===============================================================================

const wxChar wxMyApp::APP_NAME[]		= wxT( "wav2img" );
const wxChar wxMyApp::APP_VERSION[]		= wxT( "1.0" );
const wxChar wxMyApp::APP_VENDOR_NAME[] = wxT( "Edmunt Pienkowsky" );
const wxChar wxMyApp::APP_AUTHOR[]		= wxT( "Edmunt Pienkowsky - roed@onet.eu" );

// ===============================================================================

wxIMPLEMENT_APP( wxMyApp );

wxMyApp::wxMyApp( void ):
	m_sSeparator( wxT( '=' ), 75 )
{}

void wxMyApp::AddSeparator( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( m_sSeparator );
}

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

void wxMyApp::AddVersionInfos( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( wxString::Format( _( "Application version: %s" ), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( _( "Author: %s" ), APP_AUTHOR ) );
	cmdline.AddUsageText( _( "License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php" ) );
	cmdline.AddUsageText( wxString::Format( _( "wxWidgets version: %d.%d.%d. Copyright \u00A9 1992-2008 Julian Smart, Robert Roebling, Vadim Zeitlin and other members of the wxWidgets team" ), wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER ) );
	cmdline.AddUsageText( wxString::Format( _( "libsndfile version: %s. Copyright \u00A9 1999-2011 Erik de Castro Lopo" ), get_libsndfile_version() ) );
	cmdline.AddUsageText( wxString::Format( _( "Operating system: %s" ), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
}

void wxMyApp::AddColourFormatDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _( "Color format specification:" ) );
	cmdline.AddUsageText( _( "\tCSS sytntax: RGB(176,45,235)" ) );
	cmdline.AddUsageText( _( "\tCSS syntax with alpha: RGBA(176,45,235,0.7)" ) );
	cmdline.AddUsageText( _( "\tHTML syntax (no alpha): #AABBFF" ) );
	cmdline.AddUsageText( _( "\tcolor name: yellow" ) );
}

void wxMyApp::AddDisplayDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _( "System settings:" ) );

	wxRect dplRect = wxGetClientDisplayRect();
	int	   nDepth  = wxDisplayDepth();
	wxSize res	   = wxGetDisplayPPI();

	cmdline.AddUsageText( wxString::Format( _( "Display size (pixels): %dx%d (%dx%d)" ), dplRect.width, dplRect.height, dplRect.x, dplRect.y ) );
	cmdline.AddUsageText( wxString::Format( _( "Display color depth (bits): %d" ), nDepth ) );
	cmdline.AddUsageText( wxString::Format( _( "Display resolution (pixels/inch): %dx%d" ), res.x, res.y ) );
	cmdline.AddUsageText( wxString::Format( _( "Background color: %s" ), wxSystemSettings::GetColour( wxConfiguration::COLOR_BACKGROUND ).GetAsString() ) );
	cmdline.AddUsageText( wxString::Format( _( "Second background color: %s" ), wxSystemSettings::GetColour( wxConfiguration::COLOR_BACKGROUND2 ).GetAsString() ) );
}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	wxAppConsole::OnInitCmdLine( cmdline );
	cmdline.SetLogo( _( "This application draws a waveform from audio file." ) );
	m_cfg.AddCmdLineParams( cmdline );
	AddSeparator( cmdline );
	AddColourFormatDescription( cmdline );
	AddSeparator( cmdline );
	AddDisplayDescription( cmdline );
	AddSeparator( cmdline );
	AddVersionInfos( cmdline );
	AddSeparator( cmdline );
}

bool wxMyApp::OnCmdLineParsed( wxCmdLineParser& cmdline )
{
	if ( !wxAppConsole::OnCmdLineParsed( cmdline ) )
	{
		return false;
	}

	if ( !m_cfg.Read( cmdline ) )
	{
		return false;
	}

	wxLogMessage( _( "%s ver. %s" ), GetAppDisplayName(), APP_VERSION );
	return true;
}

bool wxMyApp::OnInit()
{
	SetAppName( APP_NAME );
	SetVendorName( APP_VENDOR_NAME );
	SetVendorDisplayName( APP_AUTHOR );

	m_pColourDatabase.reset( new wxColourDatabase() );
	wxTheColourDatabase = m_pColourDatabase.get();
	InitImageHandlers();

	CoInitializeEx( NULL, COINIT_MULTITHREADED | COINIT_DISABLE_OLE1DDE );

	if ( !wxAppConsole::OnInit() )
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
			return new SimpleWaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), bUseCuePoints, cuePoints, cfg.GetSecondaryBackgroundColor() );
		}

		case DRAWING_MODE_RASTER1:
		{
			return new Raster1WaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.UseLogarithmicColorPalette(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), cfg.GetColourTo(), bUseCuePoints, cuePoints, cfg.GetSecondaryBackgroundColor() );
		}

		case DRAWING_MODE_RASTER2:
		{
			return new Raster2WaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.UseLogarithmicColorPalette(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), cfg.GetColourTo(), bUseCuePoints, cuePoints, cfg.GetSecondaryBackgroundColor() );
		}

		case DRAWING_MODE_POLY:
		return new PolyWaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.UseLogarithmicColorPalette(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), cfg.GetColourTo(), bUseCuePoints, cuePoints, cfg.GetSecondaryBackgroundColor() );
	}

	wxASSERT( false );
	return NULL;
}

static McChainWaveDrawer* create_wave_drawer( const wxConfiguration& cfg, const SF_INFO& sfInfo, bool bUseCuePoints, const wxTimeSpanArray& cuePoints )
{
	if ( sfInfo.frames <= 0 )
	{
		wxLogInfo( _( "Unknown length of audio source" ) );
		return NULL;
	}

	wxUint64 nSamples	 = sfInfo.frames;
	wxUint16 nChannels	 = sfInfo.channels;
	wxUint32 nSamplerate = sfInfo.samplerate;

	MultiChannelWaveDrawer* pMcwd = NULL;

	switch ( cfg.GetDrawingMode() )
	{
		case DRAWING_MODE_AUDIO:
		{
			ArrayWaveDrawer* pAwd = new ArrayWaveDrawer( 1 );
			pAwd->AddDrawer( new AudioRenderer( nSamples, cfg.GetWidth(), cfg.UseLogarithmicScale(), cfg.GetLogarithmBase(), nSamplerate ) );
			pMcwd = pAwd;
			break;
		}

		case DRAWING_MODE_SIMPLE:
		case DRAWING_MODE_RASTER1:
		case DRAWING_MODE_RASTER2:
		case DRAWING_MODE_POLY:
		{
			if ( cfg.MultiChannel() )
			{
				McGraphicalContextWaveDrawer* pGc = new McGraphicalContextWaveDrawer( 1 );

				wxGraphicsContext* gc = pGc->Initialize( cfg.GetWidth(), cfg.GetHeight(), cfg.GetImageColorDepth(), cfg.GetBackgroundColor() );

				if ( gc == NULL )
				{
					delete pGc;
					return NULL;
				}

				for ( wxUint16 nChannel = 0; nChannel < nChannels; nChannel++ )
				{
					pGc->AddDrawer( create_wave_drawer( cfg.GetDrawingMode(), cfg, nSamples, gc, cfg.GetDrawerRect( nChannel, nChannels ), bUseCuePoints, cuePoints ) );
				}

				pMcwd = pGc;
			}
			else
			{
				McGraphicalContextWaveDrawer* pGc = new McGraphicalContextWaveDrawer( 1 );

				wxGraphicsContext* gc = pGc->Initialize( cfg.GetWidth(), cfg.GetHeight(), cfg.GetImageColorDepth(), cfg.GetBackgroundColor() );

				if ( gc == NULL )
				{
					delete pGc;
					return NULL;
				}

				pGc->AddDrawer( create_wave_drawer( cfg.GetDrawingMode(), cfg, nSamples, gc, cfg.GetDrawerRect(), bUseCuePoints, cuePoints ) );
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

static bool save_rendered_wave( McChainWaveDrawer& waveDrawer, const wxConfiguration& cfg )
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
			return pAudioRenderer->GenerateAudio( fn.GetFullPath(), cfg.GetFrequency() );
		}

		case DRAWING_MODE_SIMPLE:
		case DRAWING_MODE_RASTER1:
		case DRAWING_MODE_RASTER2:
		case DRAWING_MODE_POLY:
		{
			McGraphicalContextWaveDrawer* pGc = static_cast< McGraphicalContextWaveDrawer* >( waveDrawer.GetWaveDrawer() );
			wxImage						  img( pGc->GetBitmap() );

			img.SetOption( wxIMAGE_OPTION_RESOLUTIONX, cfg.GetImageResolution().GetWidth() );
			img.SetOption( wxIMAGE_OPTION_RESOLUTIONY, cfg.GetImageResolution().GetHeight() );
			img.SetOption( wxIMAGE_OPTION_RESOLUTIONUNIT, cfg.GetImageResolutionUnits() );
			img.SetOption( wxIMAGE_OPTION_QUALITY, cfg.GetImageQuality() );
			img.SetOption( wxIMAGE_OPTION_FILENAME, fn.GetName() );

			wxLogInfo( _( "Saving image to file \u201C%s\u201D" ), fn.GetFullName() );
			bool res = img.SaveFile( fn.GetFullPath() );

			if ( res )
			{
				wxLogInfo( _( "Image sucessfully saved to file \u201C%s\u201D" ), fn.GetFullName() );
				return true;
			}
			else
			{
				wxLogError( _( "Fail to save image to file \u201C%s\u201D" ), fn.GetFullName() );
				return false;
			}
		}
	}

	return true;
}

int wxMyApp::OnRun()
{
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
		return 100;
	}

	wxLogInfo( _( "Opening audio file" ) );
	SoundFile sfReader;

	if ( !sfReader.Open( inputFile.GetFullPath() ) )
	{
		wxLogError( _( "Cannot open sound file \u201C%s\u201D" ), inputFile.GetFullName() );
		return false;
	}

	if ( m_cfg.HasCuePointsFile() || m_cfg.HasCuePointsInterval() )
	{
		const SF_INFO& sfInfo	= sfReader.GetInfo();
		wxTimeSpan	   duration = wxTimeSpan::Milliseconds( sfInfo.frames * 1000 / sfInfo.samplerate );

		wxLogInfo( _( "Input file duration: %s" ), duration.Format() );

		if ( m_cfg.HasCuePointsInterval() )
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
	return save_rendered_wave( *pWaveDrawer, m_cfg ) ? 0 : 1;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();

	CoUninitialize();
	wxLogMessage( _( "Done" ) );
	return res;
}

