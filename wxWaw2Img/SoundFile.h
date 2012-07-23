/*
        SoundFile.h
 */

#ifndef _SOUND_FILE_H_
#define _SOUND_FILE_H_

#include <sndfile.h>

class SoundFile
{
protected:

	static const SF_VIRTUAL_IO sf_virtual_io;

protected:

	wxFile	 m_file;
	SNDFILE* m_sndfile;
	SF_INFO	 m_sf_info;

public:

	SoundFile();
	~SoundFile();

	bool Open( const wxString&, wxFile::OpenMode = wxFile::read );
	bool Open( const wxString&, const SF_INFO&, wxFile::OpenMode = wxFile::read );
	bool Close();

public:

	SNDFILE* GetHandle() const;
	const SF_INFO& GetInfo() const;
	wxFile&		   GetFile() const;

protected:

	static sf_count_t vio_get_filelen( void* user_data );
	static sf_count_t vio_seek( sf_count_t offset, int whence, void* user_data );
	static sf_count_t vio_read( void* ptr, sf_count_t count, void* user_data );
	static sf_count_t vio_write( const void* ptr, sf_count_t count, void* user_data );
	static sf_count_t vio_tell( void* user_data );

	sf_count_t vio_get_filelen();
	sf_count_t vio_seek( sf_count_t offset, int whence );
	sf_count_t vio_read( void* ptr, sf_count_t count );
	sf_count_t vio_write( const void* ptr, sf_count_t count );
	sf_count_t vio_tell();
};

#endif

