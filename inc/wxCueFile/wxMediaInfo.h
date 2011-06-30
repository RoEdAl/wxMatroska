/*
   wxMediaInfo.h
 */

#ifndef _WX_MEDIA_INFO_H
#define _WX_MEDIA_INFO_H

class wxMediaInfo:
	public wxObject
{
	wxDECLARE_ABSTRACT_CLASS( wxMediaInfo );

protected:

	wxDynamicLibrary m_dll;

public:

	static const wxChar MEDIA_INFO_LIBRARY[];

	static const wxChar* const SYMBOL_NAMES[];
	static const size_t		   SYMBOL_NAMES_SIZE;

	/** @brief Kinds of Stream */
	typedef enum MediaInfo_stream_t
	{
		MediaInfo_Stream_General,
		MediaInfo_Stream_Video,
		MediaInfo_Stream_Audio,
		MediaInfo_Stream_Text,
		MediaInfo_Stream_Chapters,
		MediaInfo_Stream_Image,
		MediaInfo_Stream_Menu,
		MediaInfo_Stream_Max
	} MediaInfo_stream_C;

	typedef enum MediaInfo_info_t
	{
		MediaInfo_Info_Name,
		MediaInfo_Info_Text,
		MediaInfo_Info_Measure,
		MediaInfo_Info_Options,
		MediaInfo_Info_Name_Text,
		MediaInfo_Info_Measure_Text,
		MediaInfo_Info_Info,
		MediaInfo_Info_HowTo,
		MediaInfo_Info_Max
	} MediaInfo_info_C;

	typedef void* ( __stdcall * MEDIAINFO_New )();
	typedef void ( __stdcall * MEDIAINFO_Delete )( void* );
	typedef size_t ( __stdcall * MEDIAINFO_Open )( void*, const wxChar* );
	typedef void ( __stdcall * MEDIAINFO_Close )( void* );
	typedef const wxChar* ( __stdcall * MEDIAINFO_Get )( void*, MediaInfo_stream_C StreamKind, size_t StreamNumber, const wxChar* Parameter, MediaInfo_info_C KindOfInfo, MediaInfo_info_C KindOfSearch );

protected:

	MEDIAINFO_New	 m_fnNew;
	MEDIAINFO_Delete m_fnDelete;
	MEDIAINFO_Open	 m_fnOpen;
	MEDIAINFO_Close	 m_fnClose;
	MEDIAINFO_Get	 m_fnGet;

	static bool load_symbols( const wxDynamicLibrary&, wxArrayPtrVoid& );

public:

	void* MediaInfoNew();
	void MediaInfoDelete( void* );
	size_t MediaInfoOpen( void*, const wxChar* );
	void MediaInfoClose( void* );
	const wxChar* MediaInfoGet( void*, MediaInfo_stream_C StreamKind, size_t StreamNumber, const wxChar* Parameter, MediaInfo_info_C KindOfInfo = MediaInfo_Info_Text, MediaInfo_info_C KindOfSearch = MediaInfo_Info_Name );

public:

	wxMediaInfo( void );

	bool Load();
	bool IsLoaded() const;
	void Unload();
};

#endif

