/*
	wxCueChapters.cpp
*/

#include "StdWx.h"

#include <wxIndex.h>
#include <wxTrack.h>
#include <wxCueSheetReader.h>
#include "wxConfiguration.h"

static const wxChar APP_VERSION[] = wxT("0.1 beta");
static const wxChar APP_AUTHOR[] = wxT("Edmunt Pienkowsky: roed@onet.eu");

static wxXmlNode* add_chapter_time_start( wxXmlNode* pChapterAtom, const wxIndex& idx )
{
	wxXmlNode* pChapterTimeStart = new wxXmlNode( pChapterAtom, wxXML_ELEMENT_NODE, wxT("ChapterTimeStart") );
	wxXmlNode* pChapterTimeStartText = new wxXmlNode( pChapterTimeStart, wxXML_TEXT_NODE, wxEmptyString, idx.GetTimeStr() );

	return pChapterTimeStart;
}

static wxXmlNode* find_chapter_time_start( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	wxXmlNode* pPrev = (wxXmlNode*)NULL;
	while( pChild != (wxXmlNode*)NULL )
	{
		if ( pChild->GetName().CmpNoCase( wxT("ChapterTimeStart") ) == 0 )
		{
			return pChild;
		}
		pPrev = pChild;
		pChild = pChild->GetNext();
	}

	wxASSERT( pPrev != (wxXmlNode*)NULL );
	return pPrev;
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxString& text )
{
	wxXmlNode* pChapterTimeStart = find_chapter_time_start( pChapterAtom );

	wxXmlNode* pChapterTimeEnd = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterTimeEnd") );
	wxXmlNode* pChapterTimeEndText = new wxXmlNode( pChapterTimeEnd, wxXML_TEXT_NODE, wxEmptyString, text );

	pChapterAtom->InsertChildAfter( pChapterTimeEnd, pChapterTimeStart );

	return pChapterTimeEnd;
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxIndex& idx )
{
	return add_chapter_time_end( pChapterAtom, idx.GetTimeStr() );
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, wxULongLong samples )
{
	return add_chapter_time_end( pChapterAtom, wxDataFile::GetSamplesStr( samples ) );
}

static wxXmlNode* add_chapter_time_end( wxXmlNode* pChapterAtom, const wxTrack& track, unsigned long nOffset )
{
	wxIndex fidx( track.GetFirstIndex() );
	fidx -= nOffset;
	return add_chapter_time_end( pChapterAtom, fidx.GetTimeStr() );
}

static bool has_chapter_time_end( wxXmlNode* pChapterAtom )
{
	wxXmlNode* pChild = pChapterAtom->GetChildren();
	while( pChild != (wxXmlNode*)NULL )
	{
		if ( pChild->GetName().CmpNoCase( wxT("ChapterTimeEnd") ) == 0 )
		{
			return true;
		}
		pChild = pChild->GetNext();
	}
	return false;
}

static wxXmlNode* get_last_child( wxXmlNode* pNode )
{
	wxXmlNode* pChild = pNode->GetChildren();
	wxXmlNode* pPrev = (wxXmlNode*)NULL;
	while( pChild != (wxXmlNode*)NULL )
	{
		pPrev = pChild;
		pChild = pChild->GetNext();
	}
	return pPrev;
}

static wxXmlNode* add_chapter_display( wxXmlNode* pChapterAtom, const wxString& sChapterString, const wxString& sLang )
{
	wxXmlNode* pChapterDisplay = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterDisplay") );
	wxXmlNode* pLastChild = get_last_child( pChapterAtom );
	if ( pLastChild != (wxXmlNode*)NULL )
	{
		pLastChild->SetNext( pChapterDisplay );
	}
	else
	{
		pChapterAtom->SetChildren( pChapterDisplay );
	}

	wxXmlNode* pChapterString = new wxXmlNode( pChapterDisplay, wxXML_ELEMENT_NODE, wxT("ChapterString") );
	wxXmlNode* pChapterStringText = new wxXmlNode( pChapterString, wxXML_TEXT_NODE, wxEmptyString, sChapterString );

	wxXmlNode* pChapterLanguage = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterLanguage") );
	wxXmlNode* pChapterLanguageText = new wxXmlNode( pChapterLanguage, wxXML_TEXT_NODE, wxEmptyString, sLang );
	pChapterString->SetNext( pChapterLanguage );

	return pChapterDisplay;

}

static wxXmlNode* add_idx_chapter_atom( wxXmlNode* pChapterAtom, const wxIndex& idx, const wxString& sLang )
{
	wxXmlNode* pIdxChapterAtom = new wxXmlNode( pChapterAtom, wxXML_ELEMENT_NODE, wxT("ChapterAtom") );
	wxXmlNode* pChapterTimeStart = add_chapter_time_start( pIdxChapterAtom, idx );

	wxString sChapterString;
	sChapterString.Format( wxT("INDEX %02d"), idx.GetNumber() );

	add_chapter_display( pChapterAtom, sChapterString, sLang );
	return pIdxChapterAtom;
}

static wxXmlDocument* comvert_to_matroska_chapters( const wxCueSheet& cueSheet, const wxConfiguration& cfg )
{
	wxXmlDocument* pXmlDoc = new wxXmlDocument();
	pXmlDoc->SetVersion( wxT("1.0") );
	pXmlDoc->SetFileEncoding( wxT("utf-8") );

	wxXmlNode* pChapters = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("Chapters") );
	pXmlDoc->SetRoot( pChapters );

	wxXmlNode* pLastComment;
	wxXmlNode* pComment = cfg.BuildXmlComments( pLastComment );
	pChapters->SetChildren( pComment );

	wxXmlNode* pEditionEntry = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("EditionEntry") );
	pLastComment->SetNext( pEditionEntry );

	wxXmlNode* pPrevChapterAtom = (wxXmlNode*)NULL;
	const wxArrayTrack& tracks = cueSheet.GetTracks();
	size_t tracksCount = tracks.Count();
	for( size_t i=0; i<tracksCount; i++ )
	{
		wxXmlNode* pChapterAtom = new wxXmlNode( (wxXmlNode*)NULL, wxXML_ELEMENT_NODE, wxT("ChapterAtom") );
		if ( i==0 )
		{
			pEditionEntry->SetChildren( pChapterAtom );
		}
		else
		{
			pPrevChapterAtom->SetNext( pChapterAtom );
		}

		const wxTrack& track = tracks[i];

		// pre-gap
		if ( track.HasPreGap() && cfg.GetChapterTimeEnd() )
		{
			if ( pPrevChapterAtom != (wxXmlNode*)NULL )
			{
				add_chapter_time_end( pPrevChapterAtom, track.GetPreGap() );
			}
		}

		// indicies
		const wxArrayIndex& idxs = track.GetIndexes();
		size_t idxsCount = idxs.Count();
		for( size_t j=0; j<idxsCount; j++ )
		{
			const wxIndex& idx = idxs[j];
			switch ( idx.GetNumber() )
			{
				case 0: // pre-gap
				if ( cfg.GetChapterTimeEnd() )
				{
					if ( pPrevChapterAtom != (wxXmlNode*)NULL )
					{
						add_chapter_time_end( pPrevChapterAtom, idx );
					}
				}
				break;

				case 1: // start
				{
					add_chapter_time_start( pChapterAtom, idx );
				}
				break;

				default:
				add_idx_chapter_atom( pChapterAtom, idx, cfg.GetLang() );
				break;
			}
		}

		// post-gap
		if ( track.HasPostGap() && cfg.GetChapterTimeEnd() )
		{
			add_chapter_time_end( pPrevChapterAtom, track.GetPostGap() );
		}

		pPrevChapterAtom = pChapterAtom;
	}

	wxXmlNode* pChapterAtom = pEditionEntry->GetChildren();
	for( size_t i=0; i<tracksCount; i++ )
	{
		if ( cfg.GetChapterTimeEnd() )
		{
			if ( !has_chapter_time_end( pChapterAtom ) )
			{
				wxDataFile dataFile;
				if ( cueSheet.IsLastTrackForDataFile( i, dataFile ) )
				{
					wxULongLong samples = dataFile.GetNumberOfSamples( cfg.GetAlternateExtensions() );
					if ( samples != wxDataFile::wxInvalidNumberOfSamples )
					{
						add_chapter_time_end( pChapterAtom, samples );
					}
				}
				else if ( cfg.GetUnknownChapterTimeEndToNextChapter() && ((i+1) < tracksCount) )
				{
					const wxTrack& nextTrack = tracks[i+1];
					add_chapter_time_end( pChapterAtom, nextTrack, cfg.GetChapterOffset() );
				}
			}

			wxString s( cueSheet.FormatTrack( i, cfg.GetTrackNameFormat() ) );
			add_chapter_display( pChapterAtom, s, cfg.GetLang() );
		}
		pChapterAtom = pChapterAtom->GetNext();
	}

	return pXmlDoc;
}

static int convert_cue_sheet( const wxCueSheet& cueSheet, const wxConfiguration& cfg )
{
	if ( cfg.SaveCueSheet() )
	{
		wxFileOutputStream fos( cfg.GetOutputFile() );
		if ( !fos.IsOk() )
		{
			wxLogError( _("Fail to open %s file"), cfg.GetOutputFile().GetData() );
			return 1;
		}

		wxTextOutputStream tos( fos );
		cueSheet.ToStream( tos );
	}
	else
	{
		wxXmlDocument* pXmlDoc = comvert_to_matroska_chapters( cueSheet, cfg );
		if ( pXmlDoc != (wxXmlDocument*)NULL )
		{
			if ( !pXmlDoc->Save( cfg.GetOutputFile() ) )
			{
				delete pXmlDoc;
				wxLogError( _("Fail to save output %s file"), cfg.GetOutputFile().GetData() );
				return 1;
			}
			delete pXmlDoc;
		}
		else
		{
			wxLogError( _("Fail to export cue sheet to Matroska chapters") );
			return 1;
		}
	}

	return 0;
}

static int do_conversion( const wxConfiguration& cfg )
{
	wxCueSheetReader reader;
	reader.UsePolishQuotationMarks( cfg.UsePolishQuotationMarks() );
	if ( cfg.IsEmbedded() )
	{
		if ( !reader.ReadEmbeddedCueSheet( cfg.GetInputFile() ) )
		{
			wxLogError( _("Fail to read embedded sue sheet from file %s or parse error"), cfg.GetInputFile().GetData() );
			return 1;
		}
	}
	else
	{
		if ( !reader.ReadCueSheet( cfg.GetInputFile() ) )
		{
			wxLogError( _("Fail to read or parse input CUE file %s"), cfg.GetInputFile().GetData() );
			return 1;
		}
	}

	if ( cfg.HasSingleDataFile() || cfg.IsEmbedded() )
	{
		wxCueSheet cueSheet( reader.GetCueSheet() );
		wxDataFile dataFile( 
			cfg.IsEmbedded()? cfg.GetInputFile() : cfg.GetSingleDataFile(),
			wxDataFile::WAVE );
		cueSheet.SetSingleDataFile( dataFile );
		return convert_cue_sheet( cueSheet, cfg );
	}
	else
	{
		return convert_cue_sheet( reader.GetCueSheet(), cfg );
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	if ( !::wxInitialize( argc, argv ) ) {
		return 100;
	}

	wxCmdLineParser cmdline( argc, argv );
	cmdline.AddSwitch( wxT("h"), wxT("help"), _("Display short information about this application and usage."), wxCMD_LINE_OPTION_HELP|wxCMD_LINE_PARAM_OPTIONAL );
	wxConfiguration::AddCmdLineParams( cmdline );
	cmdline.SetLogo( _("This application converts CUE file to Matroska chapter file in a more advanced way than standard Matroska tools.") );

	wxString sSep( wxT('='), 70 );
	cmdline.AddUsageText( sSep );
	cmdline.AddUsageText( wxString::Format( wxT("Application version: %s"), APP_VERSION ) );
	cmdline.AddUsageText( wxString::Format( wxT("Author: %s"), APP_AUTHOR ) );
	cmdline.AddUsageText( wxString::Format( wxT("Operationg system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription() ) );
	cmdline.AddUsageText( wxString::Format( wxT("wxWidgets version: %d.%d.%d"), wxMAJOR_VERSION, wxMAJOR_VERSION, wxRELEASE_NUMBER ) );
	cmdline.AddUsageText( sSep );

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

