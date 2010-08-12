/*
	wxConfiguration.h
*/

#ifndef _WX_CONFIGURATION_H_
#define _WX_CONFIGURATION_H_

#ifndef _WX_FLAC_ENCODER_H_
class wxFlacEncoder;
#endif

class wxConfiguration :public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxConfiguration)

protected:

	wxArrayString m_inputFile;
	wxFileName m_outputFile;
	wxString m_sFlacExt;

	long m_max_lpc_order;
	long m_blocksize;
	long m_qlp_coeff_precision;
	long m_rice_partition_order_min;
	long m_rice_partition_order_max;
	bool m_mid_side;
	bool m_adaptive_mid_side;
	bool m_exhaustive_model_search;

public:

	const wxArrayString& GetInputFiles() const;
	wxString GetOutputFile( const wxString& ) const;

protected:

	static const wxChar FLAC_EXT[];

	void set_compression_level( int );

public:

	wxConfiguration(void);
	~wxConfiguration(void);

	static void  AddCmdLineParams( wxCmdLineParser& );
	bool Read( const wxCmdLineParser& );
	void ConfigureFlacEncoder( wxFlacEncoder& ) const;
};

#endif // _WX_CONFIGURATION_H
