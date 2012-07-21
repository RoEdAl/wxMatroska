/*
	SoundFileReader.h
*/

#ifndef _SOUND_FILE_READER_H_
#define _SOUND_FILE_READER_H_

class SoundFileReader
{

protected:

	SoundFile m_soundFile;

public:

	SoundFileReader();
	~SoundFileReader();

	bool Open( const wxFileName& fileName );
	wxUint64 GetSize() const;
	wxUint16 GetChannels() const;
	void ReadSamples( MultiChannelWaveDrawer& );

};

#endif