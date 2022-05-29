/*
 *      wxMainFrame.h
 */

#pragma once

#ifndef _LOG_LIST_BOX_H_
#include "LogListBox.h"
#endif

class wxMainFrame:
	public wxFrame
{
	protected:

		wxNotebook* m_notebook;
		wxTreeCtrl* m_treeCtrlInputFiles;
		wxTreeItemId m_treeCtrlInputFilesRoot;
		wxCheckBox* m_checkBoxSuggestMerge;
		wxButton* m_buttonAdd;
		wxButton* m_buttonDelete;
		wxButton* m_buttonMakeMask;
		wxButton* m_buttonAddMediaFile;
		wxComboBox* m_comboBoxDst;
		wxTextCtrl* m_textCtrlDst;
		wxButton* m_buttonDstDir;
		wxCheckBox* m_checkBoxAbortOnErrors;
		wxCheckBox* m_checkBoxMergeMode;
		wxCheckBox* m_checkBoxUseMediaFiles;
		wxCheckBox* m_checkBoxMediaFilesWithoutCue;
		wxTextCtrl* m_textCtrlExt;
		wxComboBox* m_comboBoxFormat;
		wxCheckBox* m_checkBoxGenerateTagsFile;
		wxCheckBox* m_checkBoxGenerateOptionsFile;
		wxTextCtrl* m_textCtrlLang;
		wxCheckBox* m_checkBoxRunMkvmerge;
		wxComboBox* m_comboBoxEncoding;
		wxCheckBox* m_checkBoxCorrectSimpleQuotationMarks;
		wxCheckBox* m_checkBoxParseTagsFromCuesheetComments;
		wxCheckBox* m_checkBoxEllipsizeTags;
		wxCheckBox* m_checkBoxRemoveExtraSpaces;
		wxCheckBox* m_checkBoxCorrectDashes;
		wxCheckBox* m_checkBoxCapitalizedRomanLiterals;
		wxCheckBox* m_checkBoxLowercaseRomanLiterals;
		wxCheckBox* m_checkBoxNumberFullStop;
		wxCheckBox* m_checkBoxSmallLetterParenthesized;
		wxCheckBox* m_checkBoxUseCdTextTags;
		wxCheckBox* m_checkBoxUseTagsFromCuesheetComments;
		wxCheckBox* m_checkBoxUseTagsFromMediaFiles;
		wxCheckBox* m_checkBoxGenerateEditionUid;
		wxCheckBox* m_checkBoxReadMedatata;
		wxCheckBox* m_checkBoxUseMLang;
		wxCheckBox* m_checkBoxGenerateArtistTagsForTracks;
		wxCheckBox* m_checkBoxIncludeDiscNumberTag;
		wxCheckBox* m_checkBoxChapterEndTime;
		wxCheckBox* m_checkBoxChapterEndTimeFromNext;
		wxTextCtrl* m_textCtrlChapterOffset;
		wxComboBox* m_comboBoxIdx;
		wxCheckBox* m_checkBoxIdxToHiddenChapters;
		wxCheckBox* m_checkBoxTrackTilteFmt;
		wxTextCtrl* m_textCtrlTrackTilteFmt;
		wxCheckBox* m_checkBoxMkaTitleFmt;
		wxTextCtrl* m_textCtrlMkaTitleFmt;
		wxCheckBox* m_checkBoxAttachLogs;
		wxCheckBox* m_checkBoxAttachAccuRip;
		wxCheckBox* m_checkBoxAttachCover;
		wxComboBox* m_comboBoxCueSheetAttachMode;
		wxCheckBox* m_checkBoxCoverJpeg;
		wxTextCtrl* m_textCtrlJpegQuality;
		wxCheckBox* m_checkBoxFullPathInOptions;
		wxCheckBox* m_checkBoxSmallEmDash;
		wxCheckBox* m_checkBoxRenderMultilineTags;
		wxCheckBox* m_checkBoxRenderReplayGainTags;
		wxCheckBox* m_checkBoxExtCue;
		wxTextCtrl* m_textCtrlExtCue;
		wxCheckBox* m_checkBoxExtMatroskaChapters;
		wxTextCtrl* m_textCtrlExtMatroskaChapters;
		wxCheckBox* m_checkBoxExtMatroskaTags;
		wxTextCtrl* m_textCtrlExtMatroskaTags;
		wxCheckBox* m_checkBoxExtMkvmergeOptions;
		wxTextCtrl* m_textCtrlExtMkvmergeOptions;
		ListBox* m_listBoxMessages;
		wxCheckBox* m_checkBoxShowTimestamps;
		wxCheckBox* m_checkBoxAutoScroll;
		wxStaticText* m_staticBoxMsgCnt;
		wxButton* m_buttonCopy;
		wxCheckBox* m_checkBoxVerbose;
		wxCheckBox* m_checkBoxSwitchToMessagesPane;
		wxComboBox* m_comboBoxMkvmergeSubject;
		wxComboBox* m_comboBoxInfoSubject;
		wxButton* m_buttonExecInfo;
		wxButton* m_buttonExec;

	protected:

		void OnClose( wxCloseEvent& event );
		void OnCopyEvents( wxCommandEvent& event );
		void OnProcessTerminated( wxProcessEvent& event );
#ifdef __WXMSW__
		void OnTaskKillProcessTerminated(wxProcessEvent& event);
#endif
		void OnIdle( wxIdleEvent& event );
		void OnUpdateRunUiCtrl( wxUpdateUIEvent& event );
		void OnUpdateMsgCtrls(wxUpdateUIEvent& event);
		void OnUpdateButtonRun( wxUpdateUIEvent& event );
		void OnIdleWakeupTimer( wxTimerEvent& event );
		void OnAutoScrollTimer(wxTimerEvent& event);
		void OnExecCue2Mkc( wxCommandEvent& event );
		void OnExecInfoCue2Mkc( wxCommandEvent& event );
		void OnCheckVerbose( wxCommandEvent& event );
		void OnCheckShowTimestamps( wxCommandEvent& event );
		void OnButtonAdd( wxCommandEvent& event );
		void OnButtonDelete( wxCommandEvent& event );
		void OnButtonMakeMask( wxCommandEvent& event );
		void OnButtonAddMediaFile( wxCommandEvent& event );
		void OnUpdateDst( wxUpdateUIEvent& event );
		void OnUpdateCtrlDst( wxUpdateUIEvent& event );
		void OnChooseDst( wxCommandEvent& event );
		void OnSuggestMergeMode( wxCommandEvent& event );
		void OnUpdateMergeMode( wxUpdateUIEvent& event );
		void OnExeMkvmerge( wxCommandEvent& event );
		void OnUpdateMsgCnt(wxUpdateUIEvent& event);
		void OnCheckAutoScroll(wxCommandEvent& event);

	protected:

		void ExecuteCmd( const wxString& cmd, const wxString& cwd );
#ifdef __WXMSW__
		void ExecuteTaskKill();
#endif

		void ExecuteCue2Mkc( const wxString& args, bool withVerbose      = true );
		void ExecuteCue2Mkc( const wxArrayString& args, bool withVerbose = true );

		void ExecuteMkvmerge( const wxString& args );
		void ExecuteMkvmerge( const wxArrayString& args );

		void ProcessOutErr( bool once = false );

		void AddMainItem( const wxString& fileName );
		void AddChildItem( const wxTreeItemId& rootId, const wxString& fileName );

		void SuggestCommonDirPath();

	private:

		wxScopedPtr< wxLog > m_pLog;
		wxScopedPtr< wxLog > m_pNoScrollLog;
		wxLog* m_pPrevLog;

		wxScopedPtr< wxProcess > m_pProcess;
#ifdef __WXMSW__
		wxScopedPtr< wxProcess > m_pTaskKillProcess;
#endif
		wxTimer m_timerIdleWakeUp;
		wxTimer m_timerAutoScroll;
		bool m_autoScroll;
		wxString m_logTimestamp;

		wxString m_execButtonCaptionRun;
		wxString m_execButtonCaptionKill;

	private:

		wxPanel* create_src_dst_pannel( wxNotebook* notebook, const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnMiddleExp, const wxSizerFlags& btnRight );
		wxPanel* create_general_panel( wxNotebook* notebook, const wxSizerFlags& btnLeft, const wxSizerFlags& centerVertical );
		wxPanel* create_chapter_panel( wxNotebook* notebook, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& centerVertical );
		wxPanel* create_adv_panel( wxNotebook* notebook, const wxSizerFlags& btnLeft, const wxSizerFlags& centerVertical );
		wxPanel* create_messages_panel( wxNotebook* notebook, const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnRight, const wxSizerFlags& centerVertical );
		wxNotebook* create_notebook( const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnMiddleExp, const wxSizerFlags& btnRight, const wxSizerFlags& centerVertical );
		wxBoxSizer* create_bottom_ctrls( const wxFont& toolFont, const wxSizerFlags& btnLeft, const wxSizerFlags& btnMiddle, const wxSizerFlags& btnRight, const wxSizerFlags& centerVertical );
		bool read_options( wxArrayString& options ) const;

	public:

		wxMainFrame( wxWindow* parent = nullptr, wxWindowID id = wxID_ANY, const wxString& title = _("cue2mkc frontend"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxCLIP_SIBLINGS | wxTAB_TRAVERSAL );
		void OnDropFiles( const wxArrayString& fileNames );
};

