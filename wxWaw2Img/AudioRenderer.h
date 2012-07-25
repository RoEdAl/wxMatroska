/*
 *      AudioRenderer.h
 */

#ifndef _AUDIO_RENDERER_H_
#define _AUDIO_RENDERER_H_

struct AudioColumn
{
	AudioColumn();
	AudioColumn( wxFloat32, wxUint32 );
	AudioColumn( const AudioColumn& );

	AudioColumn& operator =( const AudioColumn& );
	bool operator ==( const AudioColumn& ) const;

	wxFloat32 fAmplitude;
	wxUint32 nNumberOfSamples;
};

WX_DEFINE_ARRAY( AudioColumn, AudioColumnArray );

class AudioRenderer:
	public SampleChunker
{
	public:

		AudioRenderer( wxUint64, wxUint32, bool, wxFloat32, wxUint32 );
		const AudioColumnArray& GetAudioColumns() const;

		bool		GenerateAudio( const wxFileName &, wxUint32, wxFloat32 ) const;
		static bool GenerateAudio( const wxFileName &, const AudioColumnArray &, wxUint32, wxUint32, wxFloat32 );

	protected:

		virtual void ProcessInitializer();
		virtual void NextColumn( wxFloat32 fValue, wxFloat32 fLogSample );
		virtual void ProcessFinalizer();

	protected:

		bool			 m_bUseLogarithmicScale;
		wxUint32		 m_nSourceSamplerate;
		AudioColumnArray m_ac;
};

#endif

