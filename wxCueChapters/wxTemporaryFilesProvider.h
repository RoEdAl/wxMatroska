/*
* wxTemporaryFilesProvider.h
*/

#ifndef _WX_TEMPORARY_FILES_PROVIDER_H_
#define _WX_TEMPORARY_FILES_PROVIDER_H_

#include <wxCueFile/wxDataFile.h>

class wxTemporaryFilesProvider
{
    public:

    virtual void GetTemporaryFiles(wxArrayFileName&) const = 0;
};

class wxTemporaryFilesCleaner
{
    wxDECLARE_NO_COPY_CLASS(wxTemporaryFilesCleaner);

    public:

    wxTemporaryFilesCleaner();
    wxTemporaryFilesCleaner(bool);
    ~wxTemporaryFilesCleaner();

    void Feed(const wxTemporaryFilesProvider&);
    void Add(const wxFileName&);

    void DeleteFiles();

    private:

    bool m_deleteFiles;
    wxArrayFileName m_temporaryFiles;

};

#endif
