/*
	wxConfiguration.cpp
*/

#include "StdWx.h"
#include "wxFlacEncoder.h"
#include "wxConfiguration.h"
#include "wxApp.h"

static const size_t MAX_EXT_LEN = 20;
const wxChar wxConfiguration::FLAC_EXT[] = wxT("out.flac");

IMPLEMENT_CLASS( wxConfiguration, wxObject )


wxConfiguration::wxConfiguration(void)
	:m_sFlacExt(FLAC_EXT)
{
	set_compression_level( 5 );
}

wxConfiguration::~wxConfiguration(void)
{
}

void wxConfiguration::AddCmdLineParams( wxCmdLineParser& cmdLine )
{
	cmdLine.AddOption( wxT("o"), wxT("output"), _("Output FLAC file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("od"), wxT("output-directory"), _("Output directory (default: input directory)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("fe"), wxT("flac-extension"), wxString::Format( _("Default output flac file extension (default: %s)"), FLAC_EXT ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("0"), wxT("compression-level-0"), _("Synonymous with -l 0 -b 1152 -r 3"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("fast"), _("Synonymous with --compression-level-0"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("1"), wxT("compression-level-1"), _("Synonymous with -l 0 -b 1152 -M -r 3"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("2"), wxT("compression-level-2"), _("Synonymous with -l 0 -b 1152 -m -r 3"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("3"), wxT("compression-level-3"), _("Synonymous with -l 6 -b 4096 -r 4"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("4"), wxT("compression-level-4"), _("Synonymous with -l 8 -b 4096 -M -r 4"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("5"), wxT("compression-level-5"), _("Synonymous with -l 8 -b 4096 -m -r 5"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("6"), wxT("compression-level-6"), _("Synonymous with -l 8 -b 4096 -m -r 6"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("7"), wxT("compression-level-7"), _("Synonymous with -l 8 -b 4096 -m -e -r 6"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("8"), wxT("compression-level-8"), _("Synonymous with -l 12 -b 4096 -m -e -r 6"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("best"), _("Synonymous with --compression-level-8"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("l"), wxT("max-lpc-order"), _("Max LPC order; 0 => only fixed predictors"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("b"), wxT("blocksize"), _("Specify blocksize in samples"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("q"), wxT("qlp-coeff-precision"), _("Specify precision in bits"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddOption( wxT("r"), wxT("rice-partition-order"), _("Set [min,]max residual partition order"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("m"), wxT("mid-side"), _("Try mid-side coding for each frame"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("no-mid-side"), _("Don't try mid-side coding for each frame"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("M"), wxT("adaptive-mid-side"), _("Adaptive mid-side coding for all frames"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("no-adaptive-mid-side"), _("Don't try adaptive mid-side coding for all frames"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxT("e"), wxT("exhaustive-model-search"), _("Do exhaustive model search (expensive!)"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddSwitch( wxEmptyString, wxT("no-exhaustive-model-search"), _("Don't perform exhaustive model search"), wxCMD_LINE_PARAM_OPTIONAL );
	cmdLine.AddParam( _("<FLAC file>"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE|wxCMD_LINE_PARAM_OPTIONAL );
}

static bool check_ext( const wxString& sExt )
{
	return !sExt.IsEmpty() && (sExt.Length() < MAX_EXT_LEN);
}

void wxConfiguration::set_compression_level( int level )
{
	switch( level )
	{
		case 0:
		m_max_lpc_order = 0;
		m_blocksize = 1152;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 3;
		m_mid_side = false;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = false;
		break;

		case 1:
		m_max_lpc_order = 0;
		m_blocksize = 1152;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 3;
		m_mid_side = false;
		m_adaptive_mid_side = true;
		m_exhaustive_model_search = false;
		break;

		case 2:
		m_max_lpc_order = 0;
		m_blocksize = 1152;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 3;
		m_mid_side = true;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = false;
		break;

		case 3:
		m_max_lpc_order = 6;
		m_blocksize = 4096;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 4;
		m_mid_side = false;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = false;
		break;

		case 4:
		m_max_lpc_order = 8;
		m_blocksize = 4096;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 4;
		m_mid_side = false;
		m_adaptive_mid_side = true;
		m_exhaustive_model_search = false;
		break;

		case 5:
		m_max_lpc_order = 8;
		m_blocksize = 4096;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 5;
		m_mid_side = true;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = false;
		break;

		case 6:
		m_max_lpc_order = 8;
		m_blocksize = 4096;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 6;
		m_mid_side = true;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = false;
		break;

		case 7:
		m_max_lpc_order = 8;
		m_blocksize = 4096;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 6;
		m_mid_side = true;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = true;
		break;

		case 8:
		m_max_lpc_order = 12;
		m_blocksize = 4096;
		m_qlp_coeff_precision = 0;
		m_rice_partition_order_min = 0;
		m_rice_partition_order_max = 6;
		m_mid_side = true;
		m_adaptive_mid_side = false;
		m_exhaustive_model_search = true;
		break;
	}
}

bool wxConfiguration::Read( const wxCmdLineParser& cmdLine )
{
	bool bRes = true;
	wxString s;

	if ( cmdLine.Found( wxT("fe"), &s ) )
	{
		if ( check_ext( s ) )
		{
			m_sFlacExt = s;
		}
		else
		{
			wxLogWarning( _("Invalid flac file extension %s"), s );
			bRes = false;
		}
	}

	if ( cmdLine.GetParamCount() > 0 )
	{
		for( size_t i=0; i<cmdLine.GetParamCount(); i++ )
		{
			m_inputFile.Add( cmdLine.GetParam( i ) );
		}
	}

	if ( cmdLine.Found( wxT("o"), &s ) )
	{
		m_outputFile.Assign( s );
		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _("Fail to normalize path \u201C%s\u201D"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("od"), &s ) )
	{
		m_outputFile.AssignDir( s );
		if ( !m_outputFile.MakeAbsolute() )
		{
			wxLogInfo( _("Fail to normalize path \u201C%s\u201D"), s );
			bRes = false;
		}
	}

	if ( cmdLine.Found( wxT("0") ) || cmdLine.Found( wxT("fast") ) )
	{
		set_compression_level( 0 );
	}

	if ( cmdLine.Found( wxT("1") ) )
	{
		set_compression_level( 1 );
	}

	if ( cmdLine.Found( wxT("2") ) )
	{
		set_compression_level( 2 );
	}

	if ( cmdLine.Found( wxT("3") ) )
	{
		set_compression_level( 3 );
	}

	if ( cmdLine.Found( wxT("4") ) )
	{
		set_compression_level( 4 );
	}

	if ( cmdLine.Found( wxT("5") ) )
	{
		set_compression_level( 5 );
	}

	if ( cmdLine.Found( wxT("6") ) )
	{
		set_compression_level( 6 );
	}

	if ( cmdLine.Found( wxT("7") ) )
	{
		set_compression_level( 7 );
	}

	if ( cmdLine.Found( wxT("8") ) || cmdLine.Found( wxT("best") ) )
	{
		set_compression_level( 8 );
	}

	long n;
	if ( cmdLine.Found( wxT("l"), &n ) )
	{
		m_max_lpc_order = n;
	}

	if ( cmdLine.Found( wxT("b"), &n ) )
	{
		m_blocksize = n;
	}

	if ( cmdLine.Found( wxT("q"), &n ) )
	{
		m_qlp_coeff_precision = n;
	}

	if ( cmdLine.Found( wxT("r"), &s ) )
	{
		wxStringTokenizer tokenizer( s, wxT(",") );
		if ( tokenizer.HasMoreTokens() )
		{
			if ( tokenizer.GetNextToken().ToCLong( &n ) )
			{
				m_rice_partition_order_min = 0;
				m_rice_partition_order_max = n;
			}
			else
			{
				bRes = false;
			}

			m_rice_partition_order_max = -1;
			if ( tokenizer.HasMoreTokens() )
			{
				if ( tokenizer.GetNextToken().ToCLong( &n ) )
				{
					m_rice_partition_order_min = m_rice_partition_order_max;
					m_rice_partition_order_max = n;
				}
				else
				{
					bRes = false;
				}
			}
		}
	}

	if ( cmdLine.Found( wxT("m") ) )
	{
		m_mid_side = true;
		m_adaptive_mid_side = false;
	}

	if ( cmdLine.Found( wxT("no-mid-side") ) )
	{
		m_mid_side = false;
		m_adaptive_mid_side = false;
	}

	if ( cmdLine.Found( wxT("M") ) )
	{
		m_mid_side = true;
		m_adaptive_mid_side = true;
	}

	if ( cmdLine.Found( wxT("no-adaptive-mid-side") ) )
	{
		m_mid_side = false;
		m_adaptive_mid_side = false;
	}

	if ( cmdLine.Found( wxT("e") ) )
	{
		m_exhaustive_model_search = true;
	}

	if ( cmdLine.Found( wxT("no-exhaustive-model-search") ) )
	{
		m_exhaustive_model_search = false;
	}

	return bRes;
}

const wxArrayString& wxConfiguration::GetInputFiles() const
{
	return m_inputFile;
}

wxString wxConfiguration::GetOutputFile( const wxString& inputPath ) const
{
	wxFileName inputFile( inputPath );
	if ( !inputFile.IsOk() ) return wxEmptyString;

	if ( !m_outputFile.IsOk() )
	{
		inputFile.SetExt( m_sFlacExt );
	}
	else
	{
		if ( m_outputFile.IsDir() )
		{
			inputFile.SetPath( m_outputFile.GetPath() );
			inputFile.SetExt( m_sFlacExt );
		}
		else
		{
			inputFile = m_outputFile;
		}
	}
	return inputFile.GetFullPath();
}

void wxConfiguration::ConfigureFlacEncoder( wxFlacEncoder& encoder ) const
{
	wxASSERT( encoder.get_state() == FLAC__STREAM_ENCODER_UNINITIALIZED );

	encoder.set_max_lpc_order( m_max_lpc_order );
	encoder.set_blocksize( m_blocksize );
	encoder.set_qlp_coeff_precision( m_qlp_coeff_precision );
	encoder.set_min_residual_partition_order( m_rice_partition_order_min );
	encoder.set_max_residual_partition_order( m_rice_partition_order_max );
	encoder.set_do_mid_side_stereo( m_mid_side );
	encoder.set_loose_mid_side_stereo( m_adaptive_mid_side );
	encoder.set_do_exhaustive_model_search( m_exhaustive_model_search );
}
