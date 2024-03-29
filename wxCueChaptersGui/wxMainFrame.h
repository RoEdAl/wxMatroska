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
    wxDECLARE_NO_COPY_CLASS(wxMainFrame);

    protected:

    wxNotebook* m_notebook;
    wxTreeCtrl* m_treeCtrlInputFiles;
    wxTreeItemId m_treeCtrlInputFilesRoot;
    wxCheckBox* m_checkBoxSuggestJoin;
    wxChoice* m_choiceDst;
    wxTextCtrl* m_textCtrlDst;
    wxCheckBox* m_checkBoxAbortOnErrors;
    wxCheckBox* m_checkBoxJoinMode;
    wxCheckBox* m_checkBoxMono;
    wxCheckBox* m_checkBoxUseMediaFiles;
    wxCheckBox* m_checkBoxMediaFilesWithoutCue;
    wxTextCtrl* m_textCtrlExt;
    wxChoice* m_choiceFormat;
    wxComboBox* m_textCtrlLang;
    wxCheckBox* m_checkBoxRunTool;
    wxChoice* m_choiceEncoding;
    wxCheckBox* m_checkBoxCorrectSimpleQuotationMarks;
    wxCheckBox* m_checkBoxParseTagsFromCuesheetComments;
    wxCheckBox* m_checkBoxEllipsizeTags;
    wxCheckBox* m_checkBoxRemoveExtraSpaces;
    wxCheckBox* m_checkBoxCorrectDashes;
    wxCheckBox* m_checkBoxCapitalizedRomanLiterals;
    wxCheckBox* m_checkBoxLowercaseRomanLiterals;
    wxCheckBox* m_checkBoxStrongRomanLiteralsParser;
    wxCheckBox* m_checkBoxNumberFullStop;
    wxCheckBox* m_checkBoxSmallLetterParenthesized;
    wxCheckBox* m_checkBoxAsciiToUnicode;
    wxCheckBox* m_checkBoxUseCdTextTags;
    wxCheckBox* m_checkBoxUseTagsFromCuesheetComments;
    wxCheckBox* m_checkBoxUseTagsFromMediaFiles;
    wxChoice* m_choiceFfmpegCodec;
    wxChoice* m_choiceAudioSampleWidth;
    wxCheckBox* m_checkBoxDowngradeHiResAudio;
    wxCheckBox* m_checkBoxGenerateEditionUid;
    wxCheckBox* m_checkBoxReadMedatata;
    wxCheckBox* m_checkBoxUseMLang;
    wxCheckBox* m_checkBoxGenerateArtistTagsForTracks;
    wxCheckBox* m_checkBoxIncludeDiscNumberTag;
    wxCheckBox* m_checkBoxChapterEndTime;
    wxCheckBox* m_checkBoxChapterEndTimeFromNext;
    wxCheckBox* m_checkBoxOffset;
    wxCheckBox* m_checkBoxTrack01Idx;
    wxTextCtrl* m_textCtrlChapterOffset;
    wxChoice* m_choiceIdx;
    wxCheckBox* m_checkBoxIdxToHiddenChapters;
    wxCheckBox* m_checkBoxTrackTilteFmt;
    wxTextCtrl* m_textCtrlTrackTilteFmt;
    wxCheckBox* m_checkBoxMkaTitleFmt;
    wxTextCtrl* m_textCtrlMkaTitleFmt;
    wxCheckBox* m_checkBoxAttachLogs;
    wxCheckBox* m_checkBoxAttachAccuRip;
    wxCheckBox* m_checkBoxAttachmentsInParentDir;
    wxCheckBox* m_checkBoxApplyTags;
    wxCheckBox* m_checkBoxAttachCover;
    wxCheckBox* m_checkBoxConvertCover;
    wxCheckBox* m_checkBoxCoverFromPdf;
    wxChoice* m_choiceConvertedImageExt;
    wxChoice* m_choiceCueSheetAttachMode;
    wxCheckBox* m_checkBoxFullPathInOptions;
    wxCheckBox* m_checkBoxSmallEmDash;
    wxCheckBox* m_checkBoxRenderMultilineTags;
    wxCheckBox* m_checkBoxRenderReplayGainTags;
    wxCheckBox* m_checkBoxRunReplayGainScanner;
    wxCheckBox* m_checkBoxRenderITunesTags;
    ListBox* m_listBoxMessages;
    wxCheckBox* m_checkBoxShowTimestamps;
    wxCheckBox* m_checkBoxVerbose;
    wxCheckBox* m_checkBoxSwitchToMessagesPane;
    wxChoice* m_choiceTool;
    wxChoice* m_choiceToolParam;

    protected:

    void OnClose(wxCloseEvent& event);
    void OnCopyEvents(wxCommandEvent& event);
    void OnProcessTerminated(wxProcessEvent& event);

#ifdef __WXMSW__
    void OnTaskKillProcessTerminated(wxProcessEvent& event);
#endif
    void OnIdle(wxIdleEvent& event);
    void OnUpdateRunUiCtrl(wxUpdateUIEvent& event);
    void OnUpdateMsgCtrls(wxUpdateUIEvent& event);
    void OnUpdateButtonRun(wxUpdateUIEvent& event);
    void OnIdleWakeupTimer(wxTimerEvent& event);
    void OnAutoScrollTimer(wxTimerEvent& event);
    void OnExecCue2Mkc(wxCommandEvent& event);
    void OnCheckVerbose(wxCommandEvent& event);
    void OnCheckShowTimestamps(wxCommandEvent& event);
    void OnButtonAdd(wxCommandEvent& event);
    void OnButtonDelete(wxCommandEvent& event);
    void OnButtonMakeMask(wxCommandEvent& event);
    void OnButtonAddMediaFile(wxCommandEvent& event);
    void OnButtonIdentifyMediaFile(wxCommandEvent& event);
    void OnUpdateDst(wxUpdateUIEvent& event);
    void OnUpdateCtrlDst(wxUpdateUIEvent& event);
    void OnChooseDst(wxCommandEvent& event);
    void OnSuggestJoinMode(wxCommandEvent& event);
    void OnUpdateJoinMode(wxUpdateUIEvent& event);
    void OnUpdateUseMediaFilesWithoutCue(wxUpdateUIEvent& event);
    void OnUpdateMsgCnt(wxUpdateUIEvent& event);
    void OnCheckAutoScroll(wxCommandEvent& event);
    void OnToolChoice(wxCommandEvent&);
    void OnToolExec(wxCommandEvent&);

    protected:

    void ExecuteCmd(const wxFileName&, const wxString&, const wxString&);

#ifdef __WXMSW__
    void ExecuteTaskKill();
#endif

    void ExecuteCue2Mkc(const wxString&, bool = true);
    void ExecuteCue2Mkc(const wxArrayString&, bool = true);

    void ExecuteMkvmerge(const wxString&);
    void ExecuteMkvmerge(const wxArrayString&);

    void ExecuteFfmpeg(const wxString&);
    void ExecuteFfmpeg(const wxArrayString&);

    void ExecuteFfprobe(const wxString&);
    void ExecuteFfprobe(const wxArrayString&);

    void ProcessOutErr(bool = false);

    void AddMainItem(const wxString&, bool = true);
    void AddChildItem(const wxTreeItemId&, const wxString&);

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

    private:

    wxPanel* create_src_dst_pannel(wxNotebook*);
    wxPanel* create_general_panel(wxNotebook*);
    wxPanel* create_chapter_panel(wxNotebook*);
    wxPanel* create_adv_panel(wxNotebook*);
    wxPanel* create_messages_panel(wxNotebook*);
    wxNotebook* create_notebook();
    wxBoxSizer* create_bottom_ctrls();
    bool read_options(wxArrayString&) const;
    static void get_cmd_choices(int, wxArrayString&);

    public:

    wxMainFrame(wxWindow* = nullptr, wxWindowID = wxID_ANY, const wxString& = _("cue2mkc frontend"), const wxPoint& = wxDefaultPosition, const wxSize& = wxDefaultSize, long = wxDEFAULT_FRAME_STYLE | wxCLIP_SIBLINGS | wxTAB_TRAVERSAL);
    void OnDropFiles(const wxArrayString&);
};

