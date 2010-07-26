/*
	wxCueChapters.cpp
*/

#include "StdWx.h"

#include <wxIndex.h>
#include <wxTrack.h>
#include <wxCueSheetReader.h>
#include <wxTextCueSheetRenderer.h>
#include "wxXmlCueSheetRenderer.h"
#include "wxConfiguration.h"

static const wxChar APP_VERSION[] = wxT("0.1 beta");
static const wxChar APP_AUTHOR[] = wxT("Edmunt Pienkowsky: roed@onet.eu");

static int convert_cue_sheet( const wxString& sInputFile, const wxCueSheet& cueSheet, const wxConfiguration& cfg )
{
	wxString sOutputFile( cfg.GetOutputFile( sInputFile ) );
	if ( cfg.SaveCueSheet() )
	{
		wxFileOutputStream fos( sOutputFile );
		if ( !fos.IsOk() )
		{
			wxLogError( _("Fail to open %s file"), sOutputFile );
			return 1;
		}

		wxTextOutputStream tos( fos );
		wxTextCueSheetRenderer renderer( &tos );
		if ( !renderer.Render( cueSheet ) )
		{
			return 1;
		}
	}
	else
	{
		wxXmlCueSheetRenderer renderer( cfg, sInputFile, sOutputFile );
		if ( renderer.Render( cueSheet ) )
		{
			if ( !renderer.SaveXmlDoc() )
			{
				wxLogError( _("Fail to save output %s file"), sOutputFile );
				return 1;
			}
		}
		else
		{
			wxLogError( _("Fail to export cue sheet to Matroska chapters") );
			return 1;
		}
	}

	return 0;
}

static int process_cue_file( wxCueSheetReader& reader, const wxString& sInputFile, const wxConfiguration& cfg )
{
	if ( cfg.IsEmbedded() )
	{
		if ( !reader.ReadEmbeddedCueSheet( sInputFile ) )
		{
			wxLogError( _("Fail to read embedded sue sheet from file %s or parse error"), sInputFile );
			return 1;
		}
	}
	else
	{
		if ( !reader.ReadCueSheet( sInputFile ) )
		{
			wxLogError( _("Fail to read or parse input CUE file %s"), sInputFile );
			return 1;
		}
	}

	if ( cfg.HasSingleDataFile() || cfg.IsEmbedded() )
	{
		wxCueSheet cueSheet( reader.GetCueSheet() );
		if ( !cfg.HasSingleDataFile() )
		{
			wxDataFile dataFile( sInputFile, wxDataFile::WAVE );
			cueSheet.SetSingleDataFile( dataFile );
		}
		else
		{
			wxDataFile dataFile( cfg.GetSingleDataFile(), wxDataFile::WAVE );
			cueSheet.SetSingleDataFile( dataFile );
		}
		return convert_cue_sheet( sInputFile, cueSheet, cfg );
	}
	else
	{
		return convert_cue_sheet( sInputFile, reader.GetCueSheet(), cfg );
	}
}

static int do_conversion( const wxConfiguration& cfg )
{
	wxCueSheetReader reader;
	reader.UsePolishQuotationMarks( cfg.UsePolishQuotationMarks() );

	int res;
	const wxArrayString& inputFile = cfg.GetInputFiles();
	for( size_t i=0; i<inputFile.Count(); i++ )
	{
		wxFileName fn( inputFile[i] );
		wxDir dir( fn.GetPath() );
		wxString sFileSpec( fn.GetFullName() );
		wxString sInputFile;
		if ( dir.GetFirst( &sInputFile, sFileSpec, wxDIR_FILES ) )
		{
			while( true )
			{
				fn.SetFullName( sInputFile );
				res = process_cue_file( reader, fn.GetFullPath(), cfg );
				if ( (res != 0) && cfg.AbortOnError() ) break;

				if ( !dir.GetNext( &sInputFile ) ) break;
			}
		}
	}

	return cfg.AbortOnError()? res : 0;
}

static void add_version_infos( wxCmdLineParser& cmdline )
{
	wxString sSep( wxT('='), 70 );
	cmdline.AddUsageText( sSep );
	cmdline.AddUsageText( wxString::Format( wxT("Application version: %s"), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( wxT("Author: %s"), APP_AUTHOR ) );
	cmdline.AddUsageText( wxString::Format( wxT("Operationg system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
	cmdline.AddUsageText( wxString::Format( wxT("wxWidgets version: %d.%d.%d"), wxMAJOR_VERSION, wxMAJOR_VERSION, wxRELEASE_NUMBER ) );
	cmdline.AddUsageText( sSep );
}

static void add_format_description( wxCmdLineParser& cmdline )
{
	wxString sSep( wxT('='), 70 );
	cmdline.AddUsageText( sSep );
	cmdline.AddUsageText( _("Formating directives") );

	cmdline.AddUsageText( _("\t%da% - disc arranger") );
	cmdline.AddUsageText( _("\t%dc% - disc composer") );
	cmdline.AddUsageText( _("\t%dp% - disc performer") );
	cmdline.AddUsageText( _("\t%ds% - disc songwriter") );
	cmdline.AddUsageText( _("\t%dt% - disc title") );

	cmdline.AddUsageText( _("\t%n% - track number") );
	cmdline.AddUsageText( _("\t%ta% - track arranger") );
	cmdline.AddUsageText( _("\t%tc% - track composer") );
	cmdline.AddUsageText( _("\t%tp% - track performer") );
	cmdline.AddUsageText( _("\t%ts% - track songwriter") );
	cmdline.AddUsageText( _("\t%tt% - track title") );

	cmdline.AddUsageText( _("\t%aa% - track or disc arranger") );
	cmdline.AddUsageText( _("\t%ac% - track or disc composer") );
	cmdline.AddUsageText( _("\t%ap% - track or disc performer") );
	cmdline.AddUsageText( _("\t%as% - track or disc songwriter") );
	cmdline.AddUsageText( _("\t%at% - track or disc title") );
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( !::wxInitialize( argc, argv ) ) {
		return 100;
	}

	wxCmdLineParser cmdline( argc, argv );
	cmdline.AddSwitch( wxT("h"), wxT("help"), _("Display short information about this application and usage."), wxCMD_LINE_OPTION_HELP|wxCMD_LINE_PARAM_OPTIONAL );
	wxConfiguration::AddCmdLineParams( cmdline );
	cmdline.SetLogo( _("This application converts cue sheet files to Matroska XML chapter files in a more advanced way than standard Matroska tools.") );
	add_format_description( cmdline );
	add_version_infos( cmdline );

	int res = 0;
	if ( cmdline.Parse() == 0 )
	{
		wxConfiguration cfg;
		if ( cfg.Read( cmdline ) )
		{
			res = do_conversion( cfg );
		}
		else
		{
			res = 1;
		}
	}

	::wxUninitialize();
	return res;
}

