/*
	SoundFile.cpp
*/

#include "StdWx.h"
#include "SoundFile.h"

const SF_VIRTUAL_IO SoundFile::sf_virtual_io = {
	vio_get_filelen,
	vio_seek,
	vio_read,
	vio_write,
	vio_tell
};

SoundFile::SoundFile()
	:m_sndfile(NULL)
{}

SoundFile::~SoundFile()
{
	Close();
}

static int wxfile_mode_2_sf_mode( wxFile::OpenMode mode )
{
	switch( mode )
	{
		default:
		case wxFile::read:
		return SFM_READ;

		case wxFile::write:
		return SFM_WRITE;

		case wxFile::read_write:
		return SFM_RDWR;
	}
}

bool SoundFile::Open (const wxString &filename, wxFile::OpenMode mode )
{
	if ( !m_file.Open( filename, mode ) )
	{
		return false;
	}

	m_sndfile = sf_open_virtual( const_cast<SF_VIRTUAL_IO*>(&sf_virtual_io), wxfile_mode_2_sf_mode(mode), &m_sf_info, this );

	if ( m_sndfile == NULL )
	{
		m_file.Close();
		return false;
	}

	return true;
}

bool SoundFile::Open (const wxString &filename, const SF_INFO& sf_info, wxFile::OpenMode mode )
{
	if ( !m_file.Open( filename, mode ) )
	{
		return false;
	}

	m_sf_info = sf_info;
	m_sndfile = sf_open_virtual( const_cast<SF_VIRTUAL_IO*>(&sf_virtual_io), wxfile_mode_2_sf_mode(mode), &m_sf_info, this );

	if ( m_sndfile == NULL )
	{
		m_file.Close();
		return false;
	}

	return true;
}

bool SoundFile::Close()
{
	if ( m_sndfile != NULL )
	{
		sf_close( m_sndfile );
		m_file.Close();
		m_sndfile = NULL;

		return true;
	}

	return true; // already closed
}

SNDFILE* SoundFile::GetHandle() const
{
	return m_sndfile;
}

const SF_INFO& SoundFile::GetInfo() const
{
	return m_sf_info;
}

wxFile& SoundFile::GetFile() const
{
	return const_cast<wxFile&>(m_file);
}

sf_count_t  SoundFile::vio_get_filelen(void *user_data)
{
	return static_cast<SoundFile*>(user_data)->vio_get_filelen();
}

sf_count_t  SoundFile::vio_seek(sf_count_t offset, int whence, void *user_data)
{
	return static_cast<SoundFile*>(user_data)->vio_seek( offset, whence );
}

sf_count_t  SoundFile::vio_read(void *ptr, sf_count_t count, void *user_data)
{
	return static_cast<SoundFile*>(user_data)->vio_read( ptr, count );
}

sf_count_t  SoundFile::vio_write(const void *ptr, sf_count_t count, void *user_data)
{
	return static_cast<SoundFile*>(user_data)->vio_write( ptr, count );
}

sf_count_t  SoundFile::vio_tell(void *user_data)
{
	return static_cast<SoundFile*>(user_data)->vio_tell();
}

sf_count_t  SoundFile::vio_get_filelen()
{
	return m_file.Length();
}

sf_count_t  SoundFile::vio_seek(sf_count_t offset, int whence)
{
	switch( whence )
	{
		case SEEK_CUR:
		return m_file.Seek( offset, wxFromCurrent );

		case SEEK_SET:
		return m_file.Seek( offset, wxFromStart );

		case SEEK_END:
		return m_file.Seek( offset, wxFromEnd );

		default:
		wxLogDebug( _("Where should I seek ? - %d"), whence );
		return m_file.Tell();
	}
}

sf_count_t  SoundFile::vio_read(void *ptr, sf_count_t count)
{
	return m_file.Read( ptr, count );
}

sf_count_t  SoundFile::vio_write(const void *ptr, sf_count_t count)
{
	return m_file.Write( ptr, count );
}

sf_count_t  SoundFile::vio_tell()
{
	return m_file.Tell();
}
