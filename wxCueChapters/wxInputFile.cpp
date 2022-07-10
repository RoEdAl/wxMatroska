/*
 * wxInputFile.cpp
 */

#include "wxInputFile.h"

 // ===============================================================================

const char wxInputFile::SEPARATOR = ';';

// ===============================================================================

void wxInputFile::copy(const wxInputFile& inputFile)
{
    m_inputFile = inputFile.m_inputFile;
    m_dataFile = inputFile.m_dataFile;
}

wxInputFile::wxInputFile(const wxInputFile& inputFile)
{
    copy(inputFile);
}

wxInputFile& wxInputFile::operator =(const wxInputFile& inputFile)
{
    copy(inputFile);
    return *this;
}

wxInputFile::wxInputFile(const wxString& inputFile, bool dataFiles)
{
    if (dataFiles)
    {
        wxStringTokenizer tokenizer(inputFile, SEPARATOR);

        if (tokenizer.HasMoreTokens()) m_inputFile.Assign(tokenizer.GetNextToken());

        while (tokenizer.HasMoreTokens())
        {
            const wxFileName dataFile(tokenizer.GetNextToken());
            m_dataFile.Add(dataFile);
        }
    }
    else
    {
        m_inputFile = wxFileName::FileName(inputFile);
    }
}

wxInputFile::wxInputFile(void)
{
}

bool wxInputFile::IsOk() const
{
    return m_inputFile.IsOk();
}

const wxFileName& wxInputFile::GetInputFile() const
{
    return m_inputFile;
}

bool wxInputFile::HasDataFiles() const
{
    return !m_dataFile.IsEmpty();
}

const wxArrayFileName& wxInputFile::GetDataFiles() const
{
    return m_dataFile;
}

void wxInputFile::GetDataFiles(wxArrayDataFile& dataFile, wxDataFile::FileType fileType) const
{
    dataFile.Clear();
    for (size_t i = 0, cnt = m_dataFile.GetCount(); i < cnt; ++i)
    {
        wxDataFile df(m_dataFile[i].GetFullPath(), fileType);
        dataFile.Add(df);
    }
}

wxInputFile& wxInputFile::SetInputFile(const wxFileName& fileName)
{
    m_inputFile = fileName;
    return *this;
}

wxInputFile& wxInputFile::SetDataFiles(const wxArrayFileName& dataFile)
{
    m_dataFile.Clear();
    WX_APPEND_ARRAY(m_dataFile, dataFile);
    return *this;
}

wxInputFile& wxInputFile::SetDataFile(const wxFileName& dataFile)
{
    m_dataFile.Clear();
    m_dataFile.Add(dataFile);
    return *this;
}

wxString wxInputFile::ToString(bool longPath) const
{
    wxString s;

    if (m_inputFile.IsOk())
    {
        s.Append(longPath ? m_inputFile.GetFullPath() : m_inputFile.GetFullName());
        s.Append(SEPARATOR);
    }

    for (size_t i = 0, cnt = m_dataFile.GetCount(); i < cnt; ++i)
    {
        s.Append(longPath ? m_dataFile[i].GetFullPath() : m_dataFile[i].GetFullName());
        s.Append(SEPARATOR);
    }

    if (s.Length() > 0) s.RemoveLast();
    return s;
}

