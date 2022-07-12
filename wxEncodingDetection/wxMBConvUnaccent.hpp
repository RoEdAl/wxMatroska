// ============================================================================
// wxMBConvUnaccent
// ============================================================================

class wxMBConvUnaccent: public wxMBConv
{
    private:

    static const long US_ASCII = 20127;
    static const char REPL ;
    size_t m_minMBCharWidth;

    public:

    wxMBConvUnaccent()
        :m_minMBCharWidth(0)
    {
    }

    wxMBConvUnaccent(const wxMBConvUnaccent& conv)
        : wxMBConv(), m_minMBCharWidth(conv.m_minMBCharWidth)
    {
    }

    virtual size_t MB2WC(wchar_t* buf, const char* psz, size_t n) const wxOVERRIDE
    {
        const size_t len = ::MultiByteToWideChar(US_ASCII, MB_ERR_INVALID_CHARS,
            psz, -1,
            buf, buf ? n : 0);
        if (len == 0)
        {
            return wxCONV_FAILED;
        }

        return len - 1;
    }

    virtual size_t WC2MB(char* buf, const wchar_t* pwz, size_t n) const wxOVERRIDE
    {
        BOOL usedDef wxDUMMY_INITIALIZE(false);

        const size_t len = ::WideCharToMultiByte(US_ASCII,
            WC_COMPOSITECHECK | WC_DISCARDNS,
            pwz, -1,
            buf, buf ? n : 0,
            &REPL, &usedDef
        );

        if (len == 0)
        {
            return wxCONV_FAILED;
        }

        if (!buf)
        {
            const wxCharBuffer bufDef = wxCharBuffer(len);
            buf = wxConstCast(bufDef.data(), char);
            if (!::WideCharToMultiByte(US_ASCII,
                WC_COMPOSITECHECK | WC_DISCARDNS,
                pwz, -1,
                buf, len,
                &REPL, nullptr))
            {
                return wxCONV_FAILED;
            }
        }

        return len - 1;
    }

    virtual size_t GetMBNulLen() const wxOVERRIDE
    {
        if (m_minMBCharWidth == 0)
        {
            int len = ::WideCharToMultiByte(
                US_ASCII,
                0,
                wxS(""), 1,
                nullptr, 0,
                nullptr,
                nullptr
            );

            wxMBConvUnaccent* const self = wxConstCast(this, wxMBConvUnaccent);
            switch (len)
            {
                default:
                wxLogDebug(wxS("Unexpected NUL length %d"), len);
                self->m_minMBCharWidth = (size_t)-1;
                break;

                case 0:
                self->m_minMBCharWidth = (size_t)-1;
                break;

                case 1:
                case 2:
                case 4:
                self->m_minMBCharWidth = len;
                break;
            }
        }

        return m_minMBCharWidth;
    }

    virtual wxMBConv* Clone() const wxOVERRIDE { return new wxMBConvUnaccent(*this); }
};

const char wxMBConvUnaccent::REPL = ' ';

