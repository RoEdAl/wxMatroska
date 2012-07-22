/*
   wxApp.cpp
 */
#include "StdWx.h"
#include <sndfile.h>
#include "FloatArray.h"
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

const wxChar wxMyApp::APP_NAME[]		  = wxT( "wav2img" );
const wxChar wxMyApp::APP_VERSION[]		  = wxT( "1.0" );
const wxChar wxMyApp::APP_VENDOR_NAME[]	  = wxT( "Edmunt Pienkowsky" );
const wxChar wxMyApp::APP_AUTHOR[]		  = wxT( "Edmunt Pienkowsky - roed@onet.eu" );

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
	char ver[128];
	int nRes = sf_command (NULL, SFC_GET_LIB_VERSION, ver, sizeof (ver));

	wxString v( ver, nRes );
	return v;
}

void wxMyApp::AddVersionInfos( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( wxString::Format( _( "Application version: %s" ), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( _( "Author: %s" ), APP_AUTHOR ) );
	cmdline.AddUsageText( _( "License: Simplified BSD License - http://www.opensource.org/licenses/bsd-license.php" ) );
	cmdline.AddUsageText( wxString::Format( _("libsndfile version: %s"), get_libsndfile_version() ) );
	cmdline.AddUsageText( wxString::Format( _( "Operating system: %s" ), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
}

void wxMyApp::AddColourFormatDescription( wxCmdLineParser& cmdline )
{
	cmdline.AddUsageText( _( "Color format specification:" ) );
	cmdline.AddUsageText( _("\t- RGB(r,g,b) e.g RGB(176,45,235)" ) );
	cmdline.AddUsageText( _("\t- RGBA(r,g,b,a) e.g RGBA(176,45,235,0.7)" ) );
	cmdline.AddUsageText( _("\t- #RRGGBB (HTML format) e.g #AABBFF" ) );
	cmdline.AddUsageText( _("\t- colour_name e.g yellow" ) );
}

void wxMyApp::OnInitCmdLine( wxCmdLineParser& cmdline )
{
	wxAppConsole::OnInitCmdLine( cmdline );
	cmdline.SetLogo( _( "This application draws a wave file." ) );
	wxConfiguration::AddCmdLineParams( cmdline );
	AddSeparator( cmdline );
	AddColourFormatDescription( cmdline );
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
	double d;

	if ( !sf_command( m_soundFile.GetHandle(), SFC_CALC_NORM_SIGNAL_MAX, &d, sizeof(d) ) )
	{
		wxLogMessage( _("Normalized value: %f"), d );
	}
	else
	{
		d = 1;
	}
	*/

	int nChannels = soundFile.GetInfo().channels;
	int nSamplerate = soundFile.GetInfo().samplerate;
	sf_count_t nBlock = nChannels * nSamplerate;

	wxFloatArray block( new float[ nBlock ] );

	sf_count_t nCount = sf_readf_float( sndfile, block.get(), nSamplerate );
	while( nCount > 0 )
	{
		waveDrawer.ProcessFames( block.get(), nCount );
		nCount = sf_readf_float( sndfile, block.get(), nSamplerate );
	}

	soundFile.Close();
}

static WaveDrawer* create_wave_drawer( DRAWING_MODE eMode, const wxConfiguration& cfg, wxUint64 nNumberOfSamples, wxGraphicsContext* gc, const wxRect2DInt& rc )
{
	switch( eMode )
	{
		case DRAWING_MODE_SIMPLE:
		return new SimpleWaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom() );

		case DRAWING_MODE_RASTER1:
			return new Raster1WaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.UseLogarithmicColorPalette(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), cfg.GetColourTo() );

		case DRAWING_MODE_RASTER2:
		return new Raster2WaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.UseLogarithmicColorPalette(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), cfg.GetColourTo() );

		case DRAWING_MODE_POLY:
		return new PolyWaveDrawer( nNumberOfSamples, gc, cfg.UseLogarithmicScale(), cfg.UseLogarithmicColorPalette(), cfg.GetLogarithmBase(), rc, cfg.GetColourFrom(), cfg.GetColourTo() );
	}

	wxASSERT( false );
	return NULL;
}
static McChainWaveDrawer* create_wave_drawer( const wxConfiguration& cfg, const SF_INFO& sfInfo )
{
	if ( sfInfo.frames <= 0 )
	{
		wxLogInfo( _("Unknown length of audio source") );
		return NULL;
	}

	wxUint64 nSamples = sfInfo.frames;
	wxUint16 nChannels = sfInfo.channels;
	wxUint32 nSamplerate = sfInfo.samplerate;

	MultiChannelWaveDrawer* pMcwd = NULL;

	switch ( cfg.GetDrawingMode() )
	{
		case DRAWING_MODE_AUDIO:
		{
			ArrayWaveDrawer* pAwd = new ArrayWaveDrawer( 1 );
			pAwd->AddDrawer( new AudioRenderer( nSamples, cfg.GetWidth(), cfg.UseLogarithmicScale(), cfg.GetLogarithmBase(), nSamplerate ) );
			pMcwd = pAwd;
		}
		break;

		case DRAWING_MODE_SIMPLE:
		case DRAWING_MODE_RASTER1:
		case DRAWING_MODE_RASTER2:
		case DRAWING_MODE_POLY:
		{
			McGraphicalContextWaveDrawer* pGc = new McGraphicalContextWaveDrawer( 1 );

			wxGraphicsContext* gc = pGc->Initialize( cfg.GetWidth(), cfg.GetHeight(), cfg.GetImageColorDepth(), cfg.GetBackgroundColor() );
			if ( gc == NULL )
			{
				delete pGc;
				return NULL;
			}
			pGc->AddDrawer( create_wave_drawer( cfg.GetDrawingMode(), cfg, nSamples, gc, wxRect2DInt( 0,0,cfg.GetWidth(), cfg.GetHeight() ) ) );
			pMcwd = pGc;
		}
		break;
	}

	ChannelMixer* pMixer = new ChannelMixer( nChannels, pMcwd, cfg.PowerMix() );
	return pMixer;
}

static bool save_rendered_wave( McChainWaveDrawer* pWaveDrawer, const wxConfiguration& cfg )
{
	wxFileName fn( cfg.GetOutputFile() );
	if ( !fn.IsOk() )
	{
		return false;
	}

	switch( cfg.GetDrawingMode() )
	{
		case DRAWING_MODE_AUDIO:
		{
			ArrayWaveDrawer* pAwd = static_cast<ArrayWaveDrawer*>(pWaveDrawer->GetWaveDrawer());
			AudioRenderer* pAudioRenderer = static_cast<AudioRenderer*>(pAwd->GetDrawer(0));
			return pAudioRenderer->GenerateAudio( fn.GetFullPath(), cfg.GetFrequency() );
		}

		case DRAWING_MODE_SIMPLE:
		case DRAWING_MODE_RASTER1:
		case DRAWING_MODE_RASTER2:
		case DRAWING_MODE_POLY:
		{
			McGraphicalContextWaveDrawer* pGc = static_cast<McGraphicalContextWaveDrawer*>(pWaveDrawer->GetWaveDrawer());
			wxImage img( pGc->GetBitmap() );

			img.SetOption( wxIMAGE_OPTION_RESOLUTIONX, cfg.GetImageResolution().GetWidth() );
			img.SetOption( wxIMAGE_OPTION_RESOLUTIONY, cfg.GetImageResolution().GetHeight() );
			img.SetOption( wxIMAGE_OPTION_RESOLUTIONUNIT, cfg.GetImageResolutionUnits() );
			img.SetOption( wxIMAGE_OPTION_QUALITY, cfg.GetImageQuality() );
			img.SetOption( wxIMAGE_OPTION_FILENAME, fn.GetName() );

			wxLogInfo( _("Saving image to file %s"), fn.GetFullPath() );
			bool res = img.SaveFile( fn.GetFullPath() );
			if ( res )
			{
				wxLogInfo( _("Image sucessfully saved to file %s"), fn.GetFullPath() );
				return true;
			}
			else
			{
				wxLogError( _("Fail to save image to file %s"), fn.GetFullPath() );
				return false;
			}
		}
	}

	return true;
}

int wxMyApp::OnRun()
{
	wxFileName inputFile( m_cfg.GetInputFile() );

	if ( !inputFile.IsOk() )
	{
		return 100;
	}

	wxLogInfo( _("Opening audio file") );
	SoundFile sfReader;
	if ( !sfReader.Open( inputFile.GetFullPath() ) )
	{
		wxLogError( _("Cannot open sound file %s"), inputFile.GetFullName() );
		return false;
	}

	wxLogInfo( _("Creating wave drawer") );
	wxScopedPtr<McChainWaveDrawer> pWaveDrawer( create_wave_drawer( m_cfg, sfReader.GetInfo() ) );

	if ( !pWaveDrawer )
	{
		return 10;
	}

	{
		ProcessorHolder holder( *pWaveDrawer );
		wxLogInfo( _("Drawing wave") );
		read_audio_samples( sfReader, *pWaveDrawer );
	}

	wxLogInfo( _("Wave drawed") );

	return save_rendered_wave( pWaveDrawer.get(), m_cfg )? 0 : 1;
}

int wxMyApp::OnExit()
{
	int res = wxAppConsole::OnExit();
	wxLogMessage( _( "Done" ) );
	return res;
}
