/*
 *      wxMainFrame.cpp
 */

#include <app_config.h>
#include "wxApp.h"
#include "wxMainFrame.h"

namespace
{
	const int AUTO_SCROLL_UPDATE_INTERVAL = 2000;
	const int TIMER_IDLE_WAKE_UP_INTERVAL = 250;
	const int DEF_MARGIN = 2;

	wxStaticBoxSizer* create_static_box_sizer( wxWindow* parent, const wxString& label, wxOrientation orientation )
	{
		return new wxStaticBoxSizer( new wxStaticBox( parent, wxID_ANY, label ), orientation );
	}

	wxStaticText* create_static_text( wxWindow* parent, const wxString& label )
	{
		wxStaticText* const res = new wxStaticText( parent, wxID_ANY, label );

		res->Wrap( -1 );
		return res;
	}

	wxStaticLine* create_horizontal_static_line(wxWindow* parent)
	{
		return new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(0, 2), wxLI_HORIZONTAL);
	}

	wxSizerFlags get_horizontal_static_line_sizer_flags(wxWindow* wnd)
	{
		return wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM, wnd->FromDIP(2));
	}

	wxStaticLine* create_horizontal_static_line(const wxStaticBoxSizer* parentSizer)
	{
		return create_horizontal_static_line(parentSizer->GetStaticBox());
	}

	wxStaticText* create_static_text( const wxStaticBoxSizer* parentSizer, const wxString& label )
	{
		return create_static_text( parentSizer->GetStaticBox(), label );
	}

	wxComboBox* create_ro_combobox( wxWindow* parent )
	{
		return new wxComboBox( parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
	}

	template< size_t S >
	wxComboBox* create_ro_combobox( wxWindow* parent, const std::array< wxString, S >& elements, size_t selIdx = 0 )
	{
		wxComboBox* const cb = create_ro_combobox( parent );

		for (auto i = elements.cbegin(); i != elements.cend(); ++i)
		{
			cb->AppendString( *i );
		}

		cb->SetSelection( selIdx );
		return cb;
	}

	wxCheckBox* create_checkbox( wxWindow* parent, const wxString& label, bool val = false )
	{
		wxCheckBox* const res = new wxCheckBox( parent, wxID_ANY, label );

		res->SetValue( val );
		return res;
	}

	wxCheckBox* create_checkbox( const wxStaticBoxSizer* parentSizer, const wxString& label, bool val = false )
	{
		return create_checkbox( parentSizer->GetStaticBox(), label, val );
	}

	wxCheckBox* create_3state_checkbox( wxWindow* parent, const wxString& label, wxCheckBoxState state = wxCHK_UNDETERMINED )
	{
		wxCheckBox* const res = new wxCheckBox( parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER );

		res->Set3StateValue( state );
		return res;
	}

	bool is_checked( const wxCheckBox* checkBox )
	{
		if ( checkBox->Is3State() ) return ( checkBox->Get3StateValue() == wxCHK_CHECKED );
		else return checkBox->GetValue();
	}

	wxCheckBox* create_3state_checkbox( const wxStaticBoxSizer* parentSizer, const wxString& label, wxCheckBoxState state = wxCHK_UNDETERMINED )
	{
		return create_3state_checkbox( parentSizer->GetStaticBox(), label, state );
	}

	wxButton* create_button( const wxStaticBoxSizer* parentSizer, const wxString& label )
	{
		return new wxButton( parentSizer->GetStaticBox(), wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	}

	wxButton* create_button( wxWindow* parent, const wxString& label )
	{
		return new wxButton( parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	}

	wxTextCtrl* create_text_ctrl( wxWindow* parent, const wxString& label = wxEmptyString, unsigned long maxLength = 0 )
	{
		wxTextCtrl* const res = new wxTextCtrl( parent, wxID_ANY, label );

		if ( maxLength > 0 ) res->SetMaxLength( maxLength );
		return res;
	}

	wxTextCtrl* create_text_ctrl( const wxStaticBoxSizer* parentSizer, const wxString& label = wxEmptyString, unsigned long maxLength = 0 )
	{
		return create_text_ctrl( parentSizer->GetStaticBox(), label, maxLength );
	}

	wxSizerFlags get_left_ctrl_sizer_flags(wxWindow* wnd)
	{
		return wxSizerFlags().CenterVertical().Border( wxRIGHT, wnd->FromDIP(DEF_MARGIN) ).Proportion( 0 );
	}

	wxSizerFlags get_middle_crtl_sizer_flags(wxWindow* wnd)
	{
		return wxSizerFlags().CenterVertical().Border( wxLEFT | wxRIGHT, wnd->FromDIP(DEF_MARGIN) ).Proportion( 0 );
	}

	wxSizerFlags get_middle_exp_crtl_sizer_flags(wxWindow* wnd)
	{
		return wxSizerFlags().Border( wxLEFT | wxRIGHT, wnd->FromDIP(DEF_MARGIN) ).Proportion( 1 ).Expand();
	}

	wxSizerFlags get_right_crtl_sizer_flags(wxWindow* wnd)
	{
		return wxSizerFlags().CenterVertical().Border( wxLEFT, wnd->FromDIP(DEF_MARGIN) ).Proportion( 0 );
	}

	wxSizerFlags get_vertical_allign_sizer_flags()
	{
		return wxSizerFlags().CenterVertical().Proportion( 0 );
	}

	class MyProcess:
		public wxProcess
	{
		public:

			MyProcess()
			{
				Redirect();
			}

			void CreateTxtStreams()
			{
				m_pTxtInputStream.reset( new wxTextInputStream( *( GetInputStream() ), wxEmptyString, wxConvUTF8 ) );
				m_pTxtErrorStream.reset( new wxTextInputStream( *( GetErrorStream() ), wxEmptyString, wxConvUTF8 ) );
			}

			wxTextInputStream& GetTxtInputStream() const
			{
				return *( m_pTxtInputStream.get() );
			}

			wxTextInputStream& GetTxtErrorStream() const
			{
				return *( m_pTxtErrorStream.get() );
			}

			bool HaveOutOrErr() const
			{
				return !( GetInputStream()->Eof() && GetErrorStream()->Eof() );
			}

		private:

			wxScopedPtr< wxTextInputStream > m_pTxtInputStream;
			wxScopedPtr< wxTextInputStream > m_pTxtErrorStream;
	};

	class DropTarget:
		public wxFileDropTarget
	{
		public:

			DropTarget( wxMainFrame* pMainFrame ) : m_pMainFrame( pMainFrame )
			{}

			virtual bool OnDropFiles( wxCoord WXUNUSED( x ), wxCoord WXUNUSED( y ), const wxArrayString& filenames )
			{
				m_pMainFrame->OnDropFiles( filenames );
				return true;
			}

		private:

			wxMainFrame* const m_pMainFrame;
	};

	bool negatable_switch_option( wxArrayString& options, const wxCheckBox* checkBox, const wxString& cmdSwitch )
	{
		if ( !checkBox->IsEnabled() )
		{
			wxLogDebug( "cmd[%s] - check box disabled, command line option was not generated", cmdSwitch );
			return false;
		}

		if ( checkBox->Is3State() )
		{
			const wxCheckBoxState state = checkBox->Get3StateValue();
			switch ( state )
			{
				case wxCHK_CHECKED:
				{
					options.Add( wxString::Format( "-%s", cmdSwitch ) );
					return true;
				}

				case wxCHK_UNCHECKED:
				{
					options.Add( wxString::Format( "-%s-", cmdSwitch ) );
					return false;
				}

				default:
				{
					return false;
				}
			}
		}
		else
		{
			if ( checkBox->GetValue() )
			{
				options.Add( wxString::Format( "-%s", cmdSwitch ) );
				return true;
			}
			else
			{
				options.Add( wxString::Format( "-%s-", cmdSwitch ) );
				return false;
			}
		}
	}

	bool negatable_long_switch_option( wxArrayString& options, const wxCheckBox* checkBox, const wxString& cmdSwitch )
	{
		if ( !checkBox->IsEnabled() )
		{
			wxLogDebug( "cmd[%s] - check box disabled, command line option was not generated", cmdSwitch );
			return false;
		}

		if ( checkBox->Is3State() )
		{
			const wxCheckBoxState state = checkBox->Get3StateValue();
			switch ( state )
			{
				case wxCHK_CHECKED:
				{
					options.Add( wxString::Format( "--%s", cmdSwitch ) );
					return true;
				}

				case wxCHK_UNCHECKED:
				{
					options.Add( wxString::Format( "--%s-", cmdSwitch ) );
					return false;
				}

				default:
				{
					return false;
				}
			}
		}
		else
		{
			if ( checkBox->GetValue() )
			{
				options.Add( wxString::Format( "--%s", cmdSwitch ) );
				return true;
			}
			else
			{
				options.Add( wxString::Format( "--%s-", cmdSwitch ) );
				return false;
			}
		}
	}

	wxString join_strings( const wxArrayString& elems, const wxString& sep = wxEmptyString )
	{
		if ( elems.IsEmpty() ) return wxEmptyString;

		wxString res;

		for (wxArrayString::const_iterator i = elems.begin(); i != elems.end(); ++i)
		{
			res.Append( sep ).Append( *i );
		}

		return res;
	}

	bool have_spaces( const wxString& str, const wxRegEx& spaceChecker )
	{
		if ( str.StartsWith( "--" ) || str.StartsWith( '-' ) ) return false;

		return spaceChecker.Matches( str );
	}

	wxString options_to_str( const wxArrayString& options )
	{
		if ( options.IsEmpty() ) return wxEmptyString;

		const wxRegEx spaceChecker( "\\p{Z}", wxRE_NOSUB );

		wxASSERT( spaceChecker.IsValid() );

		wxString res;

		for (wxArrayString::const_iterator i = options.begin(); i != options.end(); ++i)
		{
			if ( have_spaces( *i, spaceChecker ) ) res.Append( '\"' ).Append( *i ).Append( "\" " );
			else res.Append( *i ).Append( ' ' );
		}

		res.RemoveLast();
		return res;
	}

	wxString make_fmask( const wxString& fname )
	{
		wxFileName fn( fname );

		fn.SetName( "*" );
		return fn.GetFullPath();
	}

	class CheckBoxUiUpdater
	{
		public:

			CheckBoxUiUpdater( wxCheckBox* checkBox )
				: m_checkBox( checkBox )
			{}

			CheckBoxUiUpdater( const CheckBoxUiUpdater& uiUpdater )
				: m_checkBox( uiUpdater.m_checkBox )
			{}

			void operator ()( wxUpdateUIEvent& event ) const
			{
				event.Enable( is_checked() );
			}

			bool operator !=( const CheckBoxUiUpdater& uiUpdater ) const
			{
				return m_checkBox != uiUpdater.m_checkBox;
			}

			bool operator ==( const CheckBoxUiUpdater& uiUpdater ) const
			{
				return m_checkBox == uiUpdater.m_checkBox;
			}

		private:

			wxCheckBox* m_checkBox;

			bool is_checked() const
			{
				if (m_checkBox->Is3State()) return (m_checkBox->Get3StateValue() == wxCHK_CHECKED || m_checkBox->Get3StateValue() == wxCHK_UNDETERMINED);
				else return m_checkBox->GetValue();
			}

	};

	class TreeCtrlUiUpdater
	{
		public:

			TreeCtrlUiUpdater( wxTreeCtrl* treeCtrl )
				: m_treeCtrl( treeCtrl )
			{}

			TreeCtrlUiUpdater( const TreeCtrlUiUpdater& uiUpdater )
				: m_treeCtrl( uiUpdater.m_treeCtrl )
			{}

			void operator ()( wxUpdateUIEvent& event ) const
			{
				const wxTreeItemId itemId = m_treeCtrl->GetSelection();

				event.Enable( itemId.IsOk() );
			}

			bool operator !=( const TreeCtrlUiUpdater& uiUpdater ) const
			{
				return m_treeCtrl != uiUpdater.m_treeCtrl;
			}

			bool operator ==( const TreeCtrlUiUpdater& uiUpdater ) const
			{
				return m_treeCtrl == uiUpdater.m_treeCtrl;
			}

		private:

			wxTreeCtrl* m_treeCtrl;
	};

	wxString get_array_item( const wxArrayString& a, const size_t idx )
	{
		const size_t cnt = a.GetCount();

		if ( idx >= cnt ) return wxEmptyString;

		return a[ idx ];
	}

	wxString truncate_dir_path( const wxFileName& fn, const size_t dirCnt )
	{
		wxFileName res( fn );

		while ( res.GetDirCount() > dirCnt )
		{
			res.RemoveLastDir();
		}

		if ( res.GetDirCount() == 0 ) return wxEmptyString;

		return res.GetFullPath().RemoveLast();
	}

	wxString find_common_path( const wxArrayString& paths )
	{
		if ( paths.IsEmpty() ) return wxEmptyString;

		const size_t cnt = paths.GetCount();

		wxFileName path;

		path.AssignDir( wxFileName( paths[ 0 ] ).GetPath() );

		if ( cnt == 1 ) return path.GetFullPath();

		size_t dirCnt = path.GetDirCount();

		std::vector< wxFileName > fn;

		for (size_t i = 1; i < cnt; ++i)
		{
			wxFileName f;
			f.AssignDir( wxFileName( paths[ i ] ).GetPath() );

			const size_t dirCnt1 = f.GetDirCount();

			if ( dirCnt1 > dirCnt ) dirCnt = dirCnt1;

			fn.push_back( f );
		}

		// check drive
		const wxString vol = path.GetVolume();

		for (auto i = fn.cbegin(); i != fn.cend(); ++i)
		{
			const wxString jvol = i->GetVolume();

			if ( vol.CmpNoCase( jvol ) != 0 ) return wxEmptyString;
		}

		// check dir components
		for (size_t i = 0; i < dirCnt; ++i)
		{
			const wxString dir = get_array_item( path.GetDirs(), i );

			if ( dir.IsEmpty() ) return truncate_dir_path( path, i );

			for (auto j = fn.cbegin(); j != fn.cend(); ++j)
			{
				const wxString jdir = get_array_item( j->GetDirs(), i );

				if ( jdir.IsEmpty() || ( dir.CmpNoCase( jdir ) != 0 ) ) return truncate_dir_path( path, i );
			}
		}

		return wxEmptyString;
	}

	void kill_console_process(long pid)
	{
		if (!AttachConsole(pid))
		{
			const WXDWORD err = GetLastError();
			if (err == ERROR_ACCESS_DENIED)
			{
				FreeConsole(); // detach first
				if (!AttachConsole(pid)) // attach again
				{
					return;
				}
			}
		}

		wxLogWarning(_("exe[t]: sending CTRL+BREAK event to process %ld"), pid);
		bool res = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, pid) != 0;
		if (!res)
		{
			WXDWORD err = GetLastError();
			wxLogError(_("exe[t]: signal CTRL+BREAK not sent, error code %d"), err);
		}
		res = FreeConsole() != 0;
	}

	void kill_console_process(const wxProcess& process)
	{
		kill_console_process(process.GetPid());
	}

	wxSize calc_text_size(int charWidth)
	{
		wxScreenDC dc;
		wxString txt(wxUniChar(0x2013), charWidth);
		const wxSize extent = dc.GetTextExtent(txt);
		return wxSize( extent.GetWidth(), -1 );
	}
}

wxPanel* wxMainFrame::create_src_dst_pannel( wxNotebook* notebook, const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnMiddleExp, const wxSizerFlags& btnRight )
{
	wxPanel* const    panel      = new wxPanel( notebook );
	wxBoxSizer* const panelSizer = new wxBoxSizer( wxVERTICAL );

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Sources" ), wxVERTICAL );

		m_treeCtrlInputFiles     = new wxTreeCtrl( sizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_TWIST_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT );
		m_treeCtrlInputFilesRoot = m_treeCtrlInputFiles->AddRoot( "hidden root" );
		sizer->Add( m_treeCtrlInputFiles, 1, wxEXPAND, btnLeft.GetBorderInPixels() );

		{
			wxBoxSizer* const innerSizer = new wxBoxSizer( wxHORIZONTAL );
			TreeCtrlUiUpdater uiUpdater( m_treeCtrlInputFiles );

			m_checkBoxSuggestMerge = create_checkbox( sizer, _( "Suggest merge mode" ), true );
			m_checkBoxSuggestMerge->SetFont( toolFont );
			m_checkBoxSuggestMerge->Bind( wxEVT_CHECKBOX, &wxMainFrame::OnSuggestMergeMode, this );
			innerSizer->Add( m_checkBoxSuggestMerge, btnLeft );

			innerSizer->Add( 0, 0, 1, wxEXPAND );

			m_buttonAdd = create_button( sizer, _( "Add" ) );
			m_buttonAdd->SetFont( toolFont );
			m_buttonAdd->Bind( wxEVT_BUTTON, &wxMainFrame::OnButtonAdd, this );
			innerSizer->Add( m_buttonAdd, btnLeft );

			m_buttonDelete = create_button( sizer, _( "Delete" ) );
			m_buttonDelete->SetFont( toolFont );
			m_buttonDelete->Bind( wxEVT_UPDATE_UI, uiUpdater );
			m_buttonDelete->Bind( wxEVT_BUTTON, &wxMainFrame::OnButtonDelete, this );
			innerSizer->Add( m_buttonDelete, btnMiddle );

			m_buttonMakeMask = create_button( sizer, _( "Make mask" ) );
			m_buttonMakeMask->SetFont( toolFont );
			m_buttonMakeMask->Bind( wxEVT_UPDATE_UI, uiUpdater );
			m_buttonMakeMask->Bind( wxEVT_BUTTON, &wxMainFrame::OnButtonMakeMask, this );
			innerSizer->Add( m_buttonMakeMask, btnMiddle );

			m_buttonAddMediaFile = create_button( sizer, _( "Add media" ) );
			m_buttonAddMediaFile->SetFont( toolFont );
			m_buttonAddMediaFile->Bind( wxEVT_UPDATE_UI, uiUpdater );
			m_buttonAddMediaFile->Bind( wxEVT_BUTTON, &wxMainFrame::OnButtonAddMediaFile, this );
			innerSizer->Add( m_buttonAddMediaFile, btnRight );

			sizer->Add( innerSizer, 0, wxTOP | wxBOTTOM | wxEXPAND, btnLeft.GetBorderInPixels() );
		}

		wxSizerFlags sizerFlags;
		sizerFlags.Expand().Proportion( 1 );

		panelSizer->Add( sizer, sizerFlags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Destination" ), wxHORIZONTAL );
		sizer->GetStaticBox()->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateDst, this );

		{
			const std::array< wxString, 3 > elems{ _( "Input directory" ), _( "Directory" ), _( "File" ) };
			m_comboBoxDst = create_ro_combobox( sizer->GetStaticBox(), elems );
		}
		sizer->Add( m_comboBoxDst, btnLeft );

		m_textCtrlDst = create_text_ctrl( sizer, wxEmptyString, 1024 );
		m_textCtrlDst->SetValue( wxStandardPaths::Get().GetUserDir( wxStandardPaths::Dir_Music ) );
		m_textCtrlDst->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateCtrlDst, this );
		sizer->Add( m_textCtrlDst, btnMiddleExp );

		m_buttonDstDir = create_button( sizer, _( "\u2026" ) );
		m_buttonDstDir->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateCtrlDst, this );
		m_buttonDstDir->Bind( wxEVT_BUTTON, &wxMainFrame::OnChooseDst, this );
		sizer->Add( m_buttonDstDir, btnRight );

		wxSizerFlags sizerFlags;
		sizerFlags.Expand();

		panelSizer->Add( sizer, sizerFlags );
	}

	panel->SetSizerAndFit( panelSizer );
	return panel;
}

wxPanel* wxMainFrame::create_general_panel( wxNotebook* notebook, const wxSizerFlags& btnLeft, const wxSizerFlags& centerVertical )
{
	wxPanel* const     panel      = new wxPanel( notebook );
	wxWrapSizer* const panelSizer = new wxWrapSizer();
	const wxSizerFlags sflags     = wxSizerFlags().Expand().Border( wxLEFT );

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "General" ), wxVERTICAL );

		m_checkBoxAbortOnErrors = create_3state_checkbox( sizer->GetStaticBox(), _( "Abort on errors" ) );
		sizer->Add( m_checkBoxAbortOnErrors );

		m_checkBoxMergeMode = create_checkbox( sizer->GetStaticBox(), _( "Merge mode" ) );
		m_checkBoxMergeMode->SetToolTip(_("Merge all input files into one cue sheet"));
		m_checkBoxMergeMode->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMergeMode, this );
		sizer->Add( m_checkBoxMergeMode );

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Input" ), wxVERTICAL );

		m_checkBoxUseMediaFiles = create_3state_checkbox( sizer->GetStaticBox(), _( "Use media files" ), wxCHK_CHECKED );
		m_checkBoxUseMediaFiles->SetToolTip(_("Use media file(s) to calculate end time of chapters"));
		sizer->Add( m_checkBoxUseMediaFiles );

		m_checkBoxMediaFilesWithoutCue = create_3state_checkbox( sizer, _( "Media files without cuesheet" ) );
		m_checkBoxMediaFilesWithoutCue->SetToolTip(_("Assume input file(s) as media files without cuesheet"));
		sizer->Add( m_checkBoxMediaFilesWithoutCue );

		{
			wxBoxSizer* const innerSizer = new wxBoxSizer( wxHORIZONTAL );

			innerSizer->Add( create_static_text( sizer, _( "Alternate media files extensions" ) ), btnLeft );

			m_textCtrlExt = create_text_ctrl( sizer, wxEmptyString, 50 );
			m_textCtrlExt->SetToolTip(_("Comma separated list of alternate media extensions (without leading dot)"));
			innerSizer->Add( m_textCtrlExt, centerVertical );

			sizer->Add( innerSizer );
		}

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Output" ), wxVERTICAL );

		{
			wxGridBagSizer* const innerSizer = new wxGridBagSizer();
			wxGBSpan              oneCol( 1, 2 );

			innerSizer->Add( create_static_text( sizer, _( "Format" ) ), wxGBPosition( 0, 0 ), wxDefaultSpan, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			{
				const std::array< wxString, 3 > elems{ _( "CUE sheet" ), _( "Matroska chapters" ), _( "Chapters (wav2img)" ) };
				m_comboBoxFormat = create_ro_combobox( sizer->GetStaticBox(), elems, 1 );
			}

			innerSizer->Add( m_comboBoxFormat, wxGBPosition( 0, 1 ), wxDefaultSpan, wxEXPAND | wxBOTTOM, btnLeft.GetBorderInPixels() );

			m_checkBoxGenerateTagsFile = create_checkbox( sizer, _( "Generate tags file" ), true );
			innerSizer->Add( m_checkBoxGenerateTagsFile, wxGBPosition( 1, 0 ), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			m_checkBoxGenerateOptionsFile = create_checkbox( sizer, _( "Generate options file" ), true );
			innerSizer->Add( m_checkBoxGenerateOptionsFile, wxGBPosition( 2, 0 ), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			m_checkBoxRunMkvmerge = create_checkbox( sizer, _( "Run mkvmerge" ), true );
			m_checkBoxRunMkvmerge->SetToolTip(_("Run mkvmerge using generated options file in order to generate Matroska container"));
			m_checkBoxRunMkvmerge->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxGenerateOptionsFile ) );
			innerSizer->Add( m_checkBoxRunMkvmerge, wxGBPosition( 3, 0 ), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			innerSizer->Add( create_static_text( sizer, _( "Language" ) ), wxGBPosition( 4, 0 ), wxDefaultSpan, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			m_textCtrlLang = create_text_ctrl( sizer, "und", 4 );
			m_textCtrlLang->SetToolTip(_("Default language for chapters and tags"));
			innerSizer->Add( m_textCtrlLang, wxGBPosition( 4, 1 ), wxDefaultSpan, wxEXPAND | wxTOP | wxBOTTOM, btnLeft.GetBorderInPixels() );

			innerSizer->Add( create_static_text( sizer, _( "Text encoding" ) ), wxGBPosition( 5, 0 ), wxDefaultSpan, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			{
				const std::array< wxString, 7 > elems{ _( "default" ), _( "UTF-8" ), _( "UTF-8 with BOM" ), _( "UTF-16" ), _( "UTF-16 with BOM" ), _( "UTF-16BE" ), _( "UTF-16BE with BOM" ) };
				m_comboBoxEncoding = create_ro_combobox( sizer->GetStaticBox(), elems, 2 );
				m_comboBoxEncoding->SetToolTip(_("Text encoding of generated CUE and XML files"));
			}
			innerSizer->Add( m_comboBoxEncoding, wxGBPosition( 5, 1 ), wxDefaultSpan, wxEXPAND | wxTOP, btnLeft.GetBorderInPixels() );

			sizer->Add( innerSizer, wxSizerFlags().Expand() );
		}

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Tags sources" ), wxVERTICAL );

		m_checkBoxUseCdTextTags = create_3state_checkbox( sizer, _( "CD-TEXT" ) );
		m_checkBoxUseCdTextTags->SetToolTip(_("Copy tags from CD-TEXT"));
		sizer->Add( m_checkBoxUseCdTextTags );

		m_checkBoxUseTagsFromCuesheetComments = create_3state_checkbox( sizer, _( "Cuesheet comments" ) );
		m_checkBoxUseTagsFromCuesheetComments->SetToolTip(_("Copy tags extracted from cuesheet comments"));
		sizer->Add( m_checkBoxUseTagsFromCuesheetComments );

		m_checkBoxUseTagsFromMediaFiles = create_3state_checkbox( sizer, _( "Media files" ) );
		m_checkBoxUseTagsFromMediaFiles->SetToolTip(_("Copy tags extracted from media files"));
		sizer->Add( m_checkBoxUseTagsFromMediaFiles );

		panelSizer->Add( sizer, sflags );
	}

	panel->SetSizerAndFit( panelSizer );
	return panel;
}

wxPanel* wxMainFrame::create_chapter_panel( wxNotebook* notebook, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& centerVertical )
{
	wxPanel* const     panel      = new wxPanel( notebook );
	wxWrapSizer* const panelSizer = new wxWrapSizer();
	const wxSizerFlags sflags = wxSizerFlags().Expand().Border(wxLEFT);

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Chapters" ), wxVERTICAL );

		m_checkBoxChapterEndTime = create_3state_checkbox( sizer, _( "Calculate end time of chapters (if possible)" ) );

		sizer->Add( m_checkBoxChapterEndTime );

		m_checkBoxChapterEndTimeFromNext = create_3state_checkbox( sizer, _( "Calculate chapters's end time from next chapter" ) );
		m_checkBoxChapterEndTimeFromNext->SetToolTip(_("If track's time is unknown set it to next track position using specified shifted left by frame offset"));
		sizer->Add( m_checkBoxChapterEndTimeFromNext );

		{
			wxBoxSizer* const       innerSizer = new wxBoxSizer( wxHORIZONTAL );
			const CheckBoxUiUpdater uiUpdater( m_checkBoxChapterEndTimeFromNext );

			const wxSizerFlags offsetFlags = wxSizerFlags(btnLeft).CenterVertical().Border(wxLEFT, 8);

			wxStaticText* const staticText = create_static_text( sizer, _( "Offset" ) );
			staticText->Bind( wxEVT_UPDATE_UI, uiUpdater );
			innerSizer->Add( staticText, offsetFlags );

			m_textCtrlChapterOffset = create_text_ctrl( sizer, "150", 4 );
			m_textCtrlChapterOffset->Bind( wxEVT_UPDATE_UI, uiUpdater );
			innerSizer->Add( m_textCtrlChapterOffset,btnMiddle );

			wxStaticText* const staticTextRight = create_static_text(sizer, _("frames"));
			staticTextRight->Bind(wxEVT_UPDATE_UI, uiUpdater);
			innerSizer->Add( staticTextRight, centerVertical);

			sizer->Add(innerSizer, 0, wxTOP, panel->FromDIP(DEF_MARGIN * 2));
		}

		{
			wxBoxSizer* const innerSizer = new wxBoxSizer( wxHORIZONTAL );

			innerSizer->Add( create_static_text( sizer, _( "For track 01 assume index" ) ), btnLeft );

			{
				const std::array< wxString, 2 > elems{ _( "00" ), _( "01" ) };
				m_comboBoxIdx = create_ro_combobox( sizer->GetStaticBox(), elems, 1 );
			}
			innerSizer->Add( m_comboBoxIdx, btnMiddle );

			innerSizer->Add( create_static_text( sizer, _( "as beginning of track" ) ), centerVertical );

			sizer->Add( innerSizer, 0, wxTOP, panel->FromDIP(DEF_MARGIN * 2 ));
		}

		m_checkBoxIdxToHiddenChapters = create_3state_checkbox( sizer, _( "Convert indexes to hidden chapters" ) );
		m_checkBoxIdxToHiddenChapters->SetToolTip(_("Convert CD indexes greater than 01 to hidden Matroska chapters"));
		sizer->Add( m_checkBoxIdxToHiddenChapters, 0, wxTOP, btnLeft.GetBorderInPixels() );

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Unicode tweaks" ), wxVERTICAL );

		m_checkBoxRemoveExtraSpaces = create_3state_checkbox(sizer, _("Remove extra spaces"));
		m_checkBoxRemoveExtraSpaces->SetToolTip(_("More than two spaces are shrinked to one"));
		sizer->Add(m_checkBoxRemoveExtraSpaces);

		m_checkBoxEllipsizeTags = create_3state_checkbox( sizer, _( "Ellipsize" ) );
		m_checkBoxEllipsizeTags->SetToolTip(_("Replace last three dots with \u2026 (ellipsis: U+2026) character"));
		sizer->Add( m_checkBoxEllipsizeTags );

		m_checkBoxCorrectDashes = create_3state_checkbox( sizer, _( "Correct dashes" ) );
		m_checkBoxCorrectDashes->SetToolTip(_("-\u2009\u2192\u2009\u2013 (en dash: U+2013)\n--\u2009\u2192\u2009\u2014 (em dash: U+2014)\n---\u2009\u2192\u2009\u2E3A (two-em dash: U+2E3A"));
		sizer->Add( m_checkBoxCorrectDashes );

		m_checkBoxCorrectSimpleQuotationMarks = create_3state_checkbox(sizer, _("Correct quotation marks"));
		m_checkBoxCorrectSimpleQuotationMarks->SetToolTip(_("Try to \"correct 'quotation' marks\"\nExamples:\n\u201EPolish \u201Aquotation\u2019 marks\u201D\n\u201CEnglish (US) \u2018quotation\u2019 marks\u201D\n\u00AB\u2005French \u2039\u2005angle\u2005\u203A marks\u2005\u00BB\n\u201EGerman \u201Aquotation\u2018 marks\u201C"));
		sizer->Add(m_checkBoxCorrectSimpleQuotationMarks);

		m_checkBoxCapitalizedRomanLiterals = create_3state_checkbox( sizer, _( "Use capitalized Roman Numerals" ) );
		m_checkBoxCapitalizedRomanLiterals->SetToolTip(_("Examples:\nIII\u2009\u2192\u2009\u2162 (roman numeral three: U+2162)\nXII\u2009\u2192\u2009\u216B (roman numeral twelve: U+216B)"));
		sizer->Add( m_checkBoxCapitalizedRomanLiterals );

		m_checkBoxLowercaseRomanLiterals = create_3state_checkbox( sizer, _( "Use lowercase Roman Numerals" ) );
		m_checkBoxLowercaseRomanLiterals->SetToolTip(_("Examples:\niii\u2009\u2192\u2009\u2172 (small roman numeral three: U+2172)\nxii\u2009\u2192\u2009\u217B (small roman numeral twelve: U+217B"));
		sizer->Add( m_checkBoxLowercaseRomanLiterals );

		m_checkBoxNumberFullStop = create_3state_checkbox(sizer, _("Use '<number> full stop' characters"));
		m_checkBoxNumberFullStop->SetToolTip(_("Use Unicode characters from \u2488 (digit one full stop: U+2488) to \u249B (number twenty full stop: U+249B)"));
		sizer->Add(m_checkBoxNumberFullStop);

		m_checkBoxSmallLetterParenthesized = create_3state_checkbox(sizer, _("use '<small letter> parenthesized' characters"));
		m_checkBoxSmallLetterParenthesized->SetToolTip(_("Use Unicode characters from \u249C (parenthesized latin small letter a: U-249C) to \u24B5 (parenthesized latin small letter z: U-24B5)"));
		sizer->Add(m_checkBoxSmallLetterParenthesized);

		sizer->Add(create_horizontal_static_line(sizer), get_horizontal_static_line_sizer_flags(panel));

		m_checkBoxSmallEmDash = create_3state_checkbox(sizer, _("Use small em dash"));
		m_checkBoxSmallEmDash->SetToolTip(_("When correcting dashes use '\uFE58' (U+FE58 : small em dash) character instead of '\u2014' (U+2014 : em dash) one"));
		m_checkBoxSmallEmDash->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxCorrectDashes));
		sizer->Add(m_checkBoxSmallEmDash);

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "MKA Formatting" ), wxVERTICAL );

		{
			const wxString trackTitleFmt(_("%dp% - %dt% - %tt%"));
			const wxSize editMinSize = calc_text_size(trackTitleFmt.Length() + 4);
			const wxSizerFlags txtSizerFlags = wxSizerFlags().Expand().Proportion(1);

			wxFlexGridSizer* const innerSizer = new wxFlexGridSizer(0, 2, btnLeft.GetBorderInPixels(), 0);
			innerSizer->AddGrowableCol(1);

			m_checkBoxMkaTitleFmt = create_checkbox( sizer, _( "Container title" ) );
			innerSizer->Add( m_checkBoxMkaTitleFmt, centerVertical );

			m_textCtrlMkaTitleFmt = create_text_ctrl( sizer, _( "%dp% - %dt%" ), 200 );
			m_textCtrlMkaTitleFmt->SetSizeHints(editMinSize);
			m_textCtrlMkaTitleFmt->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxMkaTitleFmt ) );
			innerSizer->Add( m_textCtrlMkaTitleFmt, txtSizerFlags );

			m_checkBoxTrackTilteFmt = create_checkbox( sizer, _( "Track title" ) );
			innerSizer->Add( m_checkBoxTrackTilteFmt, centerVertical );

			m_textCtrlTrackTilteFmt = create_text_ctrl( sizer, trackTitleFmt, 200 );
			m_textCtrlTrackTilteFmt->SetSizeHints(editMinSize);
			m_textCtrlTrackTilteFmt->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxTrackTilteFmt ) );
			innerSizer->Add( m_textCtrlTrackTilteFmt, txtSizerFlags );

			sizer->Add( innerSizer, 0, wxEXPAND );
		}

		panelSizer->Add( sizer, sflags );
	}

	panel->SetSizerAndFit( panelSizer );
	return panel;
}

wxPanel* wxMainFrame::create_adv_panel( wxNotebook* notebook, const wxSizerFlags& btnLeft, const wxSizerFlags& centerVertical )
{
	wxPanel* const     panel      = new wxPanel( notebook );
	wxWrapSizer* const panelSizer = new wxWrapSizer();
	const wxSizerFlags sflags     = wxSizerFlags().Expand().Border( wxLEFT );

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Matroska container attachments" ), wxVERTICAL );

		{
			wxGridBagSizer* const innerSizer = new wxGridBagSizer();
			wxGBSpan              oneCol(1, 2);

			innerSizer->Add( create_static_text( sizer, _( "Cue scheet attach mode" ) ), wxGBPosition(0,0), wxDefaultSpan, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			{
				const std::array< wxString, 5 > elems{ _( "default" ), _( "none" ), _( "source" ), _( "decoded" ), _( "rendered" ) };
				m_comboBoxCueSheetAttachMode = create_ro_combobox( sizer->GetStaticBox(), elems );
			}
			innerSizer->Add( m_comboBoxCueSheetAttachMode, wxGBPosition(0,1), wxDefaultSpan, wxEXPAND|wxBOTTOM,  btnLeft.GetBorderInPixels());


			m_checkBoxAttachLogs = create_3state_checkbox( sizer, _( "Attach EAC log(s)" ), wxCHK_CHECKED );
			innerSizer->Add( m_checkBoxAttachLogs, wxGBPosition(1,0), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			m_checkBoxAttachAccuRip = create_3state_checkbox( sizer, _( "Attach AccurateRip log(s)" ) );
			innerSizer->Add( m_checkBoxAttachAccuRip, wxGBPosition(2,0), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels() );

			m_checkBoxAttachCover = create_3state_checkbox( sizer, _( "Attach cover image" ), wxCHK_CHECKED );
			innerSizer->Add( m_checkBoxAttachCover, wxGBPosition(3,0), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels());

			m_checkBoxCoverJpeg = create_3state_checkbox( sizer, _( "Convert covers to JPEG format" ) );
			innerSizer->Add( m_checkBoxCoverJpeg, wxGBPosition(4,0), oneCol, btnLeft.GetFlags(), btnLeft.GetBorderInPixels());

			const CheckBoxUiUpdater uiUpdater( m_checkBoxCoverJpeg );

			wxStaticText* const staticText = create_static_text( sizer, _( "JPEG quality" ) );
			staticText->Bind( wxEVT_UPDATE_UI, uiUpdater );
			innerSizer->Add( staticText,wxGBPosition(5,0), wxDefaultSpan, wxLEFT|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, btnLeft.GetBorderInPixels());

			m_textCtrlJpegQuality = create_text_ctrl( sizer, "75", 3 );
			m_textCtrlJpegQuality->Bind( wxEVT_UPDATE_UI, uiUpdater );
			innerSizer->Add( m_textCtrlJpegQuality, wxGBPosition(5,1), wxDefaultSpan, wxEXPAND|wxTOP|wxBOTTOM, btnLeft.GetBorderInPixels() );

			sizer->Add( innerSizer, wxSizerFlags().Expand() );
		}

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "File extensions" ), wxVERTICAL );

		{
			wxFlexGridSizer* const innerSizer = new wxFlexGridSizer( 0, 2, btnLeft.GetBorderInPixels(), 0 );
			innerSizer->AddGrowableCol( 1 );

			wxSizerFlags txtSizerFlags;
			txtSizerFlags.Expand().Proportion( 1 );

			m_checkBoxExtCue = create_checkbox( sizer, _( "Cue sheet" ) );
			innerSizer->Add( m_checkBoxExtCue, centerVertical );

			m_textCtrlExtCue = create_text_ctrl( sizer, "cue", 16 );
			m_textCtrlExtCue->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxExtCue ) );
			innerSizer->Add( m_textCtrlExtCue, txtSizerFlags );

			m_checkBoxExtMatroskaChapters = create_checkbox( sizer, _( "Matroska chapters" ) );
			innerSizer->Add( m_checkBoxExtMatroskaChapters, centerVertical );

			m_textCtrlExtMatroskaChapters = create_text_ctrl( sizer, "mkc.xml", 16 );
			m_textCtrlExtMatroskaChapters->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxExtMatroskaChapters ) );
			innerSizer->Add( m_textCtrlExtMatroskaChapters, txtSizerFlags );

			m_checkBoxExtMatroskaTags = create_checkbox( sizer, _( "Matroska tags" ) );
			innerSizer->Add( m_checkBoxExtMatroskaTags, centerVertical );

			m_textCtrlExtMatroskaTags = create_text_ctrl( sizer, "mkt.xml", 16 );
			m_textCtrlExtMatroskaTags->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxExtMatroskaTags ) );
			innerSizer->Add( m_textCtrlExtMatroskaTags, txtSizerFlags );

			m_checkBoxExtMkvmergeOptions = create_checkbox( sizer, _( "mkvmerge options" ) );
			innerSizer->Add( m_checkBoxExtMkvmergeOptions, centerVertical );

			m_textCtrlExtMkvmergeOptions = create_text_ctrl( sizer, "json", 16 );
			m_textCtrlExtMkvmergeOptions->Bind( wxEVT_UPDATE_UI, CheckBoxUiUpdater( m_checkBoxExtMkvmergeOptions ) );
			innerSizer->Add( m_textCtrlExtMkvmergeOptions, txtSizerFlags );

			sizer->Add( innerSizer, 0, wxEXPAND );
		}

		panelSizer->Add( sizer, sflags );
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Tags processing"), wxVERTICAL);

		m_checkBoxParseTagsFromCuesheetComments = create_3state_checkbox(sizer, _("Parse tags from cuesheet comments"));
		sizer->Add(m_checkBoxParseTagsFromCuesheetComments);

		m_checkBoxReadMedatata = create_3state_checkbox(sizer, _("Read metadata from media files"));
		sizer->Add(m_checkBoxReadMedatata);

		m_checkBoxIncludeDiscNumberTag = create_3state_checkbox(sizer, _("Copy/generate DISCNUMBER tag"));
		m_checkBoxIncludeDiscNumberTag->SetToolTip("Copy (or genetate in merge mode) DISCNUMBER and TOTALDISC tags");
		sizer->Add(m_checkBoxIncludeDiscNumberTag);

		m_checkBoxRenderMultilineTags = create_3state_checkbox(sizer, _("Copy multiline tags"));
		m_checkBoxRenderMultilineTags->SetToolTip(_("Copy multiline tags to Matroska container"));
		sizer->Add(m_checkBoxRenderMultilineTags);

		m_checkBoxRenderReplayGainTags = create_3state_checkbox(sizer, _("Copy ReplayGain tags"));
		m_checkBoxRenderReplayGainTags->SetToolTip(_("Copy ReplayGain tags to Matroska container"));
		sizer->Add(m_checkBoxRenderReplayGainTags);

		m_checkBoxGenerateArtistTagsForTracks = create_3state_checkbox(sizer, _("Generate ARTIST tag for every track"), wxCHK_CHECKED);
		m_checkBoxGenerateArtistTagsForTracks->SetToolTip(_("Some media players (e.g. Foobar2000) requires ARTIST tag for every track (chapter)."));
		sizer->Add(m_checkBoxGenerateArtistTagsForTracks);

		panelSizer->Add(sizer, sflags);
	}

	{
		wxStaticBoxSizer* const sizer = create_static_box_sizer( panel, _( "Other" ), wxVERTICAL );

		m_checkBoxGenerateEditionUid = create_3state_checkbox( sizer, _( "Generate edition UID" ) );
		sizer->Add( m_checkBoxGenerateEditionUid );

		m_checkBoxFullPathInOptions = create_3state_checkbox( sizer, _( "Generate full paths in options file" ) );
		sizer->Add( m_checkBoxFullPathInOptions );

		m_checkBoxUseMLang = create_3state_checkbox( sizer, _( "Use MLang library" ) );
		sizer->Add( m_checkBoxUseMLang );

		panelSizer->Add( sizer, sflags );
	}

	panel->SetSizerAndFit( panelSizer );
	return panel;
}

wxPanel* wxMainFrame::create_messages_panel( wxNotebook* notebook, const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnRight, const wxSizerFlags& centerVertical )
{
	wxPanel* const    panel      = new wxPanel( notebook );
	wxBoxSizer* const panelSizer = new wxBoxSizer( wxVERTICAL );

	m_listBoxMessages = new ListBox( panel );
	panelSizer->Add( m_listBoxMessages, 1, wxEXPAND | wxALL, wxSizerFlags::GetDefaultBorder() );

	{
		wxBoxSizer* const sizer = new wxBoxSizer( wxHORIZONTAL );

		{
			wxBoxSizer* const innerSizer = new wxBoxSizer(wxVERTICAL);

			m_checkBoxAutoScroll = create_checkbox(panel, _("Auto scroll"), m_autoScroll );
			m_checkBoxAutoScroll->SetFont(toolFont);
			m_checkBoxAutoScroll->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
			m_checkBoxAutoScroll->Bind(wxEVT_CHECKBOX, &wxMainFrame::OnCheckAutoScroll, this);
			innerSizer->Add(m_checkBoxAutoScroll);

			m_checkBoxShowTimestamps = create_checkbox(panel, _("Show timestamps"));
			m_checkBoxShowTimestamps->SetFont(toolFont);
			m_checkBoxShowTimestamps->SetToolTip(_("Show/hide message timestamps"));
			m_checkBoxShowTimestamps->Bind(wxEVT_CHECKBOX, &wxMainFrame::OnCheckShowTimestamps, this);
			m_checkBoxShowTimestamps->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
			innerSizer->Add(m_checkBoxShowTimestamps);

			sizer->Add(innerSizer, centerVertical);
		}

		sizer->Add(0, 0, 1, wxEXPAND);

		m_staticBoxMsgCnt = create_static_text(panel, wxEmptyString );
		m_staticBoxMsgCnt->SetFont(toolFont);
		m_staticBoxMsgCnt->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMsgCnt, this);
		sizer->Add(m_staticBoxMsgCnt, centerVertical );

		sizer->Add( 0, 0, 1, wxEXPAND );

		m_buttonCopy = create_button( panel, _( "Copy" ) );
		m_buttonCopy->SetFont( toolFont );
		m_buttonCopy->SetToolTip(_("Copy all messages to clipboard"));
		m_buttonCopy->Bind( wxEVT_BUTTON, &wxMainFrame::OnCopyEvents, this );
		m_buttonCopy->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMsgCtrls, this);
		sizer->Add(m_buttonCopy, wxSizerFlags(btnRight).Top());

		panelSizer->Add( sizer, 0, wxEXPAND| wxBOTTOM | wxLEFT | wxRIGHT, wxSizerFlags::GetDefaultBorder() );
	}

	panel->SetSizerAndFit( panelSizer );
	return panel;
}

wxNotebook* wxMainFrame::create_notebook( const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnMiddleExp, const wxSizerFlags& btnRight, const wxSizerFlags& centerVertical )
{
	wxNotebook* const notebook = new wxNotebook( this, wxID_ANY );

	notebook->AddPage( create_src_dst_pannel( notebook, toolFont, btnLeft, btnMiddle, btnMiddleExp, btnRight ), _( "Source and destination" ), true );
	notebook->AddPage( create_general_panel( notebook, btnLeft, centerVertical ), _( "General options" ) );
	notebook->AddPage( create_chapter_panel( notebook, btnLeft, btnMiddle, centerVertical ), _( "Chapters and formatting" ) );
	notebook->AddPage( create_adv_panel( notebook, btnLeft, centerVertical ), _( "Advanced options" ) );
	notebook->AddPage( create_messages_panel( notebook, toolFont, btnLeft, btnMiddle, btnRight, centerVertical ), _( "Messages" ) );

	return notebook;
}

wxBoxSizer* wxMainFrame::create_bottom_ctrls( const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnRight, const wxSizerFlags& centerVertical )
{
	wxBoxSizer* const sizer = new wxBoxSizer( wxHORIZONTAL );

	{
		wxBoxSizer* const innerSizer = new wxBoxSizer( wxVERTICAL );

		m_checkBoxVerbose = create_checkbox( this, _( "Verbose mode" ), wxLog::GetVerbose() );
		m_checkBoxVerbose->SetFont( toolFont );
		m_checkBoxVerbose->SetToolTip( _( "Run cue2mkc in verbose mode" ) );
		m_checkBoxVerbose->Bind( wxEVT_CHECKBOX, &wxMainFrame::OnCheckVerbose, this );
		innerSizer->Add( m_checkBoxVerbose );

		m_checkBoxSwitchToMessagesPane = create_checkbox( this, _( "Switch to messages pane" ), true );
		m_checkBoxSwitchToMessagesPane->SetFont( toolFont );
		m_checkBoxSwitchToMessagesPane->SetToolTip( _( "Switch to Messages pane before cue2mkc execution" ) );
		innerSizer->Add( m_checkBoxSwitchToMessagesPane );

		sizer->Add( innerSizer, centerVertical );
	}

	sizer->Add( 0, 0, 4, wxEXPAND );

	{
		wxBoxSizer* const innerSizer = new wxBoxSizer( wxHORIZONTAL );

		{
			std::array< wxString, 4 > elems{ _( "help" ), _( "version" ), _( "list-languages" ), _( "list-types" ) };
			m_comboBoxMkvmergeSubject = create_ro_combobox( this, elems );
		}

		m_comboBoxMkvmergeSubject->SetToolTip( _( "Mkvmerge information subject" ) );
		m_comboBoxMkvmergeSubject->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this );
		innerSizer->Add( m_comboBoxMkvmergeSubject, btnLeft );

		wxButton* pButton = create_button( this, _( "\u25C4\u2002mkvmerge" ) );
		pButton->SetToolTip( _( "Execute mkvmerge with specified option" ) );
		pButton->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this );
		pButton->Bind( wxEVT_BUTTON, &wxMainFrame::OnExeMkvmerge, this );
		innerSizer->Add( pButton, btnRight );

		sizer->Add( innerSizer, wxSizerFlags( centerVertical ).Border( wxLEFT | wxRIGHT ) );
	}

	{
		wxBoxSizer* const innerSizer = new wxBoxSizer( wxHORIZONTAL );

		{
			std::array< wxString, 5 > elems{ _("help"), _("version"), _("usage"), _("formatting"), _("license")};
			m_comboBoxInfoSubject = create_ro_combobox( this, elems );
		}

		m_comboBoxInfoSubject->SetToolTip( _( "Information subject" ) );
		innerSizer->Add( m_comboBoxInfoSubject, btnLeft );
		m_comboBoxInfoSubject->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this );

		m_buttonExecInfo = create_button( this, _( "\u25C4\u2002cue2mkc" ) );
		m_buttonExecInfo->SetToolTip( _( "Execute cue2mkc with \"--info\" (or \"--help\") option" ) );
		m_buttonExecInfo->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this );
		m_buttonExecInfo->Bind( wxEVT_BUTTON, &wxMainFrame::OnExecInfoCue2Mkc, this );
		innerSizer->Add(m_buttonExecInfo, btnMiddle);

		sizer->Add( innerSizer, centerVertical );
	}

	sizer->Add( 0, 0, 1, wxEXPAND );

	m_buttonExec = new wxButton( this, wxID_ANY, _( m_execButtonCaptionRun ) );
	m_buttonExec->SetFont( wxFont( wxNORMAL_FONT->GetPointSize() + 1, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD ) );
	m_buttonExec->SetToolTip( _( "Execute (or kill) cue2mkc utility" ) );
	m_buttonExec->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateButtonRun, this );
	m_buttonExec->Bind( wxEVT_BUTTON, &wxMainFrame::OnExecCue2Mkc, this );
	sizer->Add( m_buttonExec, centerVertical );

	return sizer;
}

wxMainFrame::wxMainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style )
	: wxFrame( parent, id, title, pos, size, style ),
	m_pPrevLog( nullptr ),
	m_logTimestamp( wxLog::GetTimestamp() ),
	m_autoScroll(true),
	m_execButtonCaptionRun(_("Run")),
	m_execButtonCaptionKill(_("Kill"))
{
	SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_FRAMEBK ) );
	SetIcon( wxICON( 0 ) );

	{
		const wxFont toolFont( wxNORMAL_FONT->GetPointSize() - 1, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

		const wxSizerFlags btnLeft( get_left_ctrl_sizer_flags(this) );
		const wxSizerFlags btnMiddle( get_middle_crtl_sizer_flags(this) );
		const wxSizerFlags btnMiddleExp( get_middle_exp_crtl_sizer_flags(this ) );
		const wxSizerFlags btnRight( get_right_crtl_sizer_flags(this) );
		const wxSizerFlags centerVertical( get_vertical_allign_sizer_flags() );

		wxBoxSizer* const sizer = new wxBoxSizer( wxVERTICAL );
		sizer->Add( m_notebook = create_notebook( toolFont, btnLeft, btnMiddle, btnMiddleExp, btnRight, centerVertical ), 1, wxEXPAND );
		sizer->Add( create_bottom_ctrls( toolFont, btnLeft, btnMiddle, btnRight, centerVertical ), 0, wxEXPAND | wxALL, 4 );
		this->SetSizerAndFit( sizer );
	}

	m_pLog.reset( new LogListBox( m_listBoxMessages ) );
	m_pNoScrollLog.reset(new SimpleLogListBox(m_listBoxMessages));

	m_pPrevLog = wxLog::SetActiveTarget( m_autoScroll? m_pLog.get() : m_pNoScrollLog.get() );
	wxLog::DisableTimestamp();
	wxLog::EnableLogging();

	wxLogInfo( _( "Simple frontend to cue2mkc utility" ) );
	wxLogInfo( _( "Version: %s" ), wxGetApp().APP_VERSION );
	wxLogInfo( _( "Author: %s" ), wxGetApp().GetVendorDisplayName() );
	wxLogInfo( _( "Operating system: %s" ), wxPlatformInfo::Get().GetOperatingSystemDescription() );
	wxLogInfo( _( "Compiler: %s %s" ), INFO_CXX_COMPILER_ID, INFO_CXX_COMPILER_VERSION );
	wxLogInfo( _( "Compiled on: %s %s (%s)" ), INFO_HOST_SYSTEM_NAME, INFO_HOST_SYSTEM_VERSION, INFO_HOST_SYSTEM_PROCESSOR );

	{
		const wxString isExec( wxGetApp().GetCue2MkcPath().IsFileExecutable() ? "[exists]" : "[not exists]" );

		wxLogMessage( _( "cue2mkc: %s %s" ), wxGetApp().GetCue2MkcPath().GetFullPath(), isExec );
	}

	{
		const wxString isExec( wxGetApp().GetMkvmergePath().IsFileExecutable() ? "[exists]" : "[not exists]" );

		wxLogMessage( _( "mkvmerge: %s %s" ), wxGetApp().GetMkvmergePath().GetFullPath(), isExec );
	}

	Bind( wxEVT_CLOSE_WINDOW, &wxMainFrame::OnClose, this );
	m_timerIdleWakeUp.Bind( wxEVT_TIMER, &wxMainFrame::OnIdleWakeupTimer, this );
	m_timerAutoScroll.Bind(wxEVT_TIMER, &wxMainFrame::OnAutoScrollTimer, this);

	SetDropTarget( new DropTarget( this ) );
}

void wxMainFrame::OnCopyEvents( wxCommandEvent& WXUNUSED( event ) )
{
	const int cnt = m_listBoxMessages->GetCount();

	if ( cnt == 0 ) return;

	wxTextOutputStreamOnString tos;

	for(int i=0; i<cnt; ++i)
	{
		*tos << m_listBoxMessages->GetString(i) << endl;
	}

	if ( wxTheClipboard->Open() )
	{
		tos->Flush();
		wxTheClipboard->SetData( new wxTextDataObject( tos.GetString() ) );
		wxTheClipboard->Close();
	}
	else
	{
		wxLogWarning( _( "Cannot copy data to clipboard" ) );
	}
}

void wxMainFrame::ExecuteCmd( const wxString& cmd, const wxString& cwd )
{
	if ( m_pProcess )
	{
		wxLogWarning( _( "exe: Unable to execute command %s" ), cmd );
		return;
	}

	if ( m_checkBoxSwitchToMessagesPane->GetValue() ) m_notebook->ChangeSelection( m_notebook->GetPageCount() - 1 );

	wxExecuteEnv env;

	env.cwd = cwd;

	MyProcess* const pProcess = new MyProcess;

	m_pProcess.reset( pProcess );
	m_pProcess->Bind( wxEVT_END_PROCESS, &wxMainFrame::OnProcessTerminated, this );

	long pid = wxExecute( cmd, wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER, m_pProcess.get(), &env );

	if ( pid == 0 )
	{
		wxLogError( _( "exe[b]: fail: %s" ), cmd );
		m_pProcess.reset();
		return;
	}

	m_listBoxMessages->Clear();

	wxLogInfo( _( "exe[b]: pid: %ld, cmd: %s" ), pid, cmd );

	//m_pProcess->CloseOutput();
	pProcess->CreateTxtStreams();
	wxLog::SetActiveTarget(m_pNoScrollLog.get());
	Bind( wxEVT_IDLE, &wxMainFrame::OnIdle, this );

	m_timerIdleWakeUp.Start( TIMER_IDLE_WAKE_UP_INTERVAL );
	if (m_autoScroll) m_timerAutoScroll.Start( AUTO_SCROLL_UPDATE_INTERVAL );
}

#ifdef __WXMSW__
void wxMainFrame::ExecuteTaskKill()
{
	if (!m_pProcess)
	{
		return;
	}

	if (m_pTaskKillProcess)
	{
		return;
	}

	const long pid = m_pProcess->GetPid();
	if (!wxProcess::Exists(pid))
	{
		return;
	}

	const wxString cmd = wxString::Format("taskkill.exe /pid %ld", pid);
	wxProcess* const process = new wxProcess();
	process->SetPriority(wxPRIORITY_MIN);
	m_pTaskKillProcess.reset(process);
	process->Bind(wxEVT_END_PROCESS, &wxMainFrame::OnTaskKillProcessTerminated, this);

	const long killPid = wxExecute(cmd, wxEXEC_HIDE_CONSOLE, process);

	if (killPid == 0)
	{
		wxLogError(_("kill[b]: fail: %s"), cmd);
		m_pTaskKillProcess.reset();
		return;
	}

	wxLogInfo(_("kill[b]: pid: %ld, cmd: %s"), killPid, cmd);
}

void wxMainFrame::OnTaskKillProcessTerminated(wxProcessEvent& event)
{
	if (m_pTaskKillProcess->GetPid() != event.GetPid())
	{
		wxLogError(wxT("kill[unk]: pid: %d, exit code: %d [%08x]"), event.GetPid(), event.GetExitCode(), event.GetExitCode());
		return;
	}

	if (event.GetExitCode() != 0)
	{
		wxLogInfo(_("kill[f]: pid: %d, exit code: %d [%08x]"), event.GetPid(), event.GetExitCode(), event.GetExitCode());
	}
	m_pTaskKillProcess.reset();
}

#endif


void wxMainFrame::ExecuteCue2Mkc( const wxArrayString& args, bool withVerbose )
{
	wxArrayString params;

	if ( withVerbose && wxLog::GetVerbose() ) params.Add( "--verbose" );

	negatable_long_switch_option( params, m_checkBoxShowTimestamps, "log-timestamps" );
	params.insert( params.end(), args.begin(), args.end() );
	ExecuteCue2Mkc( options_to_str( params ), false );
}

void wxMainFrame::ExecuteCue2Mkc( const wxString& args, bool withVerbose )
{
	const wxString toolPath( wxGetApp().GetCue2MkcPath().GetPath() );
	const wxString toolName( wxGetApp().GetCue2MkcPath().GetFullPath() );

	if ( withVerbose && wxLog::GetVerbose() ) ExecuteCmd( wxString::Format( "\"%s\" --verbose %s", toolName, args ), toolPath );
	else ExecuteCmd( wxString::Format( "\"%s\" %s", toolName, args ), toolPath );
}

void wxMainFrame::ExecuteMkvmerge( const wxString& args )
{
	const wxString toolPath( wxGetApp().GetMkvmergePath().GetPath() );
	const wxString toolName( wxGetApp().GetMkvmergePath().GetFullPath() );

	ExecuteCmd( wxString::Format( "\"%s\" %s", toolName, args ), toolPath );
}

void wxMainFrame::ExecuteMkvmerge( const wxArrayString& args )
{
	wxArrayString params;

	params.Add( "--ui-language" );
	params.Add( "en" );

	params.Add( "--output-charset" );
	params.Add( "utf-8" );

	params.insert( params.end(), args.begin(), args.end() );

	ExecuteMkvmerge( options_to_str( params ) );
}

void wxMainFrame::ProcessOutErr( bool once )
{
	wxASSERT( m_pProcess );
	const MyProcess* const pProcess = static_cast< MyProcess* >( m_pProcess.get() );

	bool         processOutErr = pProcess->HaveOutOrErr();
	const wxChar c             = once ? wxS( 'r' ) : wxS( 'f' );

	while ( processOutErr )
	{
		if ( pProcess->IsInputAvailable() && !pProcess->GetTxtInputStream().GetInputStream().Eof() )
		{
			const wxString line = pProcess->GetTxtInputStream().ReadLine();
			wxLogInfo( "out[%c]: %s", c, line );
		}

		if ( pProcess->IsErrorAvailable() && !pProcess->GetTxtErrorStream().GetInputStream().Eof() )
		{
			const wxString line = pProcess->GetTxtErrorStream().ReadLine();
			wxLogInfo( "err[%c]: %s", c, line );
		}

		processOutErr = once ? false : pProcess->HaveOutOrErr();
	}
}

void wxMainFrame::OnProcessTerminated( wxProcessEvent& event )
{
	if ( m_pProcess->GetPid() != event.GetPid() )
	{
		wxLogError( wxT( "exe[unk]: pid: %d, exit code: %d [%08x]" ), event.GetPid(), event.GetExitCode(), event.GetExitCode() );
		return;
	}

	m_timerIdleWakeUp.Stop();
	m_timerAutoScroll.Stop();

	ProcessOutErr();

	wxLog::SetActiveTarget(m_autoScroll ? m_pLog.get() : m_pNoScrollLog.get());

	wxLogInfo( _( "exe[f]: pid: %d, exit code: %d [%08x]" ), event.GetPid(), event.GetExitCode(), event.GetExitCode() );
	m_pProcess.reset();
	Unbind( wxEVT_IDLE, &wxMainFrame::OnIdle, this );
}

void wxMainFrame::OnIdleWakeupTimer( wxTimerEvent& WXUNUSED( event ) )
{
	if ( !m_pProcess ) return;

	wxWakeUpIdle();
}

void wxMainFrame::OnAutoScrollTimer(wxTimerEvent& WXUNUSED(event) )
{
	m_listBoxMessages->ShowLastItem();
}

void wxMainFrame::OnIdle( wxIdleEvent& event )
{
	if ( !m_pProcess ) return;

	ProcessOutErr( true );

	MyProcess* const pProcess = static_cast< MyProcess* >( m_pProcess.get() );

	event.RequestMore( pProcess->HaveOutOrErr() );
}

void wxMainFrame::OnUpdateButtonRun( wxUpdateUIEvent& event )
{
	if ( m_pProcess )
	{
#ifdef __WXMSW__
		event.Enable( !m_pTaskKillProcess );
#else
		event.Enable(true);
#endif
		event.SetText(m_execButtonCaptionKill);
		return;
	}

	event.SetText(m_execButtonCaptionRun);
	if ( !m_treeCtrlInputFiles->HasChildren( m_treeCtrlInputFilesRoot ) )
	{
		event.Enable( false );
		return;
	}

	if ( m_comboBoxDst->GetSelection() == 0 ) event.Enable( true );
	else event.Enable( !m_textCtrlDst->IsEmpty() );
}

void wxMainFrame::OnUpdateRunUiCtrl( wxUpdateUIEvent& event )
{
	event.Enable( !m_pProcess );
}

void wxMainFrame::OnUpdateMsgCtrls(wxUpdateUIEvent& event)
{
	event.Enable(!m_pProcess && (m_listBoxMessages->GetCount() > 0));
}

bool wxMainFrame::read_options( wxArrayString& options ) const
{
	negatable_switch_option( options, m_checkBoxAbortOnErrors, 'a' );
	negatable_switch_option( options, m_checkBoxMergeMode, 'j' );
	negatable_switch_option( options, m_checkBoxIncludeDiscNumberTag, "dn" );
	negatable_switch_option( options, m_checkBoxUseMediaFiles, "df" );
	negatable_long_switch_option( options, m_checkBoxMediaFilesWithoutCue, "single-media-file" );

	if ( !m_textCtrlExt->IsEmpty() )
	{
		options.Add( "-x" );
		options.Add( m_textCtrlExt->GetValue() );
	}

	options.Add( "-m" );
	switch ( m_comboBoxFormat->GetSelection() )
	{
		case 0:
		{
			options.Add( "cuesheet" );
			break;
		}

		case 1:
		{
			options.Add( "matroska" );
			break;
		}

		case 2:
		{
			options.Add( "wav2img" );	// deprecated
			break;
		}
	}

	negatable_switch_option( options, m_checkBoxGenerateTagsFile, 't' );
	negatable_switch_option( options, m_checkBoxGenerateOptionsFile, 'k' );

	if ( !m_textCtrlLang->IsEmpty() )
	{
		const wxString lang = m_textCtrlLang->GetValue().Lower();

		if ( lang.Cmp( "und" ) != 0 )
		{
			options.Add( "-l" );
			options.Add( lang );
		}
	}

	negatable_long_switch_option( options, m_checkBoxRunMkvmerge, "run-mkvmerge" );

	options.Add( "-e" );
	switch ( m_comboBoxEncoding->GetSelection() )
	{
		case 0:
		{
			options.Add( "default" );
			break;
		}

		case 1:
		{
			options.Add( "utf8" );
			break;
		}

		case 2:
		{
			options.Add( "utf8bom" );
			break;
		}

		case 3:
		{
			options.Add( "utf16" );
			break;
		}

		case 4:
		{
			options.Add( "utf16bom" );
			break;
		}

		case 5:
		{
			options.Add( "utf16be" );
			break;
		}

		case 6:
		{
			options.Add( "utf16bebom" );
			break;
		}
	}

	negatable_switch_option( options, m_checkBoxCorrectSimpleQuotationMarks, "cq" );
	negatable_switch_option( options, m_checkBoxParseTagsFromCuesheetComments, "tc" );
	negatable_switch_option( options, m_checkBoxEllipsizeTags, "et" );
	negatable_switch_option( options, m_checkBoxRemoveExtraSpaces, "rs" );
	negatable_long_switch_option( options, m_checkBoxCorrectDashes, "correct-dashes" );
	negatable_long_switch_option( options, m_checkBoxSmallEmDash, "small-em-dash" );
	negatable_long_switch_option(options, m_checkBoxNumberFullStop, "number-full-stop");
	negatable_long_switch_option(options, m_checkBoxSmallLetterParenthesized, "small-letter-parenthesized");

	negatable_switch_option( options, m_checkBoxCapitalizedRomanLiterals, "ru" );
	negatable_switch_option( options, m_checkBoxLowercaseRomanLiterals, "rl" );

	negatable_long_switch_option( options, m_checkBoxUseCdTextTags, "use-cdtext-tags" );
	negatable_long_switch_option( options, m_checkBoxUseTagsFromCuesheetComments, "use-cue-comments-tags" );
	negatable_long_switch_option( options, m_checkBoxUseTagsFromMediaFiles, "use-media-tags" );

	negatable_switch_option( options, m_checkBoxGenerateEditionUid, "eu" );
	negatable_long_switch_option( options, m_checkBoxReadMedatata, "read-media-tags" );
	negatable_switch_option( options, m_checkBoxRenderMultilineTags, "rm" );
	negatable_switch_option(options, m_checkBoxRenderReplayGainTags, "rg");
	negatable_long_switch_option( options, m_checkBoxUseMLang, "use-mlang" );
	negatable_switch_option( options, m_checkBoxGenerateArtistTagsForTracks, "ra" );

	negatable_switch_option( options, m_checkBoxChapterEndTime, "ce" );

	if ( negatable_switch_option( options, m_checkBoxChapterEndTimeFromNext, "cn" ) )
	{
		if ( !m_textCtrlChapterOffset->IsEmpty() )
		{
			int offset;

			if ( m_textCtrlChapterOffset->GetValue().ToInt( &offset ) )
			{
				options.Add( "" );
				options.Add( m_textCtrlChapterOffset->GetValue() );
			}
			else
			{
				wxLogError( "Invalid chapter offset value" );
				return false;
			}
		}
	}

	switch ( m_comboBoxIdx->GetSelection() )
	{
		case 0:
		{
			options.Add( "-t1i0" );
			break;
		}

		// case 1:
		// options.Add("-t1i0-");
		// break;
	}

	negatable_long_switch_option( options, m_checkBoxIdxToHiddenChapters, "hidden-indexes" );

	if ( is_checked( m_checkBoxTrackTilteFmt ) && !m_textCtrlTrackTilteFmt->IsEmpty() )
	{
		options.Add( "-f" );
		options.Add( m_textCtrlTrackTilteFmt->GetValue() );
	}

	if ( is_checked( m_checkBoxMkaTitleFmt ) && !m_textCtrlMkaTitleFmt->IsEmpty() )
	{
		options.Add( "-mf" );
		options.Add( m_textCtrlMkaTitleFmt->GetValue() );
	}

	negatable_long_switch_option( options, m_checkBoxAttachLogs, "attach-eac-log" );
	negatable_long_switch_option( options, m_checkBoxAttachAccuRip, "attach-accurip-log" );
	negatable_long_switch_option( options, m_checkBoxAttachCover, "attach-cover" );

	if ( m_comboBoxCueSheetAttachMode->GetSelection() > 0 )
	{
		options.Add( "--cue-sheet-attach-mode" );
		options.Add( m_comboBoxCueSheetAttachMode->GetValue() );
	}

	if ( negatable_long_switch_option( options, m_checkBoxCoverJpeg, "convert-cover-to-jpeg" ) )
	{
		if ( !m_textCtrlJpegQuality->IsEmpty() )
		{
			int quality;

			if ( m_textCtrlJpegQuality->GetValue().ToInt( &quality ) )
			{
				if ( quality >= 0 && quality <= 100 )
				{
					options.Add( "--jpeg-image-quality" );
					options.Add( m_textCtrlJpegQuality->GetValue() );
				}
				else
				{
					wxLogError( "Wrong JPEG quality value - not in range" );
					return false;
				}
			}
			else
			{
				wxLogError( "Wrong JPEG quality value" );
				return false;
			}
		}
	}

	negatable_long_switch_option( options, m_checkBoxFullPathInOptions, "use-full-paths" );

	if ( is_checked( m_checkBoxExtCue ) && !m_textCtrlExtCue->IsEmpty() )
	{
		options.Add( "--cue-sheet-file-extension" );
		options.Add( m_textCtrlExtCue->GetValue() );
	}

	if ( is_checked( m_checkBoxExtMatroskaChapters ) && !m_textCtrlExtMatroskaChapters->IsEmpty() )
	{
		options.Add( "--matroska-chapters-file-extension" );
		options.Add( m_textCtrlExtMatroskaChapters->GetValue() );
	}

	if ( is_checked( m_checkBoxExtMatroskaTags ) && !m_textCtrlExtMatroskaTags->IsEmpty() )
	{
		options.Add( "--matroska-tags-file-extension" );
		options.Add( m_textCtrlExtMatroskaTags->GetValue() );
	}

	if ( is_checked( m_checkBoxExtMkvmergeOptions ) && !m_textCtrlExtMkvmergeOptions->IsEmpty() )
	{
		options.Add( "--mkvmerge-options-file-extension" );
		options.Add( m_textCtrlExtMkvmergeOptions->GetValue() );
	}

	switch ( m_comboBoxDst->GetSelection() )
	{
		case 1:
		{
			if ( !m_textCtrlDst->IsEmpty() )
			{
				options.Add( "-od" );
				options.Add( m_textCtrlDst->GetValue() );
			}
			break;
		}

		case 2:
		{
			if ( !m_textCtrlDst->IsEmpty() )
			{
				options.Add( "-o" );
				options.Add( m_textCtrlDst->GetValue() );
			}
			break;
		}
	}

	if ( !m_treeCtrlInputFiles->HasChildren( m_treeCtrlInputFilesRoot ) )
	{
		wxLogError( _( "No input files" ) );
		return false;
	}

	wxTreeItemIdValue cookie;

	for ( wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild( m_treeCtrlInputFilesRoot, cookie ); i.IsOk(); i = m_treeCtrlInputFiles->GetNextChild( i, cookie ) )
	{
		if ( m_treeCtrlInputFiles->HasChildren( i ) )
		{
			wxArrayString     mediaFiles;
			wxTreeItemIdValue mediaCookie;

			for ( wxTreeItemId j = m_treeCtrlInputFiles->GetFirstChild( m_treeCtrlInputFilesRoot, mediaCookie ); j.IsOk(); j = m_treeCtrlInputFiles->GetNextChild( j, mediaCookie ) )
			{
				mediaFiles.Add( m_treeCtrlInputFiles->GetItemText( j ) );
			}

			wxString s = m_treeCtrlInputFiles->GetItemText( i );
			s.Append( join_strings( mediaFiles, ';' ) );

			options.Add( s );
		}
		else
		{
			options.Add( m_treeCtrlInputFiles->GetItemText( i ) );
		}
	}

	return true;
}

void wxMainFrame::OnExecCue2Mkc( wxCommandEvent& WXUNUSED( event ) )
{
	if (m_pProcess)
	{
#ifdef __WXMSW__
		ExecuteTaskKill();
#else
		const wxKillError res = wxProcess::Kill(m_pProcess->GetPid(), wxSIGKILL, wxKILL_CHILDREN);
		if (res != wxKILL_OK)
		{
			wxLogWarning(_("Fail to kill process %ld - error %d"), m_pProcess->GetPid(), res);
		}
#endif
		return;
	}

	wxArrayString params;

	if ( !read_options( params ) ) return;

	ExecuteCue2Mkc( params );
}

void wxMainFrame::OnExecInfoCue2Mkc( wxCommandEvent& WXUNUSED( event ) )
{
	wxArrayString params;

	if (m_comboBoxInfoSubject->GetSelection() == 0)
	{
		params.Add("--help");
	}
	else
	{
		params.Add("--info");
		params.Add(m_comboBoxInfoSubject->GetStringSelection());
	}

	ExecuteCue2Mkc( params, false );
}

void wxMainFrame::OnCheckVerbose( wxCommandEvent& event )
{
	wxLog::SetVerbose( event.IsChecked() );
}

void wxMainFrame::OnCheckShowTimestamps( wxCommandEvent& event )
{
	if ( event.IsChecked() ) wxLog::SetTimestamp( m_logTimestamp );
	else wxLog::DisableTimestamp();
}

void wxMainFrame::AddMainItem( const wxString& fileName )
{
	wxTreeItemId itemId = m_treeCtrlInputFiles->AppendItem( m_treeCtrlInputFilesRoot, fileName );

	wxASSERT( itemId.IsOk() );
	m_treeCtrlInputFiles->SelectItem( itemId );
}

void wxMainFrame::SuggestCommonDirPath()
{
	if ( !m_checkBoxSuggestMerge->GetValue() ) return;

	if ( m_comboBoxDst->GetSelection() >= 2 ) return;

	wxArrayString     files;
	wxTreeItemIdValue cookie;

	for ( wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild( m_treeCtrlInputFilesRoot, cookie ); i.IsOk(); i = m_treeCtrlInputFiles->GetNextChild( i, cookie ) )
	{
		files.Add( m_treeCtrlInputFiles->GetItemText( i ) );
	}

	const wxString commonPath = find_common_path( files );

	if ( commonPath.IsEmpty() ) return;

	m_textCtrlDst->SetValue( commonPath );
}

void wxMainFrame::OnButtonAdd( wxCommandEvent& WXUNUSED( event ) )
{
	wxFileDialog openFileDialog( this,
								 _( "Specify input file" ),
								 wxEmptyString, wxEmptyString,
								 "CUE files|*.cue|Text files|*.txt|Audio files|*.flac;*.ape;*.wv;*.wav;*.aiff;*.tta|All files|*",
								 wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST );

	if ( openFileDialog.ShowModal() != wxID_OK ) return;

	wxFileName fileName;

	fileName.AssignDir( openFileDialog.GetDirectory() );

	wxArrayString fileNames;

	openFileDialog.GetFilenames( fileNames );
	for (wxArrayString::const_iterator i = fileNames.begin(); i != fileNames.end(); ++i)
	{
		fileName.SetFullName( *i );
		AddMainItem( fileName.GetFullPath() );
	}

	SuggestCommonDirPath();
}

void wxMainFrame::OnSuggestMergeMode( wxCommandEvent& event )
{
	if ( event.IsChecked() ) m_checkBoxMergeMode->Bind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMergeMode, this );
	else m_checkBoxMergeMode->Unbind( wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMergeMode, this );
}

void wxMainFrame::OnUpdateMergeMode( wxUpdateUIEvent& event )
{
	wxASSERT( m_checkBoxSuggestMerge->GetValue() );

	wxTreeItemIdValue cookie;
	int               cnt = 0;

	for (wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild( m_treeCtrlInputFilesRoot, cookie );
		 i.IsOk() && cnt < 2;
		 i = m_treeCtrlInputFiles->GetNextChild( i, cookie ), ++cnt)
	{}

	event.Check( cnt >= 2 );
}

void wxMainFrame::OnDropFiles( const wxArrayString& fileNames )
{
	m_notebook->ChangeSelection( 0 );

	for (wxArrayString::const_iterator i = fileNames.begin(); i != fileNames.end(); ++i)
	{
		AddMainItem( *i );
	}

	SuggestCommonDirPath();
}

void wxMainFrame::OnButtonDelete( wxCommandEvent& WXUNUSED( event ) )
{
	const wxTreeItemId itemId               = m_treeCtrlInputFiles->GetSelection();
	const bool         suggestCommonDirPath = ( m_treeCtrlInputFiles->GetItemParent( itemId ) == m_treeCtrlInputFilesRoot );

	if ( !itemId.IsOk() ) return;

	m_treeCtrlInputFiles->Delete( itemId );

	if ( !suggestCommonDirPath ) return;

	SuggestCommonDirPath();
}

void wxMainFrame::OnButtonMakeMask( wxCommandEvent& WXUNUSED( event ) )
{
	const wxTreeItemId itemId = m_treeCtrlInputFiles->GetSelection();

	if ( !itemId.IsOk() ) return;

	const wxString txt( make_fmask( m_treeCtrlInputFiles->GetItemText( itemId ) ) );

	m_treeCtrlInputFiles->SetItemText( itemId, txt );
}

void wxMainFrame::AddChildItem( const wxTreeItemId& parentId, const wxString& fileName )
{
	wxASSERT( parentId.IsOk() );
	wxASSERT( parentId != m_treeCtrlInputFilesRoot );

	wxTreeItemId childId = m_treeCtrlInputFiles->AppendItem( parentId, fileName );

	wxASSERT( childId.IsOk() );

	m_treeCtrlInputFiles->ExpandAllChildren( parentId );
	m_treeCtrlInputFiles->SelectItem( childId );
}

void wxMainFrame::OnButtonAddMediaFile( wxCommandEvent& WXUNUSED( event ) )
{
	const wxTreeItemId itemId = m_treeCtrlInputFiles->GetSelection();

	if ( !itemId.IsOk() ) return;

	const wxTreeItemId parentItem = m_treeCtrlInputFiles->GetItemParent( itemId );
	const wxTreeItemId cueItem    = ( parentItem == m_treeCtrlInputFilesRoot ) ? itemId : parentItem;

	wxFileName fileName( m_treeCtrlInputFiles->GetItemText( cueItem ) );

	wxFileDialog openFileDialog( this,
								 _( "Specify media file" ),
								 fileName.GetPath(),
								 wxEmptyString,
								 "Audio files|*.flac;*.ape;*.wv;*.wav;*.aiff;*.tta|All files|*",
								 wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST );

	if ( openFileDialog.ShowModal() != wxID_OK ) return;

	fileName.AssignDir( openFileDialog.GetDirectory() );

	wxArrayString fileNames;

	openFileDialog.GetFilenames( fileNames );
	for (wxArrayString::const_iterator i = fileNames.begin(); i != fileNames.end(); ++i)
	{
		fileName.SetFullName( *i );
		AddChildItem( cueItem, fileName.GetFullPath() );
	}
}

void wxMainFrame::OnUpdateDst( wxUpdateUIEvent& event )
{
	event.Enable( m_treeCtrlInputFiles->HasChildren( m_treeCtrlInputFilesRoot ) );
}

void wxMainFrame::OnUpdateCtrlDst( wxUpdateUIEvent& event )
{
	event.Enable( m_comboBoxDst->GetSelection() > 0 );
}

void wxMainFrame::OnChooseDst( wxCommandEvent& WXUNUSED( event ) )
{
	switch ( m_comboBoxDst->GetSelection() )
	{
		case 1:	// directory
		{
			int      extraFlags = 0;
			wxString initDir    = wxEmptyString;

			const wxString dir = m_textCtrlDst->GetValue();

			if ( !dir.IsEmpty() )
			{
				wxFileName fnDir = wxFileName::DirName( dir );

				if ( fnDir.DirExists() )
				{
					extraFlags |= wxDD_CHANGE_DIR;
					initDir     = fnDir.GetFullPath();
				}
			}

			wxDirDialog dlgDir( this,
								_( "Specify destination directory" ),
								initDir,
								wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | extraFlags );

			if ( dlgDir.ShowModal() == wxID_OK ) m_textCtrlDst->SetValue( dlgDir.GetPath() );
			break;
		}

		case 2:	// file
		{
			wxFileDialog dlgFile( this,
								  _( "Specify output file" ),
								  wxEmptyString,
								  wxEmptyString,
								  _( "XML files|*.xml|Matroska chapters XML files|*.mkc.xml|CUE files|*.cue|Text files|*.txt|All files|*" ),
								  wxFD_DEFAULT_STYLE | wxFD_SAVE );

			if ( dlgFile.ShowModal() == wxID_OK ) m_textCtrlDst->SetValue( dlgFile.GetPath() );
			break;
		}
	}
}

void wxMainFrame::OnExeMkvmerge( wxCommandEvent& WXUNUSED( event ) )
{
	wxArrayString args;

	wxString longOption = m_comboBoxMkvmergeSubject->GetStringSelection();

	longOption.Prepend( "--" );

	args.Add( longOption );

	ExecuteMkvmerge( args );
}

void wxMainFrame::OnUpdateMsgCnt(wxUpdateUIEvent& event)
{
	const int cnt = m_listBoxMessages->GetCount();
	wxString txt;
	if (m_pProcess)
	{
#ifdef __WXMSW__
		if (m_pTaskKillProcess)
		{
			txt = wxString::Format("pid:\u2009%ld [killed],\u2009", m_pProcess->GetPid());
		}
		else
		{
			txt = wxString::Format("pid:\u2009%ld,\u2009", m_pProcess->GetPid());
		}
#else
		txt = wxString::Format("pid:\u2009%ld,\u2009", m_pProcess->GetPid());
#endif
	}

	switch (cnt)
	{
	case 0:
		break;

	case 1:
		txt << _("one message");

	default:
		txt << cnt << _(" messages");
		break;
	}

	if (cnt >= 1000)
	{
		txt << "\u2009\u203C";
	}

	event.SetText(txt);
	event.Enable(!m_pProcess && (cnt > 0));
}

void wxMainFrame::OnCheckAutoScroll(wxCommandEvent& event)
{
	m_autoScroll = event.IsChecked();

	if (m_pProcess)
	{
		wxLog::SetActiveTarget(m_pNoScrollLog.get());
		if (m_autoScroll)
		{
			m_timerAutoScroll.Start(AUTO_SCROLL_UPDATE_INTERVAL);
		}
		else
		{
			m_timerAutoScroll.Stop();
		}
	}
	else
	{
		wxLog::SetActiveTarget(m_autoScroll ? m_pLog.get() : m_pNoScrollLog.get());
		if (!m_autoScroll && m_timerAutoScroll.IsRunning())
		{
			m_timerAutoScroll.Stop();
		}
	}

	if (m_autoScroll)
	{
		m_listBoxMessages->ShowLastItem();
	}
}

void wxMainFrame::OnClose( wxCloseEvent& event )
{
	if ( event.CanVeto() && m_pProcess )
	{
		wxLogWarning( _( "Vetoing window close request" ) );
		event.Veto();
		return;
	}

	if ( m_pProcess )
	{
#ifdef __WXMSW__
		kill_console_process(*m_pProcess);
#else
		wxLogWarning( _( "exe[c]: kill %ld" ), m_pProcess->GetPid() );
		const wxKillError res = wxProcess::Kill( m_pProcess->GetPid(), wxSIGKILL, wxKILL_CHILDREN);
		if (res != wxKILL_OK)
		{
			wxLogWarning(_("Fail to kill process %ld - error code %d"), m_pProcess->GetPid(), res);
		}
#endif
	}

#ifdef __WXMSW__
	if (m_pTaskKillProcess)
	{
		kill_console_process(*m_pTaskKillProcess);
#endif
	}

	if ( m_timerIdleWakeUp.IsRunning() ) m_timerIdleWakeUp.Stop();
	if (m_timerAutoScroll.IsRunning()) m_timerAutoScroll.Stop();

	wxLog::SetTimestamp( m_logTimestamp );
	wxLog::EnableLogging( false );
	wxLog::SetActiveTarget( m_pPrevLog );

	Destroy();
}

