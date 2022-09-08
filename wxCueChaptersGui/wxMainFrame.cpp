/*
 *      wxMainFrame.cpp
 */

#include <app_config.h>
#include "wxApp.h"
#include "wxMainFrame.h"

namespace
{
    constexpr int AUTO_SCROLL_UPDATE_INTERVAL = 2000;
    constexpr int TIMER_IDLE_WAKE_UP_INTERVAL = 250;

    wxStaticBoxSizer* create_static_box_sizer(wxWindow* const parent, const wxString& label, const wxOrientation orientation)
    {
        return new wxStaticBoxSizer(new wxStaticBox(parent, wxID_ANY, label), orientation);
    }

    wxStaticText* create_static_text(wxWindow* const parent, const wxString& label)
    {
        wxStaticText* const res = new wxStaticText(parent, wxID_ANY, label);
        res->Wrap(-1);
        return res;
    }

    wxStaticText* create_static_text(const wxStaticBoxSizer* const parentSizer, const wxString& label)
    {
        return create_static_text(parentSizer->GetStaticBox(), label);
    }

    wxStaticLine* create_horizontal_static_line(wxWindow* const parent)
    {
        return new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(0, parent->FromDIP(1)), wxLI_HORIZONTAL);
    }

    wxSizerFlags get_horizontal_static_line_sizer_flags(const wxWindow* const wnd)
    {
        return wxSizerFlags().Expand().Border(wxTOP | wxBOTTOM, wnd->FromDIP(1));
    }

    wxStaticLine* create_horizontal_static_line(const wxStaticBoxSizer* const parentSizer)
    {
        return create_horizontal_static_line(parentSizer->GetStaticBox());
    }

    wxStaticLine* create_vertical_static_line(wxWindow* const parent)
    {
        return new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(parent->FromDIP(1), 0), wxLI_VERTICAL);
    }

    wxSizerFlags get_vertical_static_line_sizer_flags(const wxWindow* const wnd)
    {
        return wxSizerFlags().Expand().Border(wxLEFT | wxRIGHT, wnd->FromDIP(2));
    }

    wxStaticLine* create_vertical_static_line(const wxStaticBoxSizer* const parentSizer)
    {
        return create_vertical_static_line(parentSizer->GetStaticBox());
    }

    wxComboBox* create_combobox(wxWindow* const parent, const wxArrayString& choices, const size_t selIdx = -1)
    {
        wxComboBox* const res = new wxComboBox(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, choices);
        if (selIdx >= 0)
        {
            res->SetSelection(selIdx);
        }
        return res;
    }

    wxComboBox* create_combobox(const wxStaticBoxSizer* const sizer, const wxArrayString& choices, const size_t selIdx = -1)
    {
        return create_combobox(sizer->GetStaticBox(), choices, selIdx);
    }

    wxChoice* create_choice(wxWindow* const parent, const wxArrayString& choices, const size_t selIdx = 0)
    {
        wxChoice* const choice = new wxChoice(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices);
        choice->SetSelection(selIdx);
        return choice;
    }

    wxChoice* create_choice(const wxStaticBoxSizer* const sizer, const wxArrayString& choices, const size_t selIdx = 0)
    {
        return create_choice(sizer->GetStaticBox(), choices, selIdx);
    }

    wxCheckBox* create_checkbox(wxWindow* const parent, const wxString& label, const bool val = false)
    {
        wxCheckBox* const res = new wxCheckBox(parent, wxID_ANY, label);
        res->SetValue(val);
        return res;
    }

    wxCheckBox* create_checkbox(const wxStaticBoxSizer* const parentSizer, const wxString& label, const bool val = false)
    {
        return create_checkbox(parentSizer->GetStaticBox(), label, val);
    }

    wxCheckBox* create_3state_checkbox(wxWindow* const parent, const wxString& label, const wxCheckBoxState state = wxCHK_UNDETERMINED)
    {
        wxCheckBox* const res = new wxCheckBox(parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
        res->Set3StateValue(state);
        return res;
    }

    wxCheckBox* create_3state_checkbox(const wxStaticBoxSizer* const parentSizer, const wxString& label, const wxCheckBoxState state = wxCHK_UNDETERMINED)
    {
        return create_3state_checkbox(parentSizer->GetStaticBox(), label, state);
    }

    bool is_checked(const wxCheckBox* const checkBox)
    {
        if (checkBox->Is3State())
        {
            return (checkBox->Get3StateValue() == wxCHK_CHECKED);
        }
        else
        {
            return checkBox->GetValue();
        }
    }

    wxButton* create_button(wxWindow* const parent, const wxString& label)
    {
        return new wxButton(parent, wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    }

    wxButton* create_button(const wxStaticBoxSizer* const parentSizer, const wxString& label)
    {
        return create_button(parentSizer->GetStaticBox(), label);
    }

    wxTextCtrl* create_text_ctrl(wxWindow* const parent, const wxString& label = wxEmptyString, const unsigned long maxLength = 0)
    {
        wxTextCtrl* const res = new wxTextCtrl(parent, wxID_ANY, label);
        if (maxLength > 0) res->SetMaxLength(maxLength);
        return res;
    }

    wxTextCtrl* create_text_ctrl(const wxStaticBoxSizer* const parentSizer, const wxString& label = wxEmptyString, const unsigned long maxLength = 0)
    {
        return create_text_ctrl(parentSizer->GetStaticBox(), label, maxLength);
    }

    wxBitmapButton* create_bitmap_button(wxWindow* const parent, const wxBitmapBundle& bitmapBundle, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        wxBitmapButton* const res = new wxBitmapButton(parent, wxID_ANY, bitmapBundle);
        res->SetWindowVariant(windowVariant);
        return res;
    }

    wxBitmapButton* create_bitmap_button(const wxStaticBoxSizer* const parentSizer, const wxBitmapBundle& bitmapBundle, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        return create_bitmap_button(parentSizer->GetStaticBox(), bitmapBundle, windowVariant);
    }

    wxBitmapButton* create_bitmap_button(wxWindow* const parent, const wxString& resName, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        wxBitmapBundle bitmapBundle;
        wxCHECK_MSG(wxGetApp().LoadMaterialDesignIcon(resName, windowVariant, bitmapBundle), nullptr, wxString::Format("Fail to load bitmap: id=%s", resName));
        return create_bitmap_button(parent, bitmapBundle, windowVariant);
    }

    wxBitmapButton* create_bitmap_button(const wxStaticBoxSizer* const parentSizer, const wxString& resName, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        return create_bitmap_button(parentSizer->GetStaticBox(), resName, windowVariant);
    }

    wxStaticBitmap* create_static_bitmap(wxWindow* const parent, const wxBitmapBundle& bitmapBundle, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        wxStaticBitmap* const res = new wxStaticBitmap(parent, wxID_ANY, bitmapBundle);
        res->SetWindowVariant(windowVariant);
        return res;
    }

    wxStaticBitmap* create_static_bitmap(const wxStaticBoxSizer* const parentSizer, const wxBitmapBundle& bitmapBundle, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        return create_static_bitmap(parentSizer->GetStaticBox(), bitmapBundle, windowVariant);
    }

    wxStaticBitmap* create_static_bitmap(wxWindow* const parent, const wxString& resName, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        wxBitmapBundle bitmapBundle;
        wxCHECK_MSG(wxGetApp().LoadMaterialDesignIcon(resName, windowVariant, bitmapBundle), nullptr, wxString::Format("Fail to load bitmap: id=%s", resName));
        return create_static_bitmap(parent, bitmapBundle, windowVariant);
    }

    wxStaticBitmap* create_static_bitmap(const wxStaticBoxSizer* const parentSizer, const wxString& resName, const wxWindowVariant windowVariant = wxWINDOW_VARIANT_SMALL)
    {
        return create_static_bitmap(parentSizer->GetStaticBox(), resName, windowVariant);
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
            m_txtInputStream.reset(new wxTextInputStream(*(GetInputStream()), wxEmptyString, wxConvUTF8));
            m_txtErrorStream.reset(new wxTextInputStream(*(GetErrorStream()), wxEmptyString, wxConvUTF8));
        }

        wxTextInputStream& GetTxtInputStream() const
        {
            return *(m_txtInputStream.get());
        }

        wxTextInputStream& GetTxtErrorStream() const
        {
            return *(m_txtErrorStream.get());
        }

        bool HaveOutOrErr() const
        {
            return !(GetInputStream()->Eof() && GetErrorStream()->Eof());
        }

        private:

        wxScopedPtr<wxTextInputStream> m_txtInputStream;
        wxScopedPtr<wxTextInputStream> m_txtErrorStream;
    };

    class DropTarget:
        public wxFileDropTarget
    {
        public:

        DropTarget(wxMainFrame* pMainFrame): m_pMainFrame(pMainFrame)
        {
        }

        virtual bool OnDropFiles(wxCoord WXUNUSED(x), wxCoord WXUNUSED(y), const wxArrayString& filenames)
        {
            wxBusyCursor busy;
            m_pMainFrame->OnDropFiles(filenames);
            return true;
        }

        private:

        wxMainFrame* const m_pMainFrame;
    };

    bool negatable_switch_option(wxArrayString& options, const wxCheckBox* const checkBox, const wxString& cmdSwitch)
    {
        if (!checkBox->IsEnabled())
        {
            wxLogDebug("cmd[%s] - check box disabled, command line option was not generated", cmdSwitch);
            return false;
        }

        if (checkBox->Is3State())
        {
            const wxCheckBoxState state = checkBox->Get3StateValue();
            switch (state)
            {
                case wxCHK_CHECKED:
                {
                    options.Add(wxString::Format("-%s", cmdSwitch));
                    return true;
                }

                case wxCHK_UNCHECKED:
                {
                    options.Add(wxString::Format("-%s-", cmdSwitch));
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
            if (checkBox->GetValue())
            {
                options.Add(wxString::Format("-%s", cmdSwitch));
                return true;
            }
            else
            {
                options.Add(wxString::Format("-%s-", cmdSwitch));
                return false;
            }
        }
    }

    bool negatable_long_switch_option(wxArrayString& options, const wxCheckBox* const checkBox, const wxString& cmdSwitch)
    {
        if (!checkBox->IsEnabled())
        {
            wxLogDebug("cmd[%s] - check box disabled, command line option was not generated", cmdSwitch);
            return false;
        }

        if (checkBox->Is3State())
        {
            const wxCheckBoxState state = checkBox->Get3StateValue();
            switch (state)
            {
                case wxCHK_CHECKED:
                {
                    options.Add(wxString::Format("--%s", cmdSwitch));
                    return true;
                }

                case wxCHK_UNCHECKED:
                {
                    options.Add(wxString::Format("--%s-", cmdSwitch));
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
            if (checkBox->GetValue())
            {
                options.Add(wxString::Format("--%s", cmdSwitch));
                return true;
            }
            else
            {
                options.Add(wxString::Format("--%s-", cmdSwitch));
                return false;
            }
        }
    }

    wxString join_strings(const wxArrayString& elems, const wxString& sep = wxEmptyString)
    {
        if (elems.IsEmpty()) return wxEmptyString;

        wxString res;

        for (wxArrayString::const_iterator i = elems.begin(); i != elems.end(); ++i)
        {
            res.Append(sep).Append(*i);
        }

        return res;
    }

    bool have_spaces(const wxString& str, const wxRegEx& spaceChecker)
    {
        if (str.StartsWith("--") || str.StartsWith('-')) return false;

        return spaceChecker.Matches(str);
    }

    wxString options_to_str(const wxArrayString& options)
    {
        if (options.IsEmpty()) return wxEmptyString;

        const wxRegEx spaceChecker("\\p{Xps}", wxRE_NOSUB);

        wxASSERT(spaceChecker.IsValid());

        wxString res;

        for (wxArrayString::const_iterator i = options.begin(), end = options.end(); i != end; ++i)
        {
            if (have_spaces(*i, spaceChecker))
            {
                res.Append('\"').Append(*i).Append("\" ");
            }
            else
            {
                res.Append(*i).Append(' ');
            }
        }

        res.RemoveLast();
        return res;
    }

    wxString make_fmask(const wxString& fname)
    {
        wxFileName fn(fname);

        fn.SetName("*");
        return fn.GetFullPath();
    }

    class CheckBoxUiUpdater
    {
        public:

        CheckBoxUiUpdater(const wxCheckBox* const checkBox)
            : m_checkBox(checkBox)
        {
        }

        CheckBoxUiUpdater(const CheckBoxUiUpdater& uiUpdater)
            : m_checkBox(uiUpdater.m_checkBox)
        {
        }

        void operator ()(wxUpdateUIEvent& event) const
        {
            event.Enable(is_checked());
        }

        bool operator !=(const CheckBoxUiUpdater& uiUpdater) const
        {
            return m_checkBox != uiUpdater.m_checkBox;
        }

        bool operator ==(const CheckBoxUiUpdater& uiUpdater) const
        {
            return m_checkBox == uiUpdater.m_checkBox;
        }

        private:

        const wxCheckBox* const m_checkBox;

        bool is_checked() const
        {
            if (m_checkBox->Is3State()) return (m_checkBox->Get3StateValue() == wxCHK_CHECKED || m_checkBox->Get3StateValue() == wxCHK_UNDETERMINED);
            else return m_checkBox->GetValue();
        }
    };

    class ChoiceFormatUiUpdater
    {
        public:

        ChoiceFormatUiUpdater(const wxChoice* const comboBox)
            :m_choice(comboBox)
        {
        }

        ChoiceFormatUiUpdater(const ChoiceFormatUiUpdater& uiUpdater)
            :m_choice(uiUpdater.m_choice)
        {
        }

        void operator ()(wxUpdateUIEvent& event) const
        {
            const int sel = m_choice->GetSelection();
            event.Enable((sel > 0) && (sel < 5));
        }

        bool operator !=(const ChoiceFormatUiUpdater& uiUpdater) const
        {
            return m_choice != uiUpdater.m_choice;
        }

        bool operator ==(const ChoiceFormatUiUpdater& uiUpdater) const
        {
            return m_choice == uiUpdater.m_choice;
        }

        private:

        const wxChoice* const m_choice;
    };

    class TreeCtrlUiUpdater
    {
        public:

        TreeCtrlUiUpdater(const wxTreeCtrl* const treeCtrl)
            : m_treeCtrl(treeCtrl)
        {
        }

        TreeCtrlUiUpdater(const TreeCtrlUiUpdater& uiUpdater)
            : m_treeCtrl(uiUpdater.m_treeCtrl)
        {
        }

        void operator ()(wxUpdateUIEvent& event) const
        {
            const wxTreeItemId itemId = m_treeCtrl->GetSelection();

            event.Enable(itemId.IsOk());
        }

        bool operator !=(const TreeCtrlUiUpdater& uiUpdater) const
        {
            return m_treeCtrl != uiUpdater.m_treeCtrl;
        }

        bool operator ==(const TreeCtrlUiUpdater& uiUpdater) const
        {
            return m_treeCtrl == uiUpdater.m_treeCtrl;
        }

        private:

        const wxTreeCtrl* const m_treeCtrl;
    };

    class TreeCtrlProcessUiUpdater
    {
        public:

        TreeCtrlProcessUiUpdater(const wxTreeCtrl* const treeCtrl, const wxScopedPtr< wxProcess >& process)
            : m_treeCtrl(treeCtrl), m_process(process)
        {
        }

        TreeCtrlProcessUiUpdater(const TreeCtrlProcessUiUpdater& uiUpdater)
            : m_treeCtrl(uiUpdater.m_treeCtrl), m_process(uiUpdater.m_process)
        {
        }

        void operator ()(wxUpdateUIEvent& event) const
        {
            if (m_process)
            {
                event.Enable(false);
                return;
            }

            const wxTreeItemId itemId = m_treeCtrl->GetSelection();
            event.Enable(itemId.IsOk());
        }

        private:

        const wxTreeCtrl* const m_treeCtrl;
        const wxScopedPtr< wxProcess >& m_process;
    };

    class CollapsiblePaneUiUpdater
    {
        public:

        CollapsiblePaneUiUpdater(wxWindow* wnd)
            :m_wnd(wnd)
        {
        }

        CollapsiblePaneUiUpdater(const CollapsiblePaneUiUpdater& uiUpdater)
            :m_wnd(uiUpdater.m_wnd)
        {
        }

        bool operator==(const CollapsiblePaneUiUpdater& uiUpdater) const
        {
            return m_wnd == uiUpdater.m_wnd;
        }

        bool operator!=(const CollapsiblePaneUiUpdater& uiUpdater) const
        {
            return m_wnd != uiUpdater.m_wnd;
        }

        void operator()(wxCollapsiblePaneEvent& WXUNUSED(event)) const
        {
            m_wnd->Layout();
        }

        private:

        wxWindow* const m_wnd;
    };

    class ExclusiveCheckBoxEvtHandler
    {
        public:

        ExclusiveCheckBoxEvtHandler(wxCheckBox* const checkBox1, wxCheckBox* const checkBox2)
            :m_checkBox1(checkBox1), m_checkBox2(checkBox2)
        {
        }

        ExclusiveCheckBoxEvtHandler(const ExclusiveCheckBoxEvtHandler& uiUpdater)
            :m_checkBox1(uiUpdater.m_checkBox1), m_checkBox2(uiUpdater.m_checkBox2)
        {
        }

        void Bind() const
        {
            m_checkBox1->Bind(wxEVT_CHECKBOX, *this);
            m_checkBox2->Bind(wxEVT_CHECKBOX, *this);
        }

        void operator()(wxCommandEvent& event) const
        {
            wxASSERT(event.GetEventObject()->IsSameAs(*m_checkBox1) || event.GetEventObject()->IsSameAs(*m_checkBox2));

            const wxCheckBox* const eventSource = wxConstCast(wxDynamicCast(event.GetEventObject(), wxCheckBox), wxCheckBox);
            wxCheckBox* const otherCheckBox = (eventSource == m_checkBox1) ? m_checkBox2 : m_checkBox1;

            if (is_checked(eventSource))
            {
                // uncheck other checkbox
                if (otherCheckBox->Is3State())
                {
                    otherCheckBox->Set3StateValue(wxCHK_UNCHECKED);
                }
                else
                {
                    otherCheckBox->SetValue(false);
                }
            }
        }

        private:

        wxCheckBox* const m_checkBox1;
        wxCheckBox* const m_checkBox2;
    };

    wxString get_array_item(const wxArrayString& a, const size_t idx)
    {
        const size_t cnt = a.GetCount();

        if (idx >= cnt) return wxEmptyString;

        return a[idx];
    }

    wxString truncate_dir_path(const wxFileName& fn, const size_t dirCnt)
    {
        wxFileName res(fn);

        while (res.GetDirCount() > dirCnt)
        {
            res.RemoveLastDir();
        }

        if (res.GetDirCount() == 0) return wxEmptyString;

        return res.GetFullPath().RemoveLast();
    }

    wxString find_common_path(const wxArrayString& paths)
    {
        if (paths.IsEmpty()) return wxEmptyString;

        const size_t cnt = paths.GetCount();

        wxFileName path;

        path.AssignDir(wxFileName(paths[0]).GetPath());

        if (cnt == 1) return path.GetFullPath();

        size_t dirCnt = path.GetDirCount();

        std::vector< wxFileName > fn;

        for (size_t i = 1; i < cnt; ++i)
        {
            wxFileName f;
            f.AssignDir(wxFileName(paths[i]).GetPath());

            const size_t dirCnt1 = f.GetDirCount();

            if (dirCnt1 > dirCnt) dirCnt = dirCnt1;

            fn.push_back(f);
        }

        // check drive
        const wxString vol = path.GetVolume();

        for (auto i = fn.cbegin(); i != fn.cend(); ++i)
        {
            const wxString jvol = i->GetVolume();

            if (vol.CmpNoCase(jvol) != 0) return wxEmptyString;
        }

        // check dir components
        for (size_t i = 0; i < dirCnt; ++i)
        {
            const wxString dir = get_array_item(path.GetDirs(), i);

            if (dir.IsEmpty()) return truncate_dir_path(path, i);

            for (auto j = fn.cbegin(); j != fn.cend(); ++j)
            {
                const wxString jdir = get_array_item(j->GetDirs(), i);

                if (jdir.IsEmpty() || (dir.CmpNoCase(jdir) != 0)) return truncate_dir_path(path, i);
            }
        }

        return truncate_dir_path(path, dirCnt);
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
            const WXDWORD err = GetLastError();
            wxLogError(_("exe[t]: signal CTRL+BREAK not sent, error code %d"), err);
        }
        res = FreeConsole() != 0;
    }

    void kill_console_process(const wxProcess& process)
    {
        kill_console_process(process.GetPid());
    }

    wxSize calc_text_size(const wxWindow* const wnd, int charWidth)
    {
        const wxSize res = wnd->GetTextExtent(wxString(wxUniChar(0x2013), charWidth));
        return wxSize(res.x, -1);
    }

    wxSize calc_text_size(const wxWindow* const wnd, int charWidth, int charHeight)
    {
        wxString txt(wxUniChar(0x2013), charWidth);
        const wxSize extent = wnd->GetTextExtent(txt);
        return wxSize(extent.GetWidth(), extent.GetHeight() * charHeight);
    }
}

wxPanel* wxMainFrame::create_src_dst_pannel(wxNotebook* notebook)
{
    wxPanel* const    panel = new wxPanel(notebook);
    wxBoxSizer* const panelSizer = new wxBoxSizer(wxVERTICAL);

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Sources"), wxVERTICAL);
        const int gap = FromDIP(1);

        m_treeCtrlInputFiles = new wxTreeCtrl(sizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_TWIST_BUTTONS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HIDE_ROOT | wxBORDER_THEME);
        m_treeCtrlInputFilesRoot = m_treeCtrlInputFiles->AddRoot("hidden root");
        sizer->Add(m_treeCtrlInputFiles, 1, wxEXPAND);

        {
            wxBoxSizer* const innerSizer = new wxBoxSizer(wxHORIZONTAL);
            TreeCtrlUiUpdater uiUpdater(m_treeCtrlInputFiles);

            m_checkBoxSuggestJoin = create_checkbox(sizer, _("Suggest join mode"), true);
            m_checkBoxSuggestJoin->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
            m_checkBoxSuggestJoin->Bind(wxEVT_CHECKBOX, &wxMainFrame::OnSuggestJoinMode, this);
            innerSizer->Add(m_checkBoxSuggestJoin, wxSizerFlags().CenterVertical());

            innerSizer->Add(0, 0, 1, wxEXPAND);

            {
                wxBitmapButton* const button = create_bitmap_button(sizer, "content-add");
                button->Bind(wxEVT_BUTTON, &wxMainFrame::OnButtonAdd, this);
                innerSizer->Add(button, wxSizerFlags().CenterVertical().Border(wxRIGHT, gap));
            }

            {
                wxBitmapButton* const button = create_bitmap_button(sizer, "content-remove");
                button->Bind(wxEVT_UPDATE_UI, uiUpdater);
                button->Bind(wxEVT_BUTTON, &wxMainFrame::OnButtonDelete, this);
                innerSizer->Add(button, wxSizerFlags().CenterVertical().Border(wxLEFT | wxRIGHT, gap));
            }

            {
                wxBitmapButton* const button = create_bitmap_button(sizer, "editor-data_array");
                button->Bind(wxEVT_UPDATE_UI, uiUpdater);
                button->Bind(wxEVT_BUTTON, &wxMainFrame::OnButtonMakeMask, this);
                innerSizer->Add(button, wxSizerFlags().CenterVertical().Border(wxLEFT | wxRIGHT, gap));
            }

            {
                wxBitmapButton* const button = create_bitmap_button(sizer, "av-audio_file");
                button->Bind(wxEVT_UPDATE_UI, uiUpdater);
                button->Bind(wxEVT_BUTTON, &wxMainFrame::OnButtonAddMediaFile, this);
                innerSizer->Add(button, wxSizerFlags().CenterVertical().Border(wxLEFT | wxRIGHT, gap));
            }

            innerSizer->Add(create_vertical_static_line(sizer), get_vertical_static_line_sizer_flags(panel));

            {
                wxBitmapButton* const button = create_bitmap_button(sizer, "action-info");
                button->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
                button->Bind(wxEVT_UPDATE_UI, TreeCtrlProcessUiUpdater(m_treeCtrlInputFiles, m_pProcess));
                button->Bind(wxEVT_BUTTON, &wxMainFrame::OnButtonIdentifyMediaFile, this);
                innerSizer->Add(button, wxSizerFlags().CenterVertical().Border(wxLEFT, gap));
            }

            sizer->Add(innerSizer, 0, wxTOP | wxBOTTOM | wxEXPAND, gap);
        }

        panelSizer->Add(sizer, wxSizerFlags().Expand().Proportion(1));
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Destination"), wxHORIZONTAL);
        sizer->GetStaticBox()->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateDst, this);
        const int gap = panel->FromDIP(2);

        {
            wxArrayString choices;
            choices.Add(_("Input directory"));
            choices.Add(_("Directory"));
            choices.Add(_("File"));

            m_choiceDst = create_choice(sizer, choices);
        }
        sizer->Add(m_choiceDst, wxSizerFlags().CenterVertical().Border(wxRIGHT));

        m_textCtrlDst = create_text_ctrl(sizer, wxEmptyString, 1024);
        m_textCtrlDst->SetValue(wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Music));
        m_textCtrlDst->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateCtrlDst, this);
        sizer->Add(m_textCtrlDst, wxSizerFlags().CenterVertical().Border(wxLEFT | wxRIGHT, gap).Proportion(1));

        {
            wxBitmapButton* const button = create_bitmap_button(sizer, "navigation-more_horiz", wxWINDOW_VARIANT_NORMAL);
            button->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateCtrlDst, this);
            button->Bind(wxEVT_BUTTON, &wxMainFrame::OnChooseDst, this);
            sizer->Add(button, wxSizerFlags().CenterVertical());
        }

        panelSizer->Add(sizer, wxSizerFlags().Expand());
    }

    panel->SetSizerAndFit(panelSizer);
    return panel;
}

wxPanel* wxMainFrame::create_general_panel(wxNotebook* notebook)
{
    wxScrolledWindow* const panel = new wxScrolledWindow(notebook);
    const int panelGap = FromDIP(2);
    wxGridBagSizer* const panelSizer = new wxGridBagSizer(panelGap, panelGap);

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("General"), wxVERTICAL);

        m_checkBoxAbortOnErrors = create_3state_checkbox(sizer, _("Abort on errors"));
        sizer->Add(m_checkBoxAbortOnErrors);

        m_checkBoxJoinMode = create_checkbox(sizer->GetStaticBox(), _("Join mode"));
        m_checkBoxJoinMode->SetToolTip(_("Join all input files into one"));
        m_checkBoxJoinMode->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateJoinMode, this);
        sizer->Add(m_checkBoxJoinMode);

        m_checkBoxMono = create_3state_checkbox(sizer, _("Dual mono"));
        m_checkBoxMono->SetToolTip(_("Assume audio as dual mono and copy single channel only"));
        sizer->Add(m_checkBoxMono);

        m_checkBoxUseMediaFiles = create_3state_checkbox(sizer->GetStaticBox(), _("Use media files"), wxCHK_CHECKED);
        m_checkBoxUseMediaFiles->SetToolTip(_("Use media file(s) to calculate end time of chapters"));
        sizer->Add(m_checkBoxUseMediaFiles);

        m_checkBoxMediaFilesWithoutCue = create_3state_checkbox(sizer, _("Media files without cuesheet"));
        m_checkBoxMediaFilesWithoutCue->SetToolTip(_("Assume input file(s) as media files without cuesheet"));
        m_checkBoxMediaFilesWithoutCue->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateUseMediaFilesWithoutCue, this);
        sizer->Add(m_checkBoxMediaFilesWithoutCue);

        {
            wxBoxSizer* const innerSizer = new wxBoxSizer(wxHORIZONTAL);

            const wxSize editMinSize = calc_text_size(panel, 20);

            innerSizer->Add(create_static_text(sizer, _("Media types")), wxSizerFlags().CenterVertical().Border(wxRIGHT, panelGap));

            m_textCtrlExt = create_text_ctrl(sizer, wxEmptyString, 40);
            m_textCtrlExt->SetSizeHints(editMinSize);
            m_textCtrlExt->SetToolTip(_("Comma separated list of alternate media files extensions (without leading dot)"));
            innerSizer->Add(m_textCtrlExt, wxSizerFlags().CentreVertical().Proportion(1));

            sizer->Add(innerSizer, wxSizerFlags().Expand());
        }

        panelSizer->Add(sizer, wxGBPosition(0, 0), wxDefaultSpan, wxEXPAND);
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Output"), wxVERTICAL);

        {
            const int gap = FromDIP(1);
            wxGridBagSizer* const innerSizer = new wxGridBagSizer(gap, gap);
            wxGBSpan              oneCol(1, 2);

            // col 0
            innerSizer->Add(create_static_text(sizer, _("Format")), wxGBPosition(0, 0));

            {
                wxArrayString choices;
                choices.Add(_("CUE sheet"));
                choices.Add(_("mkvmerge - no tags"));
                choices.Add(_("mkvmerge"));
                choices.Add(_("ffmpeg - no tags"));
                choices.Add(_("ffmpeg"));
                choices.Add(_("Chapters (wav2img)"));

                m_choiceFormat = create_choice(sizer->GetStaticBox(), choices, 2);
            }
            innerSizer->Add(m_choiceFormat, wxGBPosition(0, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);

            // col 1
            {
                {
                    wxStaticText* const staticText = create_static_text(sizer, _("Codec"));
                    innerSizer->Add(staticText, wxGBPosition(1, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
                }

                {
                    wxArrayString choices;
                    choices.Add("default");
                    choices.Add("pcmle");
                    choices.Add("pcmbe");
                    choices.Add("flac");
                    choices.Add("wavpack");
                    choices.Add("opus");

                    m_choiceFfmpegCodec = create_choice(sizer, choices);
                    m_choiceFfmpegCodec->SetToolTip(_("Audio codec"));

                    innerSizer->Add(m_choiceFfmpegCodec, wxGBPosition(1, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);
                }
            }

            // col 2
            {
                {
                    wxStaticText* const staticText = create_static_text(sizer, _("Bit depth"));
                    innerSizer->Add(staticText, wxGBPosition(2, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
                }

                {
                    wxArrayString choices;
                    choices.Add("auto");
                    choices.Add("16");
                    choices.Add("24");

                    m_choiceAudioSampleWidth = create_choice(sizer, choices);
                    m_choiceAudioSampleWidth->SetToolTip(_("Audio sample width"));

                    const wxSizerFlags choiceSizer = wxSizerFlags().Expand().Proportion(1).Border(wxLEFT);
                    innerSizer->Add(m_choiceAudioSampleWidth, wxGBPosition(2, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);
                }
            }

            // col 3
            m_checkBoxDowngradeHiResAudio = create_checkbox(sizer, _wxS("Downgrade Hi\u2011Res audio"));
            innerSizer->Add(m_checkBoxDowngradeHiResAudio, wxGBPosition(3, 0), oneCol, wxALIGN_CENTER_VERTICAL);


            // col 4
            {
                const wxSizerFlags lineFlags = get_horizontal_static_line_sizer_flags(panel);
                innerSizer->Add(create_horizontal_static_line(sizer),
                           wxGBPosition(4, 0), oneCol,
                           lineFlags.GetFlags(), lineFlags.GetBorderInPixels());
            }

            // col 5
            innerSizer->Add(create_static_text(sizer, _("Language")), wxGBPosition(5, 0));

            {
                wxArrayString choices;
                choices.Add("und");
                choices.Add("eng");
                choices.Add("ger");
                choices.Add("pol");

                m_textCtrlLang = create_combobox(sizer, choices);
            }
            m_textCtrlLang->SetToolTip(_("Default language for chapters and tags"));
            innerSizer->Add(m_textCtrlLang, wxGBPosition(5, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);

            // col 6
            innerSizer->Add(create_static_text(sizer, _("Text encoding")), wxGBPosition(6, 0));

            {
                wxArrayString choices;
                choices.Add(_("default"));
                choices.Add(_("UTF-8"));
                choices.Add(_("UTF-8 with BOM"));
                choices.Add(_("UTF-16"));
                choices.Add(_("UTF-16 with BOM"));
                choices.Add(_("UTF-16BE"));
                choices.Add(_("UTF-16BE with BOM"));

                m_choiceEncoding = create_choice(sizer->GetStaticBox(), choices, 2);
                m_choiceEncoding->SetToolTip(_("Text encoding of generated CUE and XML files"));
            }
            innerSizer->Add(m_choiceEncoding, wxGBPosition(6, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);

            // col 7
            {
                const wxSizerFlags lineFlags = get_horizontal_static_line_sizer_flags(panel);
                innerSizer->Add(create_horizontal_static_line(sizer),
                           wxGBPosition(7, 0), oneCol,
                           lineFlags.GetFlags(), lineFlags.GetBorderInPixels());
            }


            // col 8
            m_checkBoxRunTool = create_checkbox(sizer, _("Run selected tool"), true);
            m_checkBoxRunTool->Bind(wxEVT_UPDATE_UI, ChoiceFormatUiUpdater(m_choiceFormat));
            innerSizer->Add(m_checkBoxRunTool, wxGBPosition(8, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            sizer->Add(innerSizer, wxSizerFlags().Expand());
        }

        panelSizer->Add(sizer, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND);
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _wxS("Matroska\u2122 container attachments"), wxVERTICAL);

        {
            const int gap = FromDIP(1);
            wxGridBagSizer* const innerSizer = new wxGridBagSizer(gap, gap);
            wxGBSpan              oneCol(1, 2);

            innerSizer->Add(create_static_text(sizer, _("Cue scheet attach mode")), wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, gap);

            {
                wxArrayString choices;
                choices.Add(_("default"));
                choices.Add(_("none"));
                choices.Add(_("source"));
                choices.Add(_("decoded"));
                choices.Add(_("rendered"));

                m_choiceCueSheetAttachMode = create_choice(sizer, choices);
            }
            innerSizer->Add(m_choiceCueSheetAttachMode, wxGBPosition(0, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);

            {
                const wxSizerFlags lineFlags = get_horizontal_static_line_sizer_flags(panel);
                innerSizer->Add(create_horizontal_static_line(sizer),
                           wxGBPosition(1, 0), oneCol,
                           lineFlags.GetFlags(), lineFlags.GetBorderInPixels());
            }

            m_checkBoxAttachCover = create_3state_checkbox(sizer, _("Attach cover image(s)"), wxCHK_CHECKED);
            innerSizer->Add(m_checkBoxAttachCover, wxGBPosition(2, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            m_checkBoxConvertCover = create_3state_checkbox(sizer, _("Convert cover image(s)"));
            innerSizer->Add(m_checkBoxConvertCover, wxGBPosition(3, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            m_checkBoxCoverFromPdf = create_3state_checkbox(sizer, _("Create cover from PDF"));
            innerSizer->Add(m_checkBoxCoverFromPdf, wxGBPosition(4, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            {
                wxStaticText* const staticText = create_static_text(sizer, _("Converted image type"));
                innerSizer->Add(staticText, wxGBPosition(5, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL);
            }

            {
                wxArrayString choices;
                choices.Add(_("default"));
                choices.Add(_("jpeg"));
                choices.Add(_("webp"));

                m_choiceConvertedImageExt = create_choice(sizer, choices);
            }
            innerSizer->Add(m_choiceConvertedImageExt, wxGBPosition(5, 1), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxEXPAND);

            {
                const wxSizerFlags lineFlags = get_horizontal_static_line_sizer_flags(panel);
                innerSizer->Add(create_horizontal_static_line(sizer),
                           wxGBPosition(6, 0), oneCol,
                           lineFlags.GetFlags(), lineFlags.GetBorderInPixels());
            }

            m_checkBoxAttachLogs = create_3state_checkbox(sizer, _("Attach EAC log(s)"));
            innerSizer->Add(m_checkBoxAttachLogs, wxGBPosition(7, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            m_checkBoxAttachAccuRip = create_3state_checkbox(sizer, _("Attach AccurateRip log(s)"));
            innerSizer->Add(m_checkBoxAttachAccuRip, wxGBPosition(8, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            {
                const wxSizerFlags lineFlags = get_horizontal_static_line_sizer_flags(panel);
                innerSizer->Add(create_horizontal_static_line(sizer),
                           wxGBPosition(9, 0), oneCol,
                           lineFlags.GetFlags(), lineFlags.GetBorderInPixels());
            }

            m_checkBoxAttachmentsInParentDir = create_3state_checkbox(sizer, _("Attachments in parent dir"));
            m_checkBoxAttachmentsInParentDir->SetToolTip(_("Look for attachments (cover and logs) also in parent dir(s)"));
            innerSizer->Add(m_checkBoxAttachmentsInParentDir, wxGBPosition(10, 0), oneCol, wxALIGN_CENTER_VERTICAL);

            sizer->Add(innerSizer, wxSizerFlags().Expand());
        }

        panelSizer->Add(sizer, wxGBPosition(1, 0), wxDefaultSpan, wxEXPAND | wxBOTTOM, panelGap);
    }

    panel->SetSizer(panelSizer);
    panel->FitInside();
    return panel;
}

wxPanel* wxMainFrame::create_chapter_panel(wxNotebook* notebook)
{
    wxScrolledWindow* const panel = new wxScrolledWindow(notebook);
    const int panelGap = FromDIP(2);
    wxGridBagSizer* const panelSizer = new wxGridBagSizer(panelGap, panelGap);

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Chapters"), wxVERTICAL);

        m_checkBoxChapterEndTime = create_3state_checkbox(sizer, _("Calculate end time of chapters (if possible)"));

        sizer->Add(m_checkBoxChapterEndTime);

        m_checkBoxChapterEndTimeFromNext = create_3state_checkbox(sizer, _("Calculate chapters's end time from next chapter"));
        m_checkBoxChapterEndTimeFromNext->SetToolTip(_("If track's time is unknown set it to next track position using specified shifted left by frame offset"));
        sizer->Add(m_checkBoxChapterEndTimeFromNext);

        {
            const int gap = FromDIP(1);
            wxBoxSizer* const innerSizer = new wxBoxSizer(wxHORIZONTAL);

            m_checkBoxOffset = create_checkbox(sizer, _("Offset"));
            innerSizer->Add(m_checkBoxOffset, wxSizerFlags().CenterVertical().Border(wxRIGHT, gap));

            m_textCtrlChapterOffset = create_text_ctrl(sizer, "150", 4);
            const wxSize textCtrlSize = calc_text_size(panel, 8);
            m_textCtrlChapterOffset->SetSizeHints(textCtrlSize);
            m_textCtrlChapterOffset->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxOffset));
            innerSizer->Add(m_textCtrlChapterOffset, wxSizerFlags().CenterVertical().Border(wxRIGHT, 2 * gap));

            wxStaticText* const staticText = create_static_text(sizer, _("frames"));
            staticText->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxOffset));
            innerSizer->Add(staticText, wxSizerFlags().CenterVertical());

            sizer->Add(innerSizer, wxSizerFlags().Border(wxTOP | wxBOTTOM, gap));
        }

        {
            const int gap = FromDIP(1);
            wxBoxSizer* const innerSizer = new wxBoxSizer(wxHORIZONTAL);

            m_checkBoxTrack01Idx = create_checkbox(sizer, _("For track 01 assume index"));
            innerSizer->Add(m_checkBoxTrack01Idx, wxSizerFlags().CenterVertical().Border(wxRIGHT, gap));

            {
                wxArrayString choices;
                choices.Add(_("00"));
                choices.Add(_("01"));

                m_choiceIdx = create_choice(sizer->GetStaticBox(), choices, 1);
            }
            m_choiceIdx->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxTrack01Idx));
            innerSizer->Add(m_choiceIdx, wxSizerFlags().CenterVertical().Border(wxRIGHT, 2 * gap));

            wxStaticText* const staticText = create_static_text(sizer, _("as beginning of track"));
            staticText->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxTrack01Idx));
            innerSizer->Add(staticText, wxSizerFlags().CenterVertical().Border(wxLEFT, gap));

            sizer->Add(innerSizer, wxSizerFlags().Border(wxTOP | wxBOTTOM, gap));
        }

        m_checkBoxIdxToHiddenChapters = create_3state_checkbox(sizer, _("Convert indexes to hidden chapters"));
        m_checkBoxIdxToHiddenChapters->SetToolTip(_wxS("Convert CD indexes greater than 01 to hidden Matroska\u2122 chapters"));
        sizer->Add(m_checkBoxIdxToHiddenChapters);

        panelSizer->Add(sizer, wxGBPosition(0,0), wxDefaultSpan, wxEXPAND);
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Unicode tweaks"), wxVERTICAL);

        m_checkBoxRemoveExtraSpaces = create_3state_checkbox(sizer, _("Remove extra spaces"));
        m_checkBoxRemoveExtraSpaces->SetToolTip(_("More than two spaces are shrinked to one"));
        sizer->Add(m_checkBoxRemoveExtraSpaces);

        m_checkBoxEllipsizeTags = create_3state_checkbox(sizer, _("Ellipsize"));
        m_checkBoxEllipsizeTags->SetToolTip(_wxS("Replace last three dots with \u2026 (ellipsis: U+2026) character"));
        sizer->Add(m_checkBoxEllipsizeTags);

        m_checkBoxCorrectDashes = create_3state_checkbox(sizer, _("Correct dashes"));
        m_checkBoxCorrectDashes->SetToolTip(_wxS("-\u2009\u2192\u2009\u2013 (en dash: U+2013)\n--\u2009\u2192\u2009\u2014 (em dash: U+2014)\n---\u2009\u2192\u2009\u2E3A (two-em dash: U+2E3A"));
        sizer->Add(m_checkBoxCorrectDashes);

        m_checkBoxCorrectSimpleQuotationMarks = create_3state_checkbox(sizer, _("Correct quotation marks"));
        m_checkBoxCorrectSimpleQuotationMarks->SetToolTip(_wxS("Try to \"correct 'quotation' marks\"\nExamples:\n\u201EPolish \u201Aquotation\u2019 marks\u201D\n\u201CEnglish (US) \u2018quotation\u2019 marks\u201D\n\u00AB\u2005French \u2039\u2005angle\u2005\u203A marks\u2005\u00BB\n\u201EGerman \u201Aquotation\u2018 marks\u201C"));
        sizer->Add(m_checkBoxCorrectSimpleQuotationMarks);

        m_checkBoxCapitalizedRomanLiterals = create_3state_checkbox(sizer, _("Use capitalized Roman Numerals"));
        m_checkBoxCapitalizedRomanLiterals->SetToolTip(_wxS("Examples:\nIII\u2009\u2192\u2009\u2162 (roman numeral three: U+2162)\nXII\u2009\u2192\u2009\u216B (roman numeral twelve: U+216B)"));
        sizer->Add(m_checkBoxCapitalizedRomanLiterals);

        m_checkBoxLowercaseRomanLiterals = create_3state_checkbox(sizer, _("Use lowercase Roman Numerals"));
        m_checkBoxLowercaseRomanLiterals->SetToolTip(_wxS("Examples:\niii\u2009\u2192\u2009\u2172 (small roman numeral three: U+2172)\nxii\u2009\u2192\u2009\u217B (small roman numeral twelve: U+217B"));
        sizer->Add(m_checkBoxLowercaseRomanLiterals);

        m_checkBoxAsciiToUnicode = create_3state_checkbox(sizer, _("ASCII to Unicode"));
        m_checkBoxAsciiToUnicode->SetToolTip(_wxS("Convert some ASCII sequences to single Unicode character\n\nExamples:\n(C)\u2009\u2192\u2009\u00A9\nae\u2009\u2192\u2009\u00E6\n!!\u2009\u2192\u2009\u2016\n3/5\u2009\u2192\u2009\u2157\n==\u2009\u2192\u2009\u2261\n\nRun 'cue2mkc -info ascii-to-unicode' to see complete list"));
        sizer->Add(m_checkBoxAsciiToUnicode);

        m_checkBoxNumberFullStop = create_3state_checkbox(sizer, _("Use '<number> full stop' characters"));
        m_checkBoxNumberFullStop->SetToolTip(_wxS("Use Unicode characters from \u2488 (digit one full stop: U+2488) to \u249B (number twenty full stop: U+249B)"));
        sizer->Add(m_checkBoxNumberFullStop);

        m_checkBoxSmallLetterParenthesized = create_3state_checkbox(sizer, _("use '<small letter> parenthesized' characters"));
        m_checkBoxSmallLetterParenthesized->SetToolTip(_wxS("Use Unicode characters from \u249C (parenthesized latin small letter a: U-249C) to \u24B5 (parenthesized latin small letter z: U-24B5)"));
        sizer->Add(m_checkBoxSmallLetterParenthesized);

        sizer->Add(create_horizontal_static_line(sizer), get_horizontal_static_line_sizer_flags(panel));

        m_checkBoxSmallEmDash = create_3state_checkbox(sizer, _("Use small em dash"));
        m_checkBoxSmallEmDash->SetToolTip(_wxS("When correcting dashes use '\uFE58' (U+FE58 : small em dash) character instead of '\u2014' (U+2014 : em dash) one"));
        m_checkBoxSmallEmDash->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxCorrectDashes));
        sizer->Add(m_checkBoxSmallEmDash);

        m_checkBoxStrongRomanLiteralsParser = create_3state_checkbox(sizer, _("Strong Roman Numerals parser"));
        m_checkBoxStrongRomanLiteralsParser->SetToolTip(_wxS("Use stronger roman numerals parser - include MCDL characters (XVI only by default)"));
        sizer->Add(m_checkBoxStrongRomanLiteralsParser);

        panelSizer->Add(sizer, wxGBPosition(0,1), wxDefaultSpan, wxEXPAND);
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _wxS("Matroska\u2122 titles formatting"), wxVERTICAL);

        {
            const wxString trackTitleFmt(_("%dp% - %dt% - %tt%"));
            const wxSize editMinSize = calc_text_size(panel, trackTitleFmt.Length() * 2);
            const wxSizerFlags txtSizerFlags = wxSizerFlags().Expand().Proportion(1);

            wxFlexGridSizer* const innerSizer = new wxFlexGridSizer(0, 2, FromDIP(1), 0);
            innerSizer->AddGrowableCol(1);

            m_checkBoxMkaTitleFmt = create_checkbox(sizer, _("Container"));
            m_checkBoxMkaTitleFmt->SetToolTip(_("Conainer and audio track title"));
            innerSizer->Add(m_checkBoxMkaTitleFmt, wxSizerFlags().CenterVertical());

            m_textCtrlMkaTitleFmt = create_text_ctrl(sizer, _("%dp% - %dt%"), 200);
            m_textCtrlMkaTitleFmt->SetSizeHints(editMinSize);
            m_textCtrlMkaTitleFmt->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxMkaTitleFmt));
            innerSizer->Add(m_textCtrlMkaTitleFmt, txtSizerFlags);

            m_checkBoxTrackTilteFmt = create_checkbox(sizer, _("Chapter/track"));
            innerSizer->Add(m_checkBoxTrackTilteFmt, wxSizerFlags().CenterVertical());

            m_textCtrlTrackTilteFmt = create_text_ctrl(sizer, trackTitleFmt, 200);
            m_textCtrlTrackTilteFmt->SetSizeHints(editMinSize);
            m_textCtrlTrackTilteFmt->Bind(wxEVT_UPDATE_UI, CheckBoxUiUpdater(m_checkBoxTrackTilteFmt));
            innerSizer->Add(m_textCtrlTrackTilteFmt, txtSizerFlags);

            sizer->Add(innerSizer, 0, wxEXPAND);
        }

        panelSizer->Add(sizer, wxGBPosition(1,0), wxGBSpan(1, 2), wxEXPAND | wxBOTTOM, panelGap);
    }

    panel->SetSizer(panelSizer);
    panel->FitInside();
    return panel;
}

wxPanel* wxMainFrame::create_adv_panel(wxNotebook* notebook)
{
    wxScrolledWindow* const panel = new wxScrolledWindow(notebook);
    const int panelGap = FromDIP(2);
    wxGridBagSizer* const panelSizer = new wxGridBagSizer(panelGap, panelGap);

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
        m_checkBoxRenderMultilineTags->SetToolTip(_wxS("Copy multiline tags to Matroska\u2122 container"));
        sizer->Add(m_checkBoxRenderMultilineTags);

        m_checkBoxRenderReplayGainTags = create_3state_checkbox(sizer, _("Copy ReplayGain tags"));
        m_checkBoxRenderReplayGainTags->SetToolTip(_wxS("Copy ReplayGain tags to Matroska\u2122 container"));
        sizer->Add(m_checkBoxRenderReplayGainTags);

        m_checkBoxRunReplayGainScanner = create_3state_checkbox(sizer, _("Generate ReplayGain tags"));
        m_checkBoxRunReplayGainScanner->SetToolTip(_wxS("Run ReplayGain/DR14 scanner on created Matroska\u2122 container"));
        sizer->Add(m_checkBoxRunReplayGainScanner);

        const ExclusiveCheckBoxEvtHandler evtHandler(m_checkBoxRenderReplayGainTags, m_checkBoxRunReplayGainScanner);
        evtHandler.Bind();

        m_checkBoxGenerateArtistTagsForTracks = create_3state_checkbox(sizer, _("Generate ARTIST tag for every track"), wxCHK_CHECKED);
        m_checkBoxGenerateArtistTagsForTracks->SetToolTip(_("Some media players (e.g. Foobar2000) requires ARTIST tag for every track (chapter)."));
        sizer->Add(m_checkBoxGenerateArtistTagsForTracks);

        m_checkBoxRenderITunesTags = create_3state_checkbox(sizer, _wxS("Render iTunes\u00AE tags"));
        sizer->Add(m_checkBoxRenderITunesTags);

        panelSizer->Add(sizer, wxGBPosition(0,0), wxDefaultSpan, wxEXPAND);
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Tags sources"), wxVERTICAL);

        m_checkBoxUseCdTextTags = create_3state_checkbox(sizer, _("CD-TEXT"));
        m_checkBoxUseCdTextTags->SetToolTip(_("Copy tags from CD-TEXT"));
        sizer->Add(m_checkBoxUseCdTextTags);

        m_checkBoxUseTagsFromCuesheetComments = create_3state_checkbox(sizer, _("Cuesheet comments"));
        m_checkBoxUseTagsFromCuesheetComments->SetToolTip(_("Copy tags extracted from cuesheet comments"));
        sizer->Add(m_checkBoxUseTagsFromCuesheetComments);

        m_checkBoxUseTagsFromMediaFiles = create_3state_checkbox(sizer, _("Media files"));
        m_checkBoxUseTagsFromMediaFiles->SetToolTip(_("Copy tags extracted from media files"));
        sizer->Add(m_checkBoxUseTagsFromMediaFiles);

        m_checkBoxApplyTags = create_3state_checkbox(sizer, _("Apply tags from related JSON files"));
        m_checkBoxApplyTags->SetToolTip(_("File mask: *.tags.json"));
        sizer->Add(m_checkBoxApplyTags);

        panelSizer->Add(sizer, wxGBPosition(0,1), wxDefaultSpan, wxEXPAND);
    }

    {
        wxStaticBoxSizer* const sizer = create_static_box_sizer(panel, _("Other"), wxVERTICAL);

        m_checkBoxGenerateEditionUid = create_3state_checkbox(sizer, _("Generate edition UID"));
        sizer->Add(m_checkBoxGenerateEditionUid);

        m_checkBoxFullPathInOptions = create_3state_checkbox(sizer, _("Generate full paths in options file"));
        sizer->Add(m_checkBoxFullPathInOptions);

        m_checkBoxUseMLang = create_3state_checkbox(sizer, _("Use MLang library"));
        sizer->Add(m_checkBoxUseMLang);

        panelSizer->Add(sizer, wxGBPosition(1,0), wxDefaultSpan, wxEXPAND);
    }

    panel->SetSizer(panelSizer);
    panel->FitInside();
    return panel;
}

wxPanel* wxMainFrame::create_messages_panel(wxNotebook* notebook)
{
    wxPanel* const panel = new wxPanel(notebook);
    wxBoxSizer* const panelSizer = new wxBoxSizer(wxVERTICAL);

    wxCollapsiblePane* const collapsiblePane = new wxCollapsiblePane(panel, wxID_ANY, _("Tools"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCP_NO_TLW_RESIZE);
    collapsiblePane->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
    if (collapsiblePane->GetControlWidget() != nullptr)
    {
        collapsiblePane->GetControlWidget()->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
    }
    collapsiblePane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, CollapsiblePaneUiUpdater(panel));

    {
        wxWindow* const insPane = collapsiblePane->GetPane();
        insPane->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
        {
            wxBoxSizer* const innerSizer = new wxBoxSizer(wxHORIZONTAL);
            const int gap = insPane->FromDIP(1);

            {
                wxArrayString choices;
                choices.Add(_("cue2mkc"));
                choices.Add(_("mkvmerge"));
                choices.Add(_("ffmpeg"));

                m_choiceTool = create_choice(insPane, choices);
            }

            m_choiceTool->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
            m_choiceTool->Bind(wxEVT_CHOICE, &wxMainFrame::OnToolChoice, this);
            innerSizer->Add(m_choiceTool, wxSizerFlags().CenterVertical().Border(wxRIGHT, gap));

            {
                wxArrayString choices;
                get_cmd_choices(0, choices);

                m_choiceToolParam = create_choice(insPane, choices);
            }

            m_choiceToolParam->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
            innerSizer->Add(m_choiceToolParam, wxSizerFlags().CenterVertical().Border(wxRIGHT, gap));

            {
                wxButton* const button = create_bitmap_button(insPane, "action-launch");
                button->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
                button->Bind(wxEVT_BUTTON, &wxMainFrame::OnToolExec, this);
                innerSizer->Add(button, wxSizerFlags().CenterVertical());
            }

            innerSizer->Add(0, 0, 1, wxEXPAND);

            {
                wxCheckBox* const checkBox = create_checkbox(insPane, _("Auto scroll"), m_autoScroll);
                checkBox->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
                checkBox->Bind(wxEVT_CHECKBOX, &wxMainFrame::OnCheckAutoScroll, this);
                innerSizer->Add(checkBox, wxSizerFlags().CenterVertical().Border(wxRIGHT));
            }

            {
                wxCheckBox* const checkBox = create_checkbox(insPane, _("Show timestamps"));
                checkBox->SetToolTip(_("Show/hide message timestamps"));
                checkBox->Bind(wxEVT_CHECKBOX, &wxMainFrame::OnCheckShowTimestamps, this);
                checkBox->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateRunUiCtrl, this);
                innerSizer->Add(checkBox, wxSizerFlags().CenterVertical().Border(wxLEFT));
                m_checkBoxShowTimestamps = checkBox;
            }

            insPane->SetSizer(innerSizer);
        }
        panelSizer->Add(collapsiblePane, 0, wxEXPAND | wxLEFT | wxRIGHT, FromDIP(wxSizerFlags::GetDefaultBorder()));
    }

    {
        m_listBoxMessages = new ListBox(panel);
        panelSizer->Add(m_listBoxMessages, 1, wxEXPAND | wxALL, FromDIP(wxSizerFlags::GetDefaultBorder()));
    }

    {
        wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

        {
            wxStaticText* const staticTxt = create_static_text(panel, wxEmptyString);
            staticTxt->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
            staticTxt->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMsgCnt, this);
            sizer->Add(staticTxt, wxSizerFlags().CenterVertical());
        }

        sizer->Add(0, 0, 1, wxEXPAND);

        {
            wxBitmapButton* const button = create_bitmap_button(panel, "content-content_copy");
            button->SetToolTip(_("Copy all messages to clipboard"));
            button->Bind(wxEVT_BUTTON, &wxMainFrame::OnCopyEvents, this);
            button->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateMsgCtrls, this);
            sizer->Add(button, wxSizerFlags().CenterVertical());
        }

        panelSizer->Add(sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, panel->FromDIP(wxSizerFlags::GetDefaultBorder()));
    }

    panel->SetSizer(panelSizer);
    panel->FitInside();
    return panel;
}

wxNotebook* wxMainFrame::create_notebook()
{
    wxNotebook* const notebook = new wxNotebook(this, wxID_ANY);

    notebook->AddPage(create_src_dst_pannel(notebook), _("Source and destination"), true);
    notebook->AddPage(create_general_panel(notebook), _("General options"));
    notebook->AddPage(create_chapter_panel(notebook), _("Chapters and formatting"));
    notebook->AddPage(create_adv_panel(notebook), _("Advanced options"));
    notebook->AddPage(create_messages_panel(notebook), _("Messages"));

    return notebook;
}

wxBoxSizer* wxMainFrame::create_bottom_ctrls()
{
    wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

    {
        wxBoxSizer* const innerSizer = new wxBoxSizer(wxVERTICAL);

        m_checkBoxVerbose = create_checkbox(this, _("Verbose mode"), wxLog::GetVerbose());
        m_checkBoxVerbose->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
        m_checkBoxVerbose->SetToolTip(_("Run cue2mkc in verbose mode"));
        m_checkBoxVerbose->Bind(wxEVT_CHECKBOX, &wxMainFrame::OnCheckVerbose, this);
        innerSizer->Add(m_checkBoxVerbose);

        m_checkBoxSwitchToMessagesPane = create_checkbox(this, _("Switch to Messages tab"), true);
        m_checkBoxSwitchToMessagesPane->SetWindowVariant(wxWINDOW_VARIANT_SMALL);
        m_checkBoxSwitchToMessagesPane->SetToolTip(_("Switch to Messages tab before execution"));
        innerSizer->Add(m_checkBoxSwitchToMessagesPane);

        sizer->Add(innerSizer, wxSizerFlags().CenterVertical());
    }

    sizer->Add(0, 0, 1, wxEXPAND);

    {
        wxBitmapButton* const button = create_bitmap_button(this, "action-launch", wxWINDOW_VARIANT_LARGE);
        button->SetWindowVariant(wxWINDOW_VARIANT_LARGE);
        button->SetToolTip(_("Execute (or kill) cue2mkc utility"));
        button->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateButtonRun, this);
        button->Bind(wxEVT_BUTTON, &wxMainFrame::OnExecCue2Mkc, this);
        sizer->Add(button, wxSizerFlags().CenterVertical());
    }

    return sizer;
}

wxMainFrame::wxMainFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style),
    m_pPrevLog(nullptr),
    m_logTimestamp(wxLog::GetTimestamp()),
    m_autoScroll(true)
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_FRAMEBK));
    SetIcon(wxIcon("ico_main"));

    {
        wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_notebook = create_notebook(), 1, wxEXPAND);
        sizer->Add(create_bottom_ctrls(), 0, wxEXPAND | wxALL, FromDIP(4));
        this->SetSizerAndFit(sizer);
    }

    m_pLog.reset(new LogListBox(m_listBoxMessages));
    m_pNoScrollLog.reset(new SimpleLogListBox(m_listBoxMessages));

    m_pPrevLog = wxLog::SetActiveTarget(m_autoScroll ? m_pLog.get() : m_pNoScrollLog.get());
    wxLog::DisableTimestamp();
    wxLog::EnableLogging();

    wxLogInfo(_("Simple frontend to cue2mkc utility"));
    wxLogInfo(_("Version: %s"), wxGetApp().APP_VERSION);
    wxLogInfo(_("Author: %s"), wxGetApp().GetVendorDisplayName());
    wxLogInfo(_("Operating system: %s"), wxPlatformInfo::Get().GetOperatingSystemDescription());
    wxLogInfo(_("Compiler: %s %s"), INFO_CXX_COMPILER_ID, INFO_CXX_COMPILER_VERSION);
    wxLogInfo(_("Compiled on: %s %s (%s)"), INFO_HOST_SYSTEM_NAME, INFO_HOST_SYSTEM_VERSION, INFO_HOST_SYSTEM_PROCESSOR);

    wxGetApp().ShowToolPaths();

    Bind(wxEVT_CLOSE_WINDOW, &wxMainFrame::OnClose, this);
    m_timerIdleWakeUp.Bind(wxEVT_TIMER, &wxMainFrame::OnIdleWakeupTimer, this);
    m_timerAutoScroll.Bind(wxEVT_TIMER, &wxMainFrame::OnAutoScrollTimer, this);

    SetDropTarget(new DropTarget(this));
}

void wxMainFrame::OnCopyEvents(wxCommandEvent& WXUNUSED(event))
{
    const wxString txt = m_listBoxMessages->GetItemsAsText();
    if (txt.IsEmpty()) return;

    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(txt));
        wxTheClipboard->Close();
    }
    else
    {
        wxLogWarning(_("Cannot copy data to clipboard"));
    }
}

namespace
{
    void get_cmd(const wxFileName& exe, const wxString& params, wxString& cmd, wxString& cmdDesc)
    {
        cmd.Clear();
        cmd << '"' << exe.GetFullPath() << "\" " << params;

        cmdDesc.Clear();
        cmdDesc << exe.GetName() << ' ' << params;
    }
}

void wxMainFrame::ExecuteCmd(const wxFileName& exe, const wxString& params, const wxString& cwd)
{
    if (m_pProcess)
    {
        wxLogWarning(_("exe: Unable to execute command %s %s"), exe.GetName(), params);
        return;
    }

    if (m_checkBoxSwitchToMessagesPane->GetValue()) m_notebook->ChangeSelection(m_notebook->GetPageCount() - 1);

    wxExecuteEnv env;
    env.cwd = cwd;

    MyProcess* const pProcess = new MyProcess;

    m_pProcess.reset(pProcess);
    m_pProcess->Bind(wxEVT_END_PROCESS, &wxMainFrame::OnProcessTerminated, this);

    wxString cmd, cmdDesc;
    get_cmd(exe, params, cmd, cmdDesc);

    const long pid = wxExecute(cmd, wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER, m_pProcess.get(), &env);

    if (pid == 0)
    {
        wxLogError(_("exe[b]: fail: %s"), cmdDesc);
        m_pProcess.reset();
        return;
    }

    m_listBoxMessages->Clear();

    wxLogInfo(_("exe[b]: pid: %ld, cmd: %s"), pid, cmdDesc);

    m_pProcess->CloseOutput();
    pProcess->CreateTxtStreams();
    wxLog::SetActiveTarget(m_pNoScrollLog.get());
    Bind(wxEVT_IDLE, &wxMainFrame::OnIdle, this);

    m_timerIdleWakeUp.Start(TIMER_IDLE_WAKE_UP_INTERVAL);
    if (m_autoScroll) m_timerAutoScroll.Start(AUTO_SCROLL_UPDATE_INTERVAL);
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
    const wxString cmdDesc = wxString::Format("taskkill /pid %ld", pid);

    wxProcess* const process = new wxProcess();
    process->SetPriority(wxPRIORITY_MIN);
    m_pTaskKillProcess.reset(process);
    process->Bind(wxEVT_END_PROCESS, &wxMainFrame::OnTaskKillProcessTerminated, this);

    const long killPid = wxExecute(cmd, wxEXEC_HIDE_CONSOLE, process);

    if (killPid == 0)
    {
        wxLogError(_("kill[b]: fail: %s"), cmdDesc);
        m_pTaskKillProcess.reset();
        return;
    }

    wxLogInfo(_("kill[b]: pid: %ld, cmd: %s"), killPid, cmdDesc);
}

void wxMainFrame::OnTaskKillProcessTerminated(wxProcessEvent& event)
{
    if (m_pTaskKillProcess->GetPid() != event.GetPid())
    {
        wxLogError(_("kill[unk]: pid: %d, exit code: %d [%08x]"), event.GetPid(), event.GetExitCode(), event.GetExitCode());
        return;
    }

    if (event.GetExitCode() != 0)
    {
        wxLogInfo(_("kill[f]: pid: %d, exit code: %d [%08x]"), event.GetPid(), event.GetExitCode(), event.GetExitCode());
    }
    m_pTaskKillProcess.reset();
}

#endif


void wxMainFrame::ExecuteCue2Mkc(const wxArrayString& args, bool withVerbose)
{
    wxArrayString params;

    params.Add("--output-charset");
    params.Add("utf-8");

    negatable_long_switch_option(params, m_checkBoxShowTimestamps, "log-timestamps");
    if (withVerbose && wxLog::GetVerbose()) params.Add("--verbose");

    params.insert(params.end(), args.begin(), args.end());
    ExecuteCue2Mkc(options_to_str(params), false);
}

void wxMainFrame::ExecuteCue2Mkc(const wxString& args, bool withVerbose)
{
    const wxFileName& cue2mkc = wxGetApp().GetCue2MkcPath();

    if (withVerbose && wxLog::GetVerbose())
    {
        ExecuteCmd(cue2mkc, wxString::Format("--verbose %s", args), cue2mkc.GetPath());
    }
    else
    {
        ExecuteCmd(cue2mkc, args, cue2mkc.GetPath());
    }
}

void wxMainFrame::ExecuteMkvmerge(const wxString& args)
{
    const wxFileName& mkvmerge = wxGetApp().GetMkvmergePath();
    ExecuteCmd(mkvmerge, args, wxEmptyString);
}

void wxMainFrame::ExecuteMkvmerge(const wxArrayString& args)
{
    wxArrayString params;

    params.Add("--ui-language");
    params.Add("en");

    params.Add("--output-charset");
    params.Add("utf-8");

    params.insert(params.end(), args.begin(), args.end());

    ExecuteMkvmerge(options_to_str(params));
}

void wxMainFrame::ExecuteFfmpeg(const wxString& args)
{
    const wxFileName& ffmpeg = wxGetApp().GetFfmpegPath();
    ExecuteCmd(ffmpeg, args, wxEmptyString);
}

void wxMainFrame::ExecuteFfmpeg(const wxArrayString& args)
{
    wxArrayString params;

    params.Add("-hide_banner");
    params.Add("-nostdin");
    params.Add("-nostats");

    params.insert(params.end(), args.begin(), args.end());

    ExecuteFfmpeg(options_to_str(params));
}

void wxMainFrame::ExecuteFfprobe(const wxString& args)
{
    const wxFileName& ffmpeg = wxGetApp().GetFfprobePath();
    ExecuteCmd(ffmpeg, args, wxEmptyString);
}

void wxMainFrame::ExecuteFfprobe(const wxArrayString& args)
{
    wxArrayString params;

    params.Add("-hide_banner");
    params.Add("-v");
    params.Add("error");

    params.insert(params.end(), args.begin(), args.end());

    ExecuteFfprobe(options_to_str(params));
}

void wxMainFrame::ProcessOutErr(bool once)
{
    wxASSERT(m_pProcess);
    const MyProcess* const pProcess = static_cast<MyProcess*>(m_pProcess.get());

    bool         processOutErr = pProcess->HaveOutOrErr();
    const wxChar c = once ? wxS('r') : wxS('f');

    while (processOutErr)
    {
        if (pProcess->IsInputAvailable() && !pProcess->GetTxtInputStream().GetInputStream().Eof())
        {
            const wxString line = pProcess->GetTxtInputStream().ReadLine();
            wxLogInfo("out[%c]: %s", c, line);
        }

        if (pProcess->IsErrorAvailable() && !pProcess->GetTxtErrorStream().GetInputStream().Eof())
        {
            const wxString line = pProcess->GetTxtErrorStream().ReadLine();
            wxLogInfo("err[%c]: %s", c, line);
        }

        processOutErr = once ? false : pProcess->HaveOutOrErr();
    }
}

void wxMainFrame::OnProcessTerminated(wxProcessEvent& event)
{
    if (m_pProcess->GetPid() != event.GetPid())
    {
        wxLogError(wxT("exe[unk]: pid: %d, exit code: %d [%08x]"), event.GetPid(), event.GetExitCode(), event.GetExitCode());
        return;
    }

    m_timerIdleWakeUp.Stop();
    m_timerAutoScroll.Stop();

    ProcessOutErr();

    wxLog::SetActiveTarget(m_autoScroll ? m_pLog.get() : m_pNoScrollLog.get());

    wxLogInfo(_("exe[f]: pid: %d, exit code: %d [%08x]"), event.GetPid(), event.GetExitCode(), event.GetExitCode());
    m_pProcess.reset();
    Unbind(wxEVT_IDLE, &wxMainFrame::OnIdle, this);
}

void wxMainFrame::OnIdleWakeupTimer(wxTimerEvent& WXUNUSED(event))
{
    if (!m_pProcess) return;

    wxWakeUpIdle();
}

void wxMainFrame::OnAutoScrollTimer(wxTimerEvent& WXUNUSED(event))
{
    m_listBoxMessages->ShowLastItem();
}

void wxMainFrame::OnIdle(wxIdleEvent& event)
{
    if (!m_pProcess) return;

    ProcessOutErr(true);

    MyProcess* const pProcess = static_cast<MyProcess*>(m_pProcess.get());

    event.RequestMore(pProcess->HaveOutOrErr());
}

void wxMainFrame::OnUpdateButtonRun(wxUpdateUIEvent& event)
{
    if (m_pProcess)
    {
    #ifdef __WXMSW__
        event.Enable(!m_pTaskKillProcess);
    #else
        event.Enable(true);
    #endif
        return;
    }

    if (!m_treeCtrlInputFiles->HasChildren(m_treeCtrlInputFilesRoot))
    {
        event.Enable(false);
        return;
    }

    if (m_choiceDst->GetSelection() == 0) event.Enable(true);
    else event.Enable(!m_textCtrlDst->IsEmpty());
}

void wxMainFrame::OnUpdateRunUiCtrl(wxUpdateUIEvent& event)
{
    event.Enable(!m_pProcess);
}

void wxMainFrame::OnUpdateMsgCtrls(wxUpdateUIEvent& event)
{
    event.Enable(!m_pProcess && (m_listBoxMessages->GetCount() > 0));
}

namespace
{
    bool tree_ctrl_with_data_files(const wxTreeCtrl& treeCtrl, const wxTreeItemId& rootId)
    {
        wxTreeItemIdValue cookie;
        for (wxTreeItemId i = treeCtrl.GetFirstChild(rootId, cookie); i.IsOk(); i = treeCtrl.GetNextChild(i, cookie))
        {
            if (treeCtrl.HasChildren(i)) return true;
        }
        return false;
    }
}

bool wxMainFrame::read_options(wxArrayString& options) const
{
    negatable_switch_option(options, m_checkBoxAbortOnErrors, 'a');
    negatable_switch_option(options, m_checkBoxJoinMode, 'j');
    negatable_switch_option(options, m_checkBoxIncludeDiscNumberTag, "dn");
    negatable_switch_option(options, m_checkBoxUseMediaFiles, "df");
    negatable_long_switch_option(options, m_checkBoxMediaFilesWithoutCue, "single-media-file");

    if (!m_textCtrlExt->IsEmpty())
    {
        options.Add("-x");
        options.Add(m_textCtrlExt->GetValue());
    }

    options.Add("-m");
    switch (m_choiceFormat->GetSelection())
    {
        case 0:
        options.Add("cuesheet");
        break;

        case 1:
        options.Add("mkvmerge-chapters");
        break;

        case 2:
        options.Add("mkvmerge");
        break;

        case 3:
        options.Add("ffmpeg-chapters");
        break;

        case 4:
        options.Add("ffmpeg");
        break;

        case 5:
        options.Add("wav2img");	// deprecated
        break;
    }

    if (!m_textCtrlLang->wxTextEntryBase::IsEmpty())
    {
        const wxString lang = m_textCtrlLang->GetValue().Lower();

        if (lang.Cmp("und") != 0)
        {
            options.Add("-l");
            options.Add(lang);
        }
    }

    negatable_switch_option(options, m_checkBoxRunTool, "xt");

    options.Add("-e");
    switch (m_choiceEncoding->GetSelection())
    {
        case 0:
        {
            options.Add("default");
            break;
        }

        case 1:
        {
            options.Add("utf8");
            break;
        }

        case 2:
        {
            options.Add("utf8bom");
            break;
        }

        case 3:
        {
            options.Add("utf16");
            break;
        }

        case 4:
        {
            options.Add("utf16bom");
            break;
        }

        case 5:
        {
            options.Add("utf16be");
            break;
        }

        case 6:
        {
            options.Add("utf16bebom");
            break;
        }
    }

    negatable_switch_option(options, m_checkBoxCorrectSimpleQuotationMarks, "cq");
    negatable_switch_option(options, m_checkBoxParseTagsFromCuesheetComments, "tc");
    negatable_switch_option(options, m_checkBoxEllipsizeTags, "et");
    negatable_switch_option(options, m_checkBoxRemoveExtraSpaces, "rs");
    negatable_long_switch_option(options, m_checkBoxCorrectDashes, "correct-dashes");
    negatable_long_switch_option(options, m_checkBoxSmallEmDash, "small-em-dash");
    negatable_long_switch_option(options, m_checkBoxNumberFullStop, "number-full-stop");
    negatable_long_switch_option(options, m_checkBoxSmallLetterParenthesized, "small-letter-parenthesized");
    negatable_long_switch_option(options, m_checkBoxAsciiToUnicode, "ascii-to-unicode");

    negatable_switch_option(options, m_checkBoxCapitalizedRomanLiterals, "ru");
    negatable_switch_option(options, m_checkBoxLowercaseRomanLiterals, "rl");
    negatable_long_switch_option(options, m_checkBoxStrongRomanLiteralsParser, "strong-roman-numerals-parser");

    negatable_long_switch_option(options, m_checkBoxUseCdTextTags, "use-cdtext-tags");
    negatable_long_switch_option(options, m_checkBoxUseTagsFromCuesheetComments, "use-cue-comments-tags");
    negatable_long_switch_option(options, m_checkBoxUseTagsFromMediaFiles, "use-media-tags");

    negatable_switch_option(options, m_checkBoxGenerateEditionUid, "eu");
    negatable_long_switch_option(options, m_checkBoxReadMedatata, "read-media-tags");
    negatable_switch_option(options, m_checkBoxRenderMultilineTags, "rm");
    negatable_switch_option(options, m_checkBoxRenderReplayGainTags, "rg");
    negatable_long_switch_option(options, m_checkBoxRunReplayGainScanner, "run-replaygain-scanner");
    negatable_long_switch_option(options, m_checkBoxRenderITunesTags, "render-itunes-tags");
    negatable_long_switch_option(options, m_checkBoxUseMLang, "mlang");
    negatable_switch_option(options, m_checkBoxGenerateArtistTagsForTracks, "ra");
    negatable_switch_option(options, m_checkBoxChapterEndTime, "ce");
    negatable_switch_option(options, m_checkBoxChapterEndTimeFromNext, "cn");

    if (is_checked(m_checkBoxOffset) && !m_textCtrlChapterOffset->IsEmpty())
    {
        int offset;

        if (m_textCtrlChapterOffset->GetValue().ToInt(&offset))
        {
            options.Add("-fo");
            options.Add(m_textCtrlChapterOffset->GetValue());
        }
        else
        {
            wxLogError("Invalid chapter offset value");
            return false;
        }
    }

    if (is_checked(m_checkBoxTrack01Idx))
    {
        switch (m_choiceIdx->GetSelection())
        {
            case 0:
            options.Add("-t1i0");
            break;

            case 1:
            options.Add("-t1i0-");
            break;
        }
    }

    negatable_long_switch_option(options, m_checkBoxIdxToHiddenChapters, "hidden-indexes");

    if (is_checked(m_checkBoxTrackTilteFmt) && !m_textCtrlTrackTilteFmt->IsEmpty())
    {
        options.Add("-f");
        options.Add(m_textCtrlTrackTilteFmt->GetValue());
    }

    if (is_checked(m_checkBoxMkaTitleFmt) && !m_textCtrlMkaTitleFmt->IsEmpty())
    {
        options.Add("-mf");
        options.Add(m_textCtrlMkaTitleFmt->GetValue());
    }

    negatable_long_switch_option(options, m_checkBoxAttachLogs, "attach-eac-log");
    negatable_long_switch_option(options, m_checkBoxAttachAccuRip, "attach-accurip-log");
    negatable_long_switch_option(options, m_checkBoxAttachCover, "attach-cover");
    negatable_switch_option(options, m_checkBoxAttachmentsInParentDir, "pd");
    negatable_long_switch_option(options, m_checkBoxApplyTags, "apply-tags");
    negatable_long_switch_option(options, m_checkBoxConvertCover, "convert-cover-file");
    negatable_long_switch_option(options, m_checkBoxCoverFromPdf, "cover-from-pdf");

    switch (m_choiceConvertedImageExt->GetSelection())
    {
        case 1:
        options.Add("--cover-file-ext");
        options.Add("jpg");
        break;

        case 2:
        options.Add("--cover-file-ext");
        options.Add("webp");
        break;
    }

    if (m_choiceCueSheetAttachMode->GetSelection() > 0)
    {
        options.Add("--cue-sheet-attach-mode");
        options.Add(m_choiceCueSheetAttachMode->GetStringSelection());
    }

    if (m_choiceFfmpegCodec->GetSelection() > 0)
    { // non-default
        const wxString val = m_choiceFfmpegCodec->GetStringSelection();
        options.Add("--ffmpeg-codec");
        options.Add(val);
    }

    if (m_choiceAudioSampleWidth->GetSelection() > 0)
    {
        switch (m_choiceAudioSampleWidth->GetSelection())
        {
            case 1:
            options.Add("--audio-sample-width");
            options.Add("16");
            break;

            case 2:
            options.Add("--audio-sample-width");
            options.Add("24");
            break;
        }
    }

    negatable_switch_option(options, m_checkBoxDowngradeHiResAudio, "dg");
    negatable_long_switch_option(options, m_checkBoxFullPathInOptions, "use-full-paths");
    negatable_long_switch_option(options, m_checkBoxMono, "mono");

    switch (m_choiceDst->GetSelection())
    {
        case 1:
        {
            if (!m_textCtrlDst->IsEmpty())
            {
                options.Add("-od");
                options.Add(m_textCtrlDst->GetValue());
            }
            break;
        }

        case 2:
        {
            if (!m_textCtrlDst->IsEmpty())
            {
                options.Add("-o");
                options.Add(m_textCtrlDst->GetValue());
            }
            break;
        }
    }

    if (!m_treeCtrlInputFiles->HasChildren(m_treeCtrlInputFilesRoot))
    {
        wxLogError(_("No input files"));
        return false;
    }

    if (tree_ctrl_with_data_files(*m_treeCtrlInputFiles, m_treeCtrlInputFilesRoot))
    {
        options.Add("-ds");
    }

    wxTreeItemIdValue cookie;

    for (wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild(m_treeCtrlInputFilesRoot, cookie); i.IsOk(); i = m_treeCtrlInputFiles->GetNextChild(i, cookie))
    {
        if (m_treeCtrlInputFiles->HasChildren(i))
        {
            wxArrayString     mediaFiles;
            wxTreeItemIdValue mediaCookie;

            for (wxTreeItemId j = m_treeCtrlInputFiles->GetFirstChild(m_treeCtrlInputFilesRoot, mediaCookie); j.IsOk(); j = m_treeCtrlInputFiles->GetNextChild(j, mediaCookie))
            {
                mediaFiles.Add(m_treeCtrlInputFiles->GetItemText(j));
            }

            wxString s = m_treeCtrlInputFiles->GetItemText(i);
            s.Append(join_strings(mediaFiles, ';'));

            options.Add(s);
        }
        else
        {
            options.Add(m_treeCtrlInputFiles->GetItemText(i));
        }
    }

    return true;
}

void wxMainFrame::OnExecCue2Mkc(wxCommandEvent& WXUNUSED(event))
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

    if (!read_options(params)) return;

    ExecuteCue2Mkc(params);
}

void wxMainFrame::OnCheckVerbose(wxCommandEvent& event)
{
    wxLog::SetVerbose(event.IsChecked());
}

void wxMainFrame::OnCheckShowTimestamps(wxCommandEvent& event)
{
    if (event.IsChecked()) wxLog::SetTimestamp(m_logTimestamp);
    else wxLog::DisableTimestamp();
}

void wxMainFrame::AddMainItem(const wxString& fileName, bool selectItem)
{
    wxTreeItemId itemId = m_treeCtrlInputFiles->AppendItem(m_treeCtrlInputFilesRoot, fileName);
    wxASSERT(itemId.IsOk());
    if (selectItem)
    {
        m_treeCtrlInputFiles->SelectItem(itemId);
    }
}

void wxMainFrame::SuggestCommonDirPath()
{
    if (!m_checkBoxSuggestJoin->GetValue()) return;
    if (m_choiceDst->GetSelection() >= 2) return;

    wxArrayString     files;
    wxTreeItemIdValue cookie;

    for (wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild(m_treeCtrlInputFilesRoot, cookie); i.IsOk(); i = m_treeCtrlInputFiles->GetNextChild(i, cookie))
    {
        files.Add(m_treeCtrlInputFiles->GetItemText(i));
    }

    const wxString commonPath = find_common_path(files);

    if (commonPath.IsEmpty()) return;

    m_textCtrlDst->SetValue(commonPath);
}

void wxMainFrame::OnButtonAdd(wxCommandEvent& WXUNUSED(event))
{
    wxFileDialog openFileDialog(this,
                                 _("Specify input file"),
                                 wxEmptyString, wxEmptyString,
                                 _("CUE files|*.cue|Text files|*.txt|M3U Playlists|*.m3u;*.m3u8|Audio files|*.flac;*.wv;*.wav;*.aiff|All files|*"),
                                 wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() != wxID_OK) return;

    wxFileName fileName;
    fileName.AssignDir(openFileDialog.GetDirectory());

    wxArrayString fileNames;
    openFileDialog.GetFilenames(fileNames);

    {
        wxWindowUpdateLocker locker(m_notebook);
        for (wxArrayString::const_iterator i = fileNames.begin(); i != fileNames.end(); ++i)
        {
            fileName.SetFullName(*i);
            AddMainItem(fileName.GetFullPath());
        }

        SuggestCommonDirPath();
    }
}

void wxMainFrame::OnSuggestJoinMode(wxCommandEvent& event)
{
    if (event.IsChecked())
    {
        m_checkBoxJoinMode->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateJoinMode, this);
        m_checkBoxMediaFilesWithoutCue->Bind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateUseMediaFilesWithoutCue, this);
    }
    else
    {
        m_checkBoxJoinMode->Unbind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateJoinMode, this);
        m_checkBoxMediaFilesWithoutCue->Unbind(wxEVT_UPDATE_UI, &wxMainFrame::OnUpdateUseMediaFilesWithoutCue, this);
    }
}

namespace
{
    bool is_m3u_ext(const wxFileName& fn)
    {
        const wxString& ext = fn.GetExt();
        return wxStricmp(ext, "m3u") == 0 || wxStricmp(ext, "m3u8") == 0;
    }

    bool is_media_ext(const wxFileName& fn)
    {
        const wxString& ext = fn.GetExt();
        return wxStricmp(ext, "wav") == 0 ||
            wxStricmp(ext, "wv") == 0 ||
            wxStricmp(ext, "flac") == 0 ||
            wxStricmp(ext, "aiff") == 0;
    }
}

void wxMainFrame::OnUpdateJoinMode(wxUpdateUIEvent& event)
{
    wxASSERT(m_checkBoxSuggestJoin->GetValue());

    wxTreeItemIdValue cookie;
    int               cnt = 0;
    bool m3u = false;

    for (wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild(m_treeCtrlInputFilesRoot, cookie);
         i.IsOk() && !(m3u || (cnt >= 2));
         i = m_treeCtrlInputFiles->GetNextChild(i, cookie), ++cnt)
    {
        const wxFileName fn(m_treeCtrlInputFiles->GetItemText(i));
        m3u = is_m3u_ext(fn);
    }

    event.Check(m3u || (cnt >= 2));
}

void wxMainFrame::OnUpdateUseMediaFilesWithoutCue(wxUpdateUIEvent& event)
{
    wxASSERT(m_checkBoxSuggestJoin->GetValue());

    wxTreeItemIdValue cookie;
    int               cnt = 0;

    for (wxTreeItemId i = m_treeCtrlInputFiles->GetFirstChild(m_treeCtrlInputFilesRoot, cookie);
         i.IsOk() && (cnt == 0);
         i = m_treeCtrlInputFiles->GetNextChild(i, cookie))
    {
        const wxFileName fn(m_treeCtrlInputFiles->GetItemText(i));
        if (is_m3u_ext(fn) || is_media_ext(fn))
        {
            cnt += 1;
        }
    }

    event.Check(cnt > 0);
}


void wxMainFrame::OnDropFiles(const wxArrayString& fileNames)
{
    wxWindowUpdateLocker locker(m_notebook);

    m_notebook->ChangeSelection(0);
    for (wxArrayString::const_iterator i = fileNames.begin(); i != fileNames.end(); ++i)
    {
        AddMainItem(*i, false);
    }
    SuggestCommonDirPath();
}

void wxMainFrame::OnButtonDelete(wxCommandEvent& WXUNUSED(event))
{
    const wxTreeItemId itemId = m_treeCtrlInputFiles->GetSelection();
    const bool         suggestCommonDirPath = (m_treeCtrlInputFiles->GetItemParent(itemId) == m_treeCtrlInputFilesRoot);

    if (!itemId.IsOk()) return;

    m_treeCtrlInputFiles->Delete(itemId);

    if (!suggestCommonDirPath) return;

    SuggestCommonDirPath();
}

void wxMainFrame::OnButtonMakeMask(wxCommandEvent& WXUNUSED(event))
{
    const wxTreeItemId itemId = m_treeCtrlInputFiles->GetSelection();

    if (!itemId.IsOk()) return;

    const wxString txt(make_fmask(m_treeCtrlInputFiles->GetItemText(itemId)));

    m_treeCtrlInputFiles->SetItemText(itemId, txt);
}

void wxMainFrame::AddChildItem(const wxTreeItemId& parentId, const wxString& fileName)
{
    wxASSERT(parentId.IsOk());
    wxASSERT(parentId != m_treeCtrlInputFilesRoot);

    wxTreeItemId childId = m_treeCtrlInputFiles->AppendItem(parentId, fileName);

    wxASSERT(childId.IsOk());

    m_treeCtrlInputFiles->ExpandAllChildren(parentId);
    m_treeCtrlInputFiles->SelectItem(childId);
}

void wxMainFrame::OnButtonAddMediaFile(wxCommandEvent& WXUNUSED(event))
{
    const wxTreeItemId itemId = m_treeCtrlInputFiles->GetSelection();

    if (!itemId.IsOk()) return;

    const wxTreeItemId parentItem = m_treeCtrlInputFiles->GetItemParent(itemId);
    const wxTreeItemId cueItem = (parentItem == m_treeCtrlInputFilesRoot) ? itemId : parentItem;

    wxFileName fileName(m_treeCtrlInputFiles->GetItemText(cueItem));

    wxFileDialog openFileDialog(this,
                                 _("Specify media file"),
                                 fileName.GetPath(),
                                 wxEmptyString,
                                 _("Audio files|*.flac;*.wv;*.wav;*.aiff|All files|*"),
                                 wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() != wxID_OK) return;

    fileName.AssignDir(openFileDialog.GetDirectory());

    wxArrayString fileNames;

    openFileDialog.GetFilenames(fileNames);
    for (wxArrayString::const_iterator i = fileNames.begin(); i != fileNames.end(); ++i)
    {
        fileName.SetFullName(*i);
        AddChildItem(cueItem, fileName.GetFullPath());
    }
}

void wxMainFrame::OnButtonIdentifyMediaFile(wxCommandEvent& WXUNUSED(event))
{
    const wxTreeItemId itemId = m_treeCtrlInputFiles->GetSelection();

    if (!itemId.IsOk()) return;

    bool useFfprobe = false;
    switch (m_choiceFormat->GetSelection())
    {
        case 3:
        case 4:
        useFfprobe = true;
        break;
    }

    wxArrayString args;
    if (useFfprobe)
    {
        args.Add("-bitexact");
        args.Add("-show_format");
        args.Add("-show_streams");
        args.Add("-show_chapters");
        args.Add("-of");
        args.Add("json");
        args.Add(m_treeCtrlInputFiles->GetItemText(itemId));
        ExecuteFfprobe(args);
    }
    else
    {
        args.Add("--identify");
        args.Add("-F");
        args.Add("json");
        args.Add(m_treeCtrlInputFiles->GetItemText(itemId));
        ExecuteMkvmerge(args);
    }
}

void wxMainFrame::OnUpdateDst(wxUpdateUIEvent& event)
{
    event.Enable(m_treeCtrlInputFiles->HasChildren(m_treeCtrlInputFilesRoot));
}

void wxMainFrame::OnUpdateCtrlDst(wxUpdateUIEvent& event)
{
    event.Enable(m_choiceDst->GetSelection() > 0);
}

void wxMainFrame::OnChooseDst(wxCommandEvent& WXUNUSED(event))
{
    switch (m_choiceDst->GetSelection())
    {
        case 1:	// directory
        {
            int      extraFlags = 0;
            wxString initDir = wxEmptyString;

            const wxString dir = m_textCtrlDst->GetValue();

            if (!dir.IsEmpty())
            {
                wxFileName fnDir = wxFileName::DirName(dir);

                if (fnDir.DirExists())
                {
                    extraFlags |= wxDD_CHANGE_DIR;
                    initDir = fnDir.GetFullPath();
                }
            }

            wxDirDialog dlgDir(this,
                                _("Specify destination directory"),
                                initDir,
                                wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST | extraFlags);

            if (dlgDir.ShowModal() == wxID_OK) m_textCtrlDst->SetValue(dlgDir.GetPath());
            break;
        }

        case 2:	// file
        {
            wxFileDialog dlgFile(this,
                                  _("Specify output file"),
                                  wxEmptyString,
                                  wxEmptyString,
                                  _("XML files|*.xml|Matroska chapters XML files|*.mkc.xml|Ffmpeg metadata files|*.ffm.txt|Matroska containers|*.mka|CUE files|*.cue|Text files|*.txt|All files|*"),
                                  wxFD_SAVE);

            if (dlgFile.ShowModal() == wxID_OK) m_textCtrlDst->SetValue(dlgFile.GetPath());
            break;
        }
    }
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
            txt = wxString::Format(_wxS("pid:\u2009%ld [killed],\u2009"), m_pProcess->GetPid());
        }
        else
        {
            txt = wxString::Format(_wxS("pid:\u2009%ld,\u2009"), m_pProcess->GetPid());
        }
    #else
        txt = wxString::Format(_wxS("pid:\u2009%ld,\u2009"), m_pProcess->GetPid());
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
        txt << wxS("\u2009\u203C");
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

void wxMainFrame::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto() && m_pProcess)
    {
        wxLogWarning(_("Vetoing window close request"));
        event.Veto();
        return;
    }

    if (m_pProcess)
    {
    #ifdef __WXMSW__
        kill_console_process(*m_pProcess);
    #else
        wxLogWarning(_("exe[c]: kill %ld"), m_pProcess->GetPid());
        const wxKillError res = wxProcess::Kill(m_pProcess->GetPid(), wxSIGKILL, wxKILL_CHILDREN);
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

    if (m_timerIdleWakeUp.IsRunning()) m_timerIdleWakeUp.Stop();
    if (m_timerAutoScroll.IsRunning()) m_timerAutoScroll.Stop();

    wxLog::SetTimestamp(m_logTimestamp);
    wxLog::EnableLogging(false);
    wxLog::SetActiveTarget(m_pPrevLog);

    Destroy();
}

void wxMainFrame::get_cmd_choices(int choice, wxArrayString& choices)
{
    choices.Empty();
    switch (choice)
    {
        case 0: // cue2mkc
        choices.Add("help");
        choices.Add("version");
        choices.Add("usage");
        choices.Add("tools");
        choices.Add("ascii-to-unicode");
        choices.Add("formatting");
        choices.Add("license");
        break;

        case 1: // mkvmerge
        choices.Add(_("help"));
        choices.Add(_("version"));
        choices.Add(_("list-languages"));
        choices.Add(_("list-types"));
        break;

        case 2: // ffmpeg
        choices.Add("help");
        choices.Add("version");
        choices.Add("long help");
        choices.Add("full help");
        choices.Add("license");
        choices.Add("build");
        choices.Add("codecs");
        choices.Add("wavpack encoder");
        choices.Add("flac encoder");
        choices.Add("opus encoder");
        choices.Add("muxers");
        choices.Add("matroska muxer");
        choices.Add("filters");
        choices.Add("pan filter");
        choices.Add("concat filter");
        choices.Add("replaygain filter");
        choices.Add("drmeter filter");
        choices.Add("ebur128 filter");
        choices.Add("aresample filter");
        break;

        default:
        wxASSERT(false);
    }
}

void wxMainFrame::OnToolChoice(wxCommandEvent& evt)
{
    wxArrayString choices;
    get_cmd_choices(evt.GetSelection(), choices);

    {
        wxWindowUpdateLocker locker(m_choiceToolParam);
        m_choiceToolParam->Clear();
        m_choiceToolParam->Append(choices);
        m_choiceToolParam->SetSelection(0);
    }
    m_choiceToolParam->GetParent()->Layout();
}

void wxMainFrame::OnToolExec(wxCommandEvent& evt)
{
     switch (m_choiceTool->GetSelection())
    {
        case 0:
        {
            wxArrayString params;

            if (m_choiceToolParam->GetSelection() == 0)
            {
                params.Add("--help");
            }
            else
            {
                params.Add("--info");
                params.Add(m_choiceToolParam->GetStringSelection());
            }

            ExecuteCue2Mkc(params, false);
            break;
        }

        case 1:
        {
            wxArrayString args;

            wxString longOption = m_choiceToolParam->GetStringSelection();
            longOption.Prepend("--");

            args.Add(longOption);

            ExecuteMkvmerge(args);
            break;
        }

        case 2:
         {
            wxArrayString args;
            switch (m_choiceToolParam->GetSelection())
            {
                case 1: // -version
                args.Add("-version");
                break;

                case 2: // long help
                args.Add("-h");
                args.Add("long");
                break;

                case 3: // full help
                args.Add("-h");
                args.Add("full");
                break;

                case 4: // license
                args.Add("-L");
                break;

                case 5: // buildconf
                args.Add("-buildconf");
                break;

                case 6:
                args.Add("-codecs");
                break;

                case 7:
                args.Add("-h");
                args.Add("encoder=wavpack");
                break;

                case 8:
                args.Add("-h");
                args.Add("encoder=flac");
                break;

                case 9:
                args.Add("-h");
                args.Add("encoder=libopus");
                break;

                case 10:
                args.Add("-muxers");
                break;

                case 11:
                args.Add("-h");
                args.Add("muxer=matroska");
                break;

                case 12:
                args.Add("-filters");
                break;

                case 13:
                args.Add("-h");
                args.Add("filter=pan");
                break;

                case 14:
                args.Add("-h");
                args.Add("filter=concat");
                break;

                case 15:
                args.Add("-h");
                args.Add("filter=replaygain");
                break;

                case 16:
                args.Add("-h");
                args.Add("filter=drmeter");
                break;

                case 17:
                args.Add("-h");
                args.Add("filter=ebur128");
                break;

                case 18:
                args.Add("-h");
                args.Add("filter=aresample");
                break;

                default:
                case 0: // -help
                args.Add("-help");
                break;
            }

            ExecuteFfmpeg(args);
            break;
        }
    }
}
