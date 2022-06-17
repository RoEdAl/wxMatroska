/*
 * wxTemporaryFilesProvider.cpp
 */

#include "wxTemporaryFilesProvider.h"

 wxTemporaryFilesCleaner::wxTemporaryFilesCleaner()
     :m_deleteFiles(true)
 { }

 wxTemporaryFilesCleaner::wxTemporaryFilesCleaner(bool deleteFiles)
     :m_deleteFiles(deleteFiles)
 {
 }

 void wxTemporaryFilesCleaner::Feed(const wxTemporaryFilesProvider& filesProvider)
 {
     filesProvider.GetTemporaryFiles(m_temporaryFiles);
 }

 void wxTemporaryFilesCleaner::Add(const wxFileName& tmpFile)
 {
     m_temporaryFiles.Add(tmpFile);
 }

 wxTemporaryFilesCleaner::~wxTemporaryFilesCleaner()
 {
     if (m_deleteFiles)
     {
         DeleteFiles();
     }
     else
     {
         for (size_t i = 0, cnt = m_temporaryFiles.GetCount(); i < cnt; ++i)
         {
             wxLogInfo(_("Created file: %s"), m_temporaryFiles[i].GetFullPath());
         }
     }
 }

 void wxTemporaryFilesCleaner::DeleteFiles()
 {
     wxASSERT(m_deleteFiles);

     for (size_t i = 0, cnt = m_temporaryFiles.GetCount(); i < cnt; ++i)
     {
         const wxFileName& fn = m_temporaryFiles[i];
         const wxString fnFullPath = fn.GetFullPath();

         if (!wxFileExists(fnFullPath))
         {
             wxLogDebug("Nonexistent temporary file: %s", fnFullPath);
             continue;
         }

         if (!wxRemoveFile(fn.GetFullPath()))
         {
             wxLogWarning(_("Unable to delete temporary file: %s"), fnFullPath);
         }
     }

     m_temporaryFiles.Clear();
 }
