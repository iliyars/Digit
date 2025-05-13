//
// Stingray Software Extension Classes
// Copyright (C) 1995 Stingray Software Inc.
// All Rights Reserved
//
// This source code is only intended as a supplement to the
// Stingray Extension Class product.
// See the SEC help files for detailed information
// regarding using SEC classes.
//
//  Author:             SPL
//  Description:        Implementation of SECFileSystem
//  Created:            Oct 30, 1995
//

//***************************************************************************
//  Include Files
//***************************************************************************

#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL


#ifndef __SECFILESYSTEM_H__
#include "..\..\Include\Utils\filesys.h"
#endif

#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


//***************************************************************************
//
// Name:    SECFileSystem
//
// Purpose: Constructor.
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Constructs a SECFileSystem object.
//@xref <c SECFileSystem>
SECFileSystem::SECFileSystem()
{
  // Initialize instance variables
  m_nMaxFileNameLength = 1024;
} // SECFileSystem


//***************************************************************************
//
// Name:    ~SECFileSystem
//
// Purpose: Destructor.
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc SECFileSystem destructor.
//@xref <c SECFileSystem>
SECFileSystem::~SECFileSystem()
{
  // No code.
} // ~SECFileSystem


//***************************************************************************
//
// Name:    GetFileStatus
//
// Purpose: To return the status information about the specified file.
//          See the MFC help on the CFileStatus struct for more information.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CFileStatus Status;
//          BOOL bRetVal = fs.GetFileStatus("c:\\test.txt", Status);
//
// Notes:    None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the file status.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | The name of the file.
//@parm  CFileStatus& | FileStatus | The status information is returned in 
// this struct.
//@comm To return the status information about the specified file.  See the 
// MFC help on the CFileStatus struct for more information.
//@xref <c SECFileSystem>  <mf SECFileSystem::GetFileAccessTime> 
// <mf SECFileSystem::GetFileModifyTime> <mf SECFileSystem::GetFileCreateTime>
// <mf SECFileSystem::GetFileSize>
//@ex |
//CFileStatus Status;
//BOOL bRetVal = fs.GetFileStatus("c:\\test.txt", Status);
BOOL SECFileSystem::GetFileStatus(const CString& FileName, CFileStatus& FileStatus)
{
  return CFile::GetStatus(FileName, FileStatus);
} // GetFileStatus


//***************************************************************************
//
// Name:    GetFileCreateTime
//
// Purpose: To return the creation time of the specified file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CTime CreateTime;
//          BOOL bRetVal = fs.GetFileCreateTime("c:\\test.txt", CreateTime);
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the time and date the file was created.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | The name of the file.
//@parm  CTime& | time | The time that the file was created.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileStatus>
// <mf SECFileSystem::GetFileModifyTime> <mf SECFileSystem::GetFileAccessTime>
// <mf SECFileSystem::GetFileSize>
//@ex |
//CTime CreateTime;
//BOOL bRetVal = fs.GetFileCreateTime("c:\\test.txt", CreateTime);
BOOL SECFileSystem::GetFileCreateTime(const CString& FileName, CTime& time)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    time = FileStatus.m_ctime;
    return TRUE;
  } // if

  return FALSE;
} // GetFileCreateTime


//***************************************************************************
//
// Name:    GetFileModifyTime
//
// Purpose:  To return the last modification time of the specified file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CTime ModifyTime;
//          BOOL bRetVal = fs.GetFileModifyTime("c:\\test.txt", ModifyTime);
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the time and date the file was last modified.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | The name of the file.
//@parm  CTime& | time | The time that the file was modified.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileStatus>
// <mf SECFileSystem::GetFileCreateTime> <mf SECFileSystem::GetFileAccessTime>
// <mf SECFileSystem::GetFileSize>
//@ex |
//CTime ModifyTime;
//BOOL bRetVal = fs.GetFileModifyTime("c:\\test.txt", ModifyTime);
BOOL SECFileSystem::GetFileModifyTime(const CString& FileName, CTime& time)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    time = FileStatus.m_mtime;
    return TRUE;
  } // if

  return FALSE;
} // GetFileModifyTime


//***************************************************************************
//
// Name:    GetFileAccessTime
//
// Purpose: To return the time of last access for the specified file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          CTime AccessTime;
//          BOOL bRetVal = fs.GetFileAccessTime("c:\\test.txt", AccessTime);
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the time and date the file was last accessed.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | The name of the file.
//@parm  CTime& | time | The time that the file was last access.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileStatus>
// <mf SECFileSystem::GetFileCreateTime> <mf SECFileSystem::GetFileModifyTime>
// <mf SECFileSystem::GetFileSize>
//@ex  | 
//CTime AccessTime;
//BOOL bRetVal = fs.GetFileAccessTime("c:\\test.txt", AccessTime);
BOOL SECFileSystem::GetFileAccessTime(const CString& FileName, CTime& time)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    time = FileStatus.m_atime;
    return TRUE;
  } // if

  return FALSE;
} // GetFileAccessTime


//***************************************************************************
//
// Name:    GetFileSize
//
// Purpose: To return the size of the specified file in bytes.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          unsigned long lSize = 0;
//          BOOL bRetVal = fs.GetFileSize("c:\\test.txt", lSize);
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the size of the file in bytes.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | The name of the file
//@parm  unsigned long& | lSize | If no error, the size of the file in bytes.
//@xref <c SECFileSystem> <mf SECFileSystem::GetDirectorySize>
//@xref <mf SECFileSystem::GetFileStatus>
//@comm To return the size (in bytes) of the specified file.
//@ex |
//unsigned long lSize;
//BOOL bRetVal = fs.GetFileSize("c:\\foo\\bar\\what.txt", lSize);
BOOL SECFileSystem::GetFileSize(const CString& FileName, unsigned long& lSize)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    lSize = FileStatus.m_size;
    return TRUE;
  } // if

  return FALSE;
} // GetFileSize


//***************************************************************************
//
// Name:    GetFileAttribute
//
// Purpose: To return the attribute bits associated with the file.
//
// Ret Val: TRUE: If no error.
//          FALSE: If error (ie. file does not exist).
//
// Example:
//          BOOL bRetVal = fs.GetFileAttribute("c:\\test.txt", bAttr);
//
// Notes:   See the Attribute enum in filesys.h for specific attributes.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the attribute bits for the file. 
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | The name of the file.
//@parm  BYTE& | nAttribute | If successful, the attribute byte specifying 
// file info. (See <md SECFileSystem::Attribute>).
//@comm To return the attribute byte for the specified file.  See the 
// <md SECFileSystem::Attribute> enum in filesys.h for specific attributes.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileStatus>
// <mf SECFileSystem::GetFileModifyTime> <mf SECFileSystem::GetFileAccessTime>
// <mf SECFileSystem::GetFileSize> <md SECFileSystem::Attribute>
//@ex |
//BOOL bRetVal = fs.GetFileAttribute("c:\\test.txt", bAttr);
BOOL SECFileSystem::GetFileAttribute(const CString& FileName, BYTE& nAttribute)
{
  CFileStatus FileStatus;
  if (CFile::GetStatus(FileName, FileStatus))
  {
    nAttribute = FileStatus.m_attribute;
    return TRUE;
  } // if

  return FALSE;
} // GetFileAttribute


#ifdef WIN32
//***************************************************************************
//
// Name:    GetADirectoryEntry
//
// Purpose: Function used in Win32 to get get directory entries with the
//          correct attributes set.
//
// Ret Val: TRUE: If no error.
//          FALSE: Error occurred.
//
// Example:
//
// Notes:   None
//
//***************************************************************************
BOOL SECFileSystem::GetADirectoryEntry(CString &fileName, const CString& Wildcard /* = "" */, const unsigned long eFileAttrib /* = normal */)
{
  BOOL      bRetVal = TRUE;
  DWORD		tmp;
  unsigned long mask = eFileAttrib;

  // If they passed in a Wildcard, we want to lookup the first entry.
  if (Wildcard != "")
  {
    m_hFind = FindFirstFile((const TCHAR *)Wildcard, &m_FileInfo);
    if (m_hFind == INVALID_HANDLE_VALUE)
    {
      bRetVal = FALSE;
    } // if
  } // if
  else
  {
    bRetVal = FindNextFile(m_hFind, &m_FileInfo);
  } // else


  if (bRetVal == FALSE)
  {
    // Error occurred.
    fileName = "";
    return FALSE;
  } // if

  tmp = m_FileInfo.dwFileAttributes;
  if (mask==_A_NORMAL || mask==FILE_ATTRIBUTE_NORMAL)
  {  
    if (!(tmp & 0x5e)) tmp = 0x80, mask = 0x80;
  }
  if (mask==allfiles || (tmp & mask))
  {
	fileName=m_FileInfo.cFileName;
  } // if
  else
  {
    fileName = "";
  } // else

  return TRUE;
} // GetADirectoryEntry
#endif


//***************************************************************************
//
// Name:     GetDirectoryEntry
//
// Purpose:  To return the next directory entry based on a wildcard, etc...
//
// Example:  CString *pFileName = fs.GetDirectoryEntry("*.txt", hidden);
//
// Notes:    None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns a single directory entry.
//@rdesc The name of a file in the current directory or a directory specified 
// in Wildcard.
//@parmopt const CString& | Wildcard | "" | The wildcard to use.
//@parmopt  const unsigned long | eFileAttrib | normal | The file attributes
// filter. (See <md SECFileSystem::Attribute>).
//@comm To return the next directory entry based on a wildcard, etc.  
//@devnote The CString returned by GetDirectoryEntry() must be deleted by the user.
//@xref <c SECFileSystem> <mf SECFileSystem::GetDirectory> <md SECFileSystem::Attribute>
//@ex |
//// Get the first filename matching *.txt
//CString *pFileName = fs.GetDirectoryEntry("*.txt", CFileSystem::normal);
//// Get the second filename matching *.txt
//CString *pFileName = fs.GetDirectoryEntry();
//@end
CString * SECFileSystem::GetDirectoryEntry(const CString& Wildcard /* = "" */, const unsigned long eFileAttrib /* = normal */)
{
#ifdef WIN32

  // Turn off critical error handler.
  UINT nPrevErrorMode = ::SetErrorMode(SEM_FAILCRITICALERRORS);

  BOOL      bRetVal = TRUE;

  CString fileName;
  CString l_Wildcard = Wildcard;
  while (bRetVal  &&  fileName == "")
  {
    bRetVal = GetADirectoryEntry(fileName, l_Wildcard, eFileAttrib);
    l_Wildcard = "";
  } // while

  // Turn on critical error handler.
  ::SetErrorMode(nPrevErrorMode);

  if (bRetVal == FALSE  &&  fileName == "")
  {
    return NULL;
  } // if

  // Create a string and copy the name to it.
  CString *pString = new CString(fileName);
  return pString;

#else

	int nRetVal;

	// Turn off DOS critical error handler.
	::SetErrorMode(1);

	// If they passed in a Wildcard, we want to lookup the first entry.
	if (Wildcard != "")
	{
		nRetVal = _dos_findfirst((const char *) Wildcard, (int) eFileAttrib, &m_FileInfo);
	} // if
	else
	{
		nRetVal = _dos_findnext(&m_FileInfo);
	} // else

	if (nRetVal != 0)
	{
		// Turn on DOS critical error handler.
		::SetErrorMode(0);

		// Error occurred, return NULL.
		return NULL;
	} // if

	// Create a string and copy the name to it.
	CString *pString = new CString(m_FileInfo.name);
	pString->Mid(0,12);

	// Make the filename lower case.
	pString->MakeLower();

	// Turn on DOS critical error handler.
	::SetErrorMode(0);

	return pString;

#endif
} // GetDirectoryEntry


//***************************************************************************
//
// Name:     GetCurrentDirectory
//
// Purpose:  To get the current working directory.
//
// Example:  CString CurrentDir = fs.GetCurrentDirecory();
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the current directory name.
//@rdesc CString 
//@parmopt const CString& | FileSystem | "" | The name of the filesystem that 
// the current directory is retrieved for.  If no filesystem or an empty 
// string is given, the current filesystem is used.
//@comm To get the current working directory of the current filesystem 
// (i.e., drive).  Also, the current directory of another filesystem can be 
// retrieved.
//
//To get the current directory of a filesystem that is not the current filesystem, 
// the current filesystem is changed to the requested filesystem and then the 
// filesystem is changed back after the current working directory is retrieved.
//@xref <c SECFileSystem>  <mf SECFileSystem::GetCurrentFileSystem>
// <mf SECFileSystem::ChangeDirectory>
//@ex |
//CString CurrentDir = fs.GetCurrentDirecory();
//ASSERT(CurrentDir == "c:\\tmp");
//CString CurrentDir = fs.GetCurrentDirecory("a:\\");
//ASSERT(CurrentDir == "a:\\foo");
//@end
CString SECFileSystem::GetCurrentDirectory(const CString& FileSystem /* = "" */)
{
  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  CString CurrentFileSystem = GetCurrentFileSystem();
  if (FileSystem != "")
  {
    if (!ChangeFileSystem(FileSystem))
    {
      return "";
    } // if
  } // if

  CString String;

#ifdef WIN32

  DWORD dwRetVal = ::GetCurrentDirectory(m_nMaxFileNameLength, String.GetBufferSetLength(m_nMaxFileNameLength));
  String.ReleaseBuffer(-1);

  if (dwRetVal == 0)
  {
    String = "";
  } // if

#else
	char *pRetVal = _tgetcwd(String.GetBufferSetLength(m_nMaxFileNameLength), m_nMaxFileNameLength);
	String.ReleaseBuffer(-1);

	// Make the string lower case.
	String.MakeLower();

	// If an error occured, clean up and return NULL.
	if (pRetVal == 0)
	{
		String = "";
	} // if

#endif

  if (FileSystem != "")
  {
    VERIFY(ChangeFileSystem(CurrentFileSystem));
  } // if

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return String;
} // GetCurrentDirectory


//***************************************************************************
//
// Name:    ChangeDirectory
//
// Purpose: To change the current working directory.
//
// Example: BOOL bRetVal = fs.ChangeDirectory("c:\\foo\\bar");
//
// Notes:   None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Changes the current working directory.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | NewDirectory | Name of the directory to change to.
//@comm To change the current working directory.
//@xref <c SECFileSystem> <mf SECFileSystem::GetCurrentDirectory>
//@ex |
//BOOL bRetVal = fs.ChangeDirectory("c:\\foo\\bar");
BOOL SECFileSystem::ChangeDirectory(const CString& NewDirectory)
{
  CString DirName = NewDirectory;

 #ifdef WIN32

  return SetCurrentDirectory((const TCHAR *) DirName);

#else

	int nRetVal = _tchdir((const TCHAR *) DirName);
	if (nRetVal == -1)
	{
		return FALSE;
	} // if

	return TRUE;

#endif
} // ChangeDirectory


//***************************************************************************
//
// Name:    RenameDirectory
//
// Purpose: To rename a directory.
//
// Ret Val: TRUE : No error, or OldName == NewName.
//          FALSE : Error.
//
// Example: BOOL bRetVal = fs.RenameDirectory("c:\\foo", "c:\\bar");
//
// Notes:   Using WIN16, if the new directory name is longer than 8.3,
//          the parts that are too long will be truncated.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Renames a directory.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | OldName | The name of the directory to rename.
//@parm  const CString& | NewName | The new name of the directory.
//@comm To rename a directory.
//
//It is not an error to rename a directory to the same name.
//@xref <c SECFileSystem> <mf SECFileSystem::RenameFile>
//@ex |
// BOOL bRetVal = fs.RenameDirectory("c:\\foo", "c:\\bar");
BOOL SECFileSystem::RenameDirectory(const CString& OldName, const CString& NewName)
{
  if (OldName == NewName)
  {
    return TRUE;
  } // if

  if (!DirectoryExists(OldName)  ||  DirectoryExists(NewName))
  {
    return FALSE;
  } // if

  TRY
  {
    CFile::Rename((const TCHAR *) OldName, (const TCHAR *) NewName);
  } // TRY
  CATCH(CFileException, Exception)
  {
    return FALSE;
  } // CATCH
  END_CATCH

  return TRUE;
} // RenameDirectory


//***************************************************************************
//
// Name:    MakeDirectory
//
// Purpose: To make a directory.
//
// Example: BOOL bRetVal = fs.MakeDirectory("c:\\foo\\bar");
//
// Notes:   None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Creates a subdirectory.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | NewDirectory | The name of the new directory.
//@xref <c SECFileSystem> <mf SECFileSystem::MakePath>
// <mf SECFileSystem::DeleteDirectory>
//@ex | 
// BOOL bRetVal = fs.MakeDirectory("c:\\foo\\bar");
BOOL SECFileSystem::MakeDirectory(const CString& NewDirectory)
{
#ifdef WIN32

  SECURITY_ATTRIBUTES security_attrib;
  security_attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
  security_attrib.lpSecurityDescriptor = NULL;
  security_attrib.bInheritHandle = TRUE;

  BOOL bRetVal = CreateDirectory((const TCHAR *) NewDirectory, &security_attrib);
  return bRetVal;

#else

	int nRetVal = _tmkdir((const char *) NewDirectory);
	if (nRetVal == -1)
	{
		return FALSE;
	} // if

	return TRUE;

#endif
} // MakeDirectory


//***************************************************************************
//
// Name:    MakePath
//
// Purpose: To make all the directories in a given path.  If any of the
//          directories exist, the creation continues with lower level
//          directories.
//
// Ret Val: TRUE : No error.
//          FALSE : Unable to create path.
//                  Path already exists.
//                  No path given.
//
// Example: BOOL bRetVal = fs.MakePath("c:\\foo\\bar\\what");
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Makes a subdirectory (including all intervening dirs necessary).
//@rdesc Nonzero if successful; otherwise 0 (Unable to create path. Error 
// occurred, path already exists, or no path given).
//@parm const CString& | NewDirectory | Name of path to create.
//@comm To make all the directories in a given path.  If any of the 
// directories exist, the creation continues with lower level directories.
//@xref <c SECFileSystem> <mf SECFileSystem::MakeDirectory>
// <mf SECFileSystem::DeleteDirectory>
//@ex | BOOL bRetVal = fs.MakePath("c:\\foo\\bar\\what");
BOOL SECFileSystem::MakePath(const CString& NewDirectory)
{
  CString NewDir = NewDirectory;  // Copy string for manipulation
  CString  DirName;
  BOOL    bRetVal = TRUE;

  // Error if no directory specified.
  if (NewDir.GetLength() == 0)
  {
    return FALSE;
  } // if

  // Make sure the directory name ends in a slash
  if (NewDir[NewDir.GetLength() - 1] != '\\')
  {
    NewDir = NewDir + '\\';
  } // if

  // Create each directory in the path
  UINT  nIndex = 0;
  BOOL  bDone = FALSE;
  while (!bDone)
  {
    // Extract one directory
    nIndex = NewDir.Find('\\');
    if (nIndex != -1)
    {
      DirName = DirName + NewDir.Left(nIndex);
      NewDir = NewDir.Right(NewDir.GetLength() - nIndex - 1);

      // The first time through, we might have a drive name
      if (DirName.GetLength() >= 1  &&  DirName[DirName.GetLength() - 1] != ':')
      {
        bRetVal = MakeDirectory(DirName);
      } // if
      DirName = DirName + '\\';
    } // if
    else
    {
      // We're finished
      bDone = TRUE;
    } // else
  } // while

  // Return the last MakeDirectory() return value.
  return bRetVal;
} // MakePath


//***************************************************************************
//
// Name:    DeleteDirectory
//
// Purpose: To delete a directory.  Optionally, all lower level files
//          and directories can be deleted.
//
// Ret Val: TRUE : Successfully deleted directory.
//          FALSE : Failed to delete directory.
//
// Example: BOOL bRetVal = fs.DeleteDirectory("c:\\foo\\bar", TRUE);
//
// Notes:   If bDeleteFilesAndDirs == FALSE, directory must be empty or
//          an error will occur and FALSE will be returned.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Deletes a directory and all its files and subdirectories.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | Directory | Name of the directory to delete.
//@parmopt  const BOOL | bDeleteFilesAndDirs | FALSE | Whether to delete all 
//the files and directories in the specified directory before the specified 
// directory is deleted.
//@comm To delete a directory.  Optionally, all lower level files and 
// directories can be deleted.
//
//If bDeleteFilesAndDirs is FALSE, directory must be empty or an error 
// will occur and FALSE will be returned.
//@xref <c SECFileSystem> <mf SECFileSystem::MakeDirectory>
// <mf SECFileSystem::MakePath> <mf ECFileSystem::DeleteFiles>
//@end
BOOL SECFileSystem::DeleteDirectory(const CString& Directory, const BOOL bDeleteFilesAndDirs /* = FALSE */)
{
  if (bDeleteFilesAndDirs)
  {
    CStringList *pSubDirs = GetSubdirList(Directory);
    for (POSITION pos = pSubDirs->GetHeadPosition(); pos != NULL; )
    {
      CString DirName = pSubDirs->GetNext(pos);
      DeleteDirectory(DirName, bDeleteFilesAndDirs);
    } // for

    delete pSubDirs ;
    pSubDirs = NULL;

    DeleteFiles(AppendWildcard(Directory, "*.*"), (const unsigned long)allfiles);
  } // if

#ifdef WIN32

  BOOL bRetVal = RemoveDirectory((const TCHAR *) Directory);
  return bRetVal;

#else

	int nRetVal = _trmdir((const char *) Directory);
	if (nRetVal == -1)
	{
		return FALSE;
	} // if

	return TRUE;
#endif
} // DeleteDirectory


//***************************************************************************
//
// Name:    GetDirectorySize
//
// Purpose: To return the size (in bytes) of the files in the specified
//          directory.
//
// Example: LONG lSize = fs.GetDirectorySize("c:\\foo\\bar", "*.*", TRUE);
//
// Notes:   If an error occurs in reading the status of a file, that
//          file is skipped and not counted as part of the size.
//          There is currently not way to find out if an error of this
//          kind occured.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the number of bytes in all the files in a directory.
//@rdesc The sum of all the sizes of the files in the current directory.
//@parmopt const CString& | Directory | "" | Name of directroy to get the 
// size of.  If not specified, the current directory is assumed.
//@parmopt const CString& | WildCard | "*.*" | Wildcard to use when 
// looking for files.
//@parmopt const BOOL | bRecurseSubdirs | FALSE | Whether to traverse 
// subdirectories.
//@comm To return the size (in bytes) of the files in the specified directory.
//
//If an error occurs in reading the status of a file, that file is skipped and 
// not counted as part of the size.  There is currently no way to find out if 
// an error of this kind occurred.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileSize>
//@ex | LONG lSize = fs.GetDirectorySize("c:\\foo\\bar", "*.*", TRUE);
//@end
LONG SECFileSystem::GetDirectorySize(const CString& Directory /* = "" */, const CString& WildCard /* = "*.*" */, const BOOL bRecurseSubdirs /* = FALSE */)
{
  LONG lSize = 0;

  // Do all the subdirectories first...
  if (bRecurseSubdirs)
  {
    CStringList *pSubDirs = GetSubdirList(Directory);
    for (POSITION pos = pSubDirs->GetHeadPosition(); pos != NULL; )
    {
      CString DirName = pSubDirs->GetNext(pos);
      lSize += GetDirectorySize(DirName, WildCard, bRecurseSubdirs);
    } // for

    delete pSubDirs;
    pSubDirs = NULL;
  } // if

  // Find the sizes of all the files in the specified directory.
  CStringList *pFileList = GetFileList(AppendWildcard(Directory, WildCard), 
  									  (const unsigned long)allfiles);
  for (POSITION pos = pFileList->GetHeadPosition(); pos != NULL; )
  {
    CFileStatus status;
    CString FileName = pFileList->GetNext(pos);
    if (CFile::GetStatus(FileName, status))
    {
      lSize += status.m_size;
    } // if
  } // for

  delete pFileList;
  pFileList = NULL;
  
  return lSize;
} // GetDirectorySize


//***************************************************************************
//
// Name:    GetCurrentFileSystem
//
// Purpose: To return a string containing the current file system name.
//
// Example: CString FileSystem = fs.GetCurrentFileSystem();
//
// Notes:   None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the current filesystem.
//@rdesc The name of the current filesystem.
//@comm To return a string containing the current filesystem name.
//@xref <c SECFileSystem> <mf SECFileSystem::GetCurrentDirectory>
// <mf SECFileSystem::ChangeFileSystem>
//@ex | CString FileSystem = fs.GetCurrentFileSystem();
CString SECFileSystem::GetCurrentFileSystem()
{
  unsigned int nDrive = 0;
  char cDrive = 'a';

#ifdef WIN32
  nDrive = _getdrive();
#else
	_dos_getdrive(&nDrive);
#endif
  cDrive = (char) ('a' + nDrive - 1);
  CString String = cDrive;
  String += ":\\";

  return String;
} // GetCurrentFileSystem


//***************************************************************************
//
// Name:    ChangeFileSystem
//
// Purpose: To change the current file system.
//
// Example: BOOL bRetVal = fs.ChangeFileSystem('c');
//
// Notes:   Obsolete.  The version that takes a CString parameter
//          should be used instead.  This change was made in preparation
//          for the addition of Windows NT support.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Changes the current filesystem.
//@syntax ChangeFileSystem(const char cFileSystem);
//@syntax ChangeFileSystem(const CString& FileSystem)
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const char | cFileSystem | The file system drive letter.
//@parm const CString& | FileSystem | The file system name.
//@devnote The version that takes a char parameter is obsolete.  The version 
// that takes a CString parameter should be used instead.  This change was 
// made for Windows NT support.
//@xref <c SECFileSystem> <mf SECFileSystem::GetCurrentFileSystem>
//@ex | BOOL bRetVal = fs.ChangeFileSystem("c:\\");
BOOL SECFileSystem::ChangeFileSystem(const char cFileSystem)
{
  unsigned int nNewDrive;
  BOOL bRetVal = FALSE;

  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  nNewDrive = _totupper(cFileSystem) - 'A' + 1;

  if (nNewDrive >= 0  &&  nNewDrive <= 26)
  {
#ifdef WIN32

    if (_chdrive(nNewDrive) == 0)
    {
      bRetVal = TRUE;
    } // if

#else
    
		// Attempt change filesystems.
    unsigned int nNumDrives;		// ignored return value
		_dos_setdrive(nNewDrive, &nNumDrives);

		// Make sure the change actually happened (it's the only way).
		unsigned int nCurrentDrive;
		_dos_getdrive(&nCurrentDrive);
		if (nCurrentDrive == nNewDrive)
		{
			bRetVal = TRUE;
		} // if

#endif
  } // if

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return bRetVal;
} // ChangeFileSystem


//***************************************************************************
//
// Name:    ChangeFileSystem
//
// Purpose: To change the current file system.
//
// Example: BOOL bRetVal = fs.ChangeFileSystem("c:\\");
//
// Notes:   Expects a filesystem name of the form "X:\".
//
//***************************************************************************
BOOL SECFileSystem::ChangeFileSystem(const CString& FileSystem)
{
#ifdef WIN32

  return SetCurrentDirectory((const TCHAR *) FileSystem);

#else

	BOOL bRetVal = FALSE;

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = FileSystem[0];
		bRetVal = ChangeFileSystem(cFileSystem);
	} // if

	return bRetVal;

#endif
} // ChangeFileSystem


//***************************************************************************
//
// Name:    GetFileSystemList
//
// Purpose: To return a list of available file systems (ie. drives).
//
// Example: CStringList *pFSList = fs.GetFileSystemList();
//
// Notes:   The user must free the returned list.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns a list of available filesystems.
//@rdesc A list of filesystem names.  Items from this list can be directly 
// passed to <mf SECFileSystem::ChangeFileSystem>.
//@comm To return a list of available filesystems (i.e., drives).
//@devnote The user will have to delete the CStringList returned.
//@xref <c SECFileSystem> <mf SECFileSystem::ChangeFileSystem>
//@ex | CStringList *pFSList = fs.GetFileSystemList();
CStringList  * SECFileSystem::GetFileSystemList()
{
  CStringList *pStringList = new CStringList();

  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  for (int nDriveNum = 0; nDriveNum < 26; nDriveNum++)
  {

#ifdef WIN32
  
    CString DriveName = (char)('a' + nDriveNum);
    DriveName = DriveName + ":\\";
    if (GetDriveType((const TCHAR *)DriveName) > 1)
    {
      CString DriveName = (char)('a' + nDriveNum);
      DriveName = DriveName + ":\\";
      pStringList->AddTail((const TCHAR *)DriveName);
    } // if

#else

		if (GetDriveType(nDriveNum) != 0)
		{
			CString DriveName = (char)('a' + nDriveNum);
			DriveName = DriveName + ":\\";
			pStringList->AddTail((const char *)DriveName);
		} // if

#endif

  } // for

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return pStringList;
} // GetFileSystemList


//***************************************************************************
//
// Name:    GetVolumeLabel
//
// Purpose: To get the volume label for a filesystem.
//
// Example: CString VolumeLabel = fs.GetVolumneLabel("c:\\");
//
// Notes:   None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the volume label of the specified filesystem.
//@rdesc The volume label for the specified filesystem.  If no volume label 
//is available, a zero length string is returned.
//@parm const CString& | FileSystem | The name of the filesystem.
//@ex | CString VolumeLabel = fs.GetVolumneLabel("c:\\");
//@xref <c SECFileSystem>
CString SECFileSystem::GetVolumeLabel(const CString& FileSystem)
{
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

#ifdef WIN32

  CString  VolumeLabel;
  DWORD    dwVolumeSerialNumber;
  DWORD    dwMaximumComponentLength;
  DWORD    dwFileSystemFlags;
  TCHAR    FileSystemName[255];

  if (!GetVolumeInformation((const TCHAR *) FileSystem,
                        VolumeLabel.GetBufferSetLength(255), 255,
                        &dwVolumeSerialNumber,
                        &dwMaximumComponentLength,
                        &dwFileSystemFlags,
                        FileSystemName, 255  ))
  {
    VolumeLabel = _T("");
  } // if

#else

	CString VolumeLabel = "";
	CString *pVolLabel = GetDirectoryEntry(AppendWildcard(FileSystem, "*.*"), volume);
	if (pVolLabel != NULL)
	{
		VolumeLabel = *pVolLabel;

		delete pVolLabel;
		pVolLabel = NULL;
	} // if

#endif

  ::SetErrorMode(0);

  return VolumeLabel;
} // GetVolumeLabel


//***************************************************************************
//
// Name:    GetFileSystemType
//
// Purpose: To get the type (removable, network, etc...) of the specified
//          filesystem.
//
// Ret Val: 0 = Error.
//          DRIVE_REMOVABLE : Removable Disk (ie. floppy)
//          DRIVE_FIXED : Non-Removable Disk (ie. hard drive)
//          DRIVE_REMOTE : Network Disk (ie. NFS mounted, Netware volume)
//
// Example: LONG lType = fs.GetFileSystemType("c:\\");
//          ASSERT(lType == DRIVE_FIXED);
//
// Notes:   None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the type of the specified filesystem.
//@rdesc Returns one of the following flags:
//@flag DRIVE_UNDETERMINED	| Error = 0.
//@flag DRIVE_REMOVABLE	| Removable Disk (i.e., floppy).
//@flag DRIVE_FIXED | Non-Removable Disk (i.e., hard drive).
//@flag DRIVE_REMOTE | Network Disk (i.e., NFS mounted, 
// Netware volume).
//@parm const CString& | FileSystem | The name of the filesystem.
//@comm To get the type (removable, network, etc.) of the specified filesystem.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileSystemList>
//@ex | LONG lType = fs.GetFileSystemType("c:\\");
//ASSERT(lType == DRIVE_FIXED);
LONG SECFileSystem::GetFileSystemType(const CString& FileSystem)
{
  LONG lType = DRIVE_UNDETERMINED;

#ifdef WIN32

  lType = GetDriveType((const TCHAR *) FileSystem);

#else

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = (char) _totupper(FileSystem[0]);

		int nDriveNum = _totupper(cFileSystem) - 'A';
	
		if (nDriveNum >= 0  &&  nDriveNum <= 26)
		{
			::SetErrorMode(1);
			lType = GetDriveType(nDriveNum);
			::SetErrorMode(0);
		} // if
	} // if

#endif

  return lType;
} // GetFileSystemType


//***************************************************************************
//
// Name:  GetFreeSpace
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the number of available bytes on the specified filesystem.
//@rdesc The number of free bytes on the specified filesystem.
//@parm const CString& | FileSystem | The name of the filesystem.
//@comm  To get the number of free bytes on a filesystem.
//@xref <c SECFileSystem> <mf SECFileSystem::GetTotalSpace>
//@ex | LONG lSize = fs.GetFreeSpace("c:\\");
LONG SECFileSystem::GetFreeSpace(const CString& FileSystem)
{
#ifdef WIN32

  LONG  lRetVal;
  DWORD dwSectorsPerCluster;
  DWORD dwBytesPerSector;
  DWORD dwFreeClusters;
  DWORD dwClusters;

  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  if (GetDiskFreeSpace((const TCHAR *) FileSystem,
      &dwSectorsPerCluster,
      &dwBytesPerSector,
      &dwFreeClusters,
      &dwClusters))
  {
    lRetVal = dwSectorsPerCluster * dwBytesPerSector * dwFreeClusters;
  } // if
  else
  {
    lRetVal = -1;
  } // else

  ::SetErrorMode(0);

  return lRetVal;

#else

	unsigned long lFreeSpace = -1;
	struct _diskfree_t diskspace;

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = (char) _totupper(FileSystem[0]);

		int nDriveNum = _totupper(cFileSystem) - 'A';
	
		if (nDriveNum >= 0  &&  nDriveNum <= 26)
		{
			::SetErrorMode(1);
			if	(_dos_getdiskfree(	nDriveNum + 1,	&diskspace	) == 0)
			{
					lFreeSpace =(unsigned long) diskspace.avail_clusters *
											(unsigned long) diskspace.sectors_per_cluster	*
											(unsigned long) diskspace.bytes_per_sector;
			} // if
			::SetErrorMode(0);
		} // if
	} // if

	return lFreeSpace;

#endif
} // GetFreeSpace


//***************************************************************************
//
// Name:  GetTotalSpace
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the number of bytes (used and unused) on the specified filesystem.
//@rdesc The total number of bytes on the specified filesystem.
//@parm const CString& | FileSystem | The name of the filesystem.
//@comm To get the total number of bytes on a filesystem.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFreeSpace>
//@ex | LONG lSize = fs.GetTotalSpace("c:\\");
LONG SECFileSystem::GetTotalSpace(const CString& FileSystem)
{
#ifdef WIN32

  LONG  lRetVal;
  DWORD dwSectorsPerCluster;
  DWORD dwBytesPerSector;
  DWORD dwFreeClusters;
  DWORD dwClusters;

  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  if (GetDiskFreeSpace((const TCHAR *) FileSystem,
      &dwSectorsPerCluster,
      &dwBytesPerSector,
      &dwFreeClusters,
      &dwClusters))
  {
    lRetVal = dwSectorsPerCluster * dwBytesPerSector * dwClusters;
  } // if
  else
  {
    lRetVal = -1L;
  } // else

  ::SetErrorMode(0);

  return lRetVal;

#else

	unsigned long lTotalSpace = -1;
	struct _diskfree_t diskspace;

	if (FileSystem.GetLength() > 0)
	{
		char cFileSystem = (char) _totupper(FileSystem[0]);

		int nDriveNum = _totupper(cFileSystem) - 'A';
	
		if (nDriveNum >= 0  &&  nDriveNum <= 26)
		{
			::SetErrorMode(1);
			if	(_dos_getdiskfree(	nDriveNum + 1,	&diskspace	) == 0)
			{
					lTotalSpace =(unsigned long) diskspace.total_clusters *
											(unsigned long) diskspace.sectors_per_cluster	*
											(unsigned long) diskspace.bytes_per_sector;
			} // if
			::SetErrorMode(0);
		} // if
	} // if

	return lTotalSpace;

#endif
} // GetTotalSpace


//***************************************************************************
//
// Name:  IsReadOnlyFileSystem
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Determines if the specified filesystem is writeable.
//@rdesc Nonzero if the filesystem is read-only or an error occurred; 0 if 
// the filesystem is not read-only.
//@parm const CString& | FileSystem | The name of the filesystem to interrogate.
//@comm  To determine if a filesystem is read-only, an attempt to open a 
// new file is made.  If the file creation is successful, the file is deleted.
//@ex | BOOL bRetVal = IsReadOnlyFileSystem("c:\\");
//@xref <c SECFileSystem>
BOOL SECFileSystem::IsReadOnlyFileSystem(const CString& FileSystem)
{
  BOOL            bRetVal = TRUE;
  CFile           TestFile;
  CFileException  fileException;
  CString         TempFileName;

  // Turn off DOS critical error handler.
  ::SetErrorMode(SEM_FAILCRITICALERRORS);

  // Get the temp file name
  TempFileName = "xxxxtest.999";

  // Try to create the temporary file.
  if (TestFile.Open(AppendWildcard(FileSystem, TempFileName), CFile::modeCreate, &fileException))
  {
    TestFile.Close();
    if (DeleteFile(AppendWildcard(FileSystem, TempFileName)))
    {
      bRetVal = FALSE;
    } // else
  } // if

  // Turn on DOS critical error handler.
  ::SetErrorMode(0);

  return bRetVal;
} // IsReadOnlyFileSystem


//***************************************************************************
//
// Name:    RenameFile
//
// Purpose: To rename a file.
//
// Ret Val: TRUE : No error, or OldFileName == NewFileName.
//          FALSE : Error.
//
// Example: BOOL bRetVal = fs.RenameFile("c:\\foo.txt", "c:\\bar.doc");
//          BOOL bRetVal = fs.RenameFile("c:\\foo.txt", "c:\\what\\bar.doc");
//
// Notes:   A file can also be moved using this function by specifying a
//          different directory in the NewFileName.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Renames a file.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | OldFileName | The name of the file to rename.
//@parm  const CString& | NewFileName | The new name of the file.
//@comm To rename a file.  A file can also be moved from one directory to 
// another in the same filesystem by specifying a new directory (in addition 
// to the filename) in the second parameter.
//
//It is not an error to rename a file to the same name.
//@xref <c SECFileSystem> <mf SECFileSystem::RenameDirectory>
//@ex | BOOL bRetVal = fs.RenameFile("c:\\foo.txt", "c:\\bar.doc");
//ASSERT(fs.RenameFile("c:\\foo.txt", "c:\\foo.txt") == TRUE);
//ASSERT(fs.RenameFile("c:\\foo.txt", "c:\\bar\\foo.txt") == TRUE);
BOOL SECFileSystem::RenameFile(const CString& OldFileName, const CString& NewFileName)
{
  if (OldFileName == NewFileName)
  {
    return TRUE;
  } // if

  if (!FileExists(OldFileName)  ||  FileExists(NewFileName))
  {
    return FALSE;
  } // if

  TRY
  {
    CFile::Rename((const TCHAR *) OldFileName, (const TCHAR *) NewFileName);
  } // TRY
  CATCH(CFileException, Exception)
  {
    return FALSE;
  } // CATCH
  END_CATCH

  return TRUE;
} // RenameFile


//***************************************************************************
//
// Name:    DeleteFiles
//
// Purpose: To delete a set of files based on a wildcard file specification.
//
// Example: BOOL bRetVal = fs.DeleteFiles("c:\\foo\\bar\\*.txt");
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Deletes specified files (i.e., *.txt).
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileSpec | The file specifier.
//@parmopt  const unsigned long | eFileAttrib | normal | The file attributes
// filter (See <md SECFileSystem::Attribute>.
//@comm To delete a set of files based on a wildcard file specification.
//@xref <c SECFileSystem> <mf SECFileSystem::DeleteFile> <md SECFileSystem::Attribute>
//@ex | BOOL bRetVal = fs.DeleteFiles("c:\\foo\\bar\\*.txt");
//@end
BOOL SECFileSystem::DeleteFiles(const CString& FileSpec, const unsigned long eFileAttrib /* = normal */)
{
  BOOL bRetVal = TRUE;
  CStringList *pDir = GetDirectory(FileSpec, eFileAttrib);

  for (POSITION pos = pDir->GetHeadPosition(); pos != NULL; )
  {
    CString FileName = pDir->GetNext(pos);
    if (DeleteFile(FileName) == FALSE)
    {
      bRetVal = FALSE;
    } // if
  } // for

  // Clean up.
  delete pDir;
  pDir = NULL;

  return bRetVal;
} // DeleteFiles


//***************************************************************************
//
// Name:    DeleteFile
//
// Purpose: To delete a file.
//
// Example:
//          BOOL bRetVal = fs.DeleteFile("c:\foo.txt");
//
// Notes:   None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Deletes a file.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileName | Name of the file to delete.
//@xref <c SECFileSystem> <mf SECFileSystem::DeleteFiles>
//@ex | BOOL bRetVal = fs.DeleteFile("c:\\foo.txt");
BOOL SECFileSystem::DeleteFile(const CString& FileName)
{
  TRY
  {
    CFile::Remove((const TCHAR *) FileName);
  } // TRY
  CATCH(CFileException, Exception)
  {
    return FALSE;
  } // CATCH
  END_CATCH

  return TRUE;
} // DeleteFile


//***************************************************************************
//
// Name:    CloseFile
//
// Purpose: To close a file.
//
// Ret Val: TRUE : File was closed successfully.
//          FALSE : Error occured.
//
// Notes:   None
//
//***************************************************************************
BOOL SECFileSystem::CloseFile(CFile *pFile) const
{
  BOOL bRetVal = TRUE;

  TRY
  {
    pFile->Close();
  } // TRY
  CATCH(CFileException, e)
  {
    bRetVal = FALSE;
  } // CATCH
  END_CATCH

  return bRetVal;
} // CloseFile


//***************************************************************************
//
// Name:    CopyFile
//
// Purpose: To copy a file.
//
// Ret Val: TRUE : File was copied successfully.
//          FALSE : Error occured.
//
// Example: BOOL bRetVal = fs.CopyFile("foo.txt", "a:\\bar.txt", 20480);
//
// Notes:   If the destination file exists, an error occurs.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Copies the contents of one file to another.
//@rdesc Nonzero if successful; 0 if an error occurred.  
//@devnote If the destination file exists, an error occurs.
//@parm const CString& | SourceFileName | Name of the file to copy.
//@parm  const CString& | DestFileName | Name of the new file.
//@parmopt  const unsigned long | lBuffSize | 10240 | Size of buffer to use 
// when copying file data.
//@xref <c SECFileSystem> <mf SECFileSystem::CopyFiles>
//@ex | BOOL bRetVal = fs.CopyFile("foo.txt", "a:\\bar.txt", 20480);
//@end
BOOL SECFileSystem::CopyFile(const CString& SourceFileName, const CString& DestFileName, const unsigned long lBuffSize /* = 10240 */)
{
  BOOL    bRetVal  = TRUE;
  char *  pBuff    = new char[lBuffSize];
  CFile Source;
  CFile Dest;

  // Check for exitance of the destination file.
  CFileStatus destStatus;
  if ((CFile::GetStatus(DestFileName, destStatus)))
  {
    delete [] pBuff;
    pBuff = NULL;

    return FALSE;
  } // if

  // Open the files, creating the destination.
  if (Source.Open((const TCHAR *) SourceFileName, CFile::modeRead))
  {
    if (Dest.Open((const TCHAR *) DestFileName, CFile::modeCreate | CFile::modeWrite))
    {
      DWORD  dwLength = Source.GetLength();
    
      // Copy the data in the file.
      while (dwLength > 0)
      {
        UINT nRead = Source.Read(pBuff, (UINT) lBuffSize);
        if (nRead)
        {
          Dest.Write(pBuff, nRead);
        } // if
      
        dwLength -= nRead;
      } // while
    
      CloseFile(&Source);
      CloseFile(&Dest);
    } // if
    else
    {
      CloseFile(&Source);

      bRetVal = FALSE;
    } // else
  } // if
  else
  {
    bRetVal = FALSE;
  } // else

  delete [] pBuff;
  pBuff = NULL;

  return bRetVal;
} // CopyFile


//***************************************************************************
//
// Name:    CopyFiles
//
// Purpose: To copy a set of files based on a wildcard file specification.
//
// Example: BOOL bRetVal = fs.CopyFiles("c:\\foo\\bar\\*.txt", "c:\\foo2");
//
// Notes:   If an error occurs, the copy is aborted.  There is no way to
//          which if any of the files were copied.
//
//          Even if you are ignoring errors, the return value indicates
//          whether an error occured.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Copies more than one file to another subdirectory.
//@rdesc Nonzero if successful; 0 if an error occurred.
//@parm const CString& | FileSpec | Name of the files to copy (with wildcards).
//@parm  const CString& | DestPath | Name of the directory to copy the files to.
//@parmopt  const BOOL | bIgnoreErrors | FALSE | Ignore errors when copying files.
//@parmopt  const unsigned long | eFileAttrib | normal | The file attributes
// filter of files to copy (See <md SECFileSystem::Attribute>).
//@comm To copy a set of files based on a wildcard file specification.
//@devnote 	If an error occurs and bIgnoreErrors is FALSE, the copy is aborted.  
// There is no way to know if any of the files were copied.  Even if you are 
// ignoring errors (i.e., bIgnoreErrors = TRUE), the return value indicates 
// whether an error occurred.  
//
//If a destination file exists, an error occurs.
//@xref <c SECFileSystem> <mf SECFileSystem::CopyFile>
// <mf SECFileSystem::DeleteFiles> <md SECFileSystem::Attribute>
//@ex | BOOL bRetVal = fs.CopyFiles("c:\\foo\\bar\\*.txt", "c:\\foo2");
//@end
BOOL SECFileSystem::CopyFiles(const CString& FileSpec, const CString& DestPath, const BOOL bIgnoreErrors /* = FALSE */, const unsigned long eFileAttrib /* = normal */)
{
  BOOL bRetVal = TRUE;
  CStringList *pDir = GetDirectory(FileSpec, eFileAttrib);

  for (POSITION pos = pDir->GetHeadPosition(); pos != NULL; )
  {
    CString FileName = pDir->GetNext(pos);
    CString DestFileName = AppendWildcard(DestPath, GetFileName(FileName));

    if (CopyFile(FileName, DestFileName) == FALSE)
    {
      bRetVal = FALSE;

      if (bIgnoreErrors == FALSE)
      {
        break;
      } // if
    } // if
  } // for

  // Clean up.
  delete pDir;
  pDir = NULL;

  return bRetVal;
} // CopyFiles


//***************************************************************************
//
// Name:    CompareFiles
//
// Purpose: To compare the contents of two files to see if they are the
//          same.
//
// Ret Val: TRUE, if the files are the same.
//          FALSE, if the files are different, or an error occurs.
//
// Example: BOOL bRetVal = fs.CompareFiles("foo.txt", "bar.txt", 20480);
//
// Notes:   None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Compares the contents of one file with another.
//@rdesc Nonzero if the files are the same; 0 if the files are different, 
// or an error occurs.
//@parm const CString& | FileName1 | One filename.
//@parm  const CString& | FileName2 | Another filename.
//@parmopt  const unsigned long | lBuffSize | 10240 | Size of buffer to 
// use when reading file data.
//@comm To compare the contents of two files to see if they are the same.
//@xref <c SECFileSystem> <mf SECFileSystem::CompareDirectories>
//@ex | BOOL bRetVal = fs.CompareFiles("foo.txt", "bar.txt", 20480);
//@end
BOOL SECFileSystem::CompareFiles(const CString& FileName1, const CString& FileName2, const unsigned long lBuffSize /* = 10240 */)
{
  BOOL    bRetVal  = TRUE;
  char *  pBuff1  = new char[lBuffSize];
  char *  pBuff2  = new char[lBuffSize];
  CFile   File1;
  CFile   File2;

  // Make sure we allocated the buffers
  if (!pBuff1  ||  !pBuff2)
  {
    if (pBuff1)
    {
      delete [] pBuff1;
    } // if

    if (pBuff2)
    {
      delete [] pBuff2;
    } // if

    return FALSE;
  } // if

  // Open the files.
  if (File1.Open((const TCHAR *) FileName1, CFile::modeRead | CFile::shareDenyWrite))
  {
    if (File2.Open((const TCHAR *) FileName2, CFile::modeRead | CFile::shareDenyWrite))
    {
      DWORD  dwLength1 = File1.GetLength();
      DWORD  dwLength2 = File2.GetLength();
      if (dwLength1 != dwLength2)
      {
        bRetVal = FALSE;
      } // if

      // Read the data in the file.
      while (bRetVal == TRUE  &&  dwLength1 > 0)
      {
        UINT nRead1 = File1.Read(pBuff1, (UINT) lBuffSize);
        UINT nRead2 = File2.Read(pBuff2, (UINT) lBuffSize);

        if (nRead1 != nRead2)
        {
          bRetVal = FALSE;
          break;              // break out of the while loop
        } // if

        if (memcmp(pBuff1, pBuff2, nRead1) != 0)
        {
          bRetVal = FALSE;
        } // if
      
        dwLength1 -= nRead2;
      } // while

      CloseFile(&File1);
      CloseFile(&File2);
    } // if
    else
    {
      CloseFile(&File1);

      bRetVal = FALSE;
    } // else
  } // if
  else
  {
    bRetVal = FALSE;
  } // else

  delete [] pBuff1;
  delete [] pBuff2;

  return bRetVal;
} // CompareFiles


//***************************************************************************
//
// Name:    CompareDirectories
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Compares the file names, sizes, and contents of two directories.
//@rdesc Nonzero if the directories are the same; 0 if the directories are 
// different, or an error occurs.
//@parm const CString& | PathName1 | One directory.
//@parm  const CString& | PathName2 | Another directory.
//@parmopt  const BOOL | bRecurseSubdirs | FALSE | Compare files in 
// subdirectories also.
//@parmopt  const BOOL | bCompareFileContents | FALSE | Compare the contents 
// as well as the name.
//@parmopt  const unsigned long | eFileAttrib | normal | The file attributes
// of files to compare (See <md SECFileSystem::Attribute>).
//@comm To compare the contents of two directories to see if they are the same.  
// Normally, CompareDirectories just determines if all the files in one 
// directory have corresponding  files in another directory.  By using the 
// bCompareFileContents option, CompareDirectories can also make sure that 
// the files contain the same information.
//@xref <c SECFileSystem> <mf SECFileSystem::CompareFiles>
//@ex | BOOL bRetVal = fs.CompareDirectories("c:\\foo", "c:\\bar");
//bRetVal = fs.CompareDirectories("c:\\foo", "c:\\bar", TRUE, TRUE);
//bRetVal = fs.CompareDirectories("c:\\foo", "c:\\bar", FALSE, TRUE);
//@end
BOOL SECFileSystem::CompareDirectories(const CString& PathName1, const CString& PathName2, const BOOL bRecurseSubdirs /* = FALSE */, const BOOL bCompareFileContents /* = FALSE */, const unsigned long eFileAttrib /* = normal */)
{
  BOOL bRetVal = TRUE;


  // See if the two pathnames are the same.

  if ( PathName1 == PathName2 )
  {
    return TRUE;
  } // if


  // Get the first directory listing.

  CString CurrentDirectory = GetCurrentDirectory();
  if (ChangeDirectory(GetPath(PathName1)) == 0)
  {
    return FALSE;
  } // if
  CStringList *pDir1 = GetDirectory(GetFileName(PathName1), eFileAttrib, bRecurseSubdirs);
  if (ChangeDirectory(CurrentDirectory) == 0)
  {
      delete pDir1;
      pDir1 = NULL;
    return FALSE;
  } // if


  // Get the second directory listing.

  if (ChangeDirectory(GetPath(PathName2)) == 0)
  {
      delete pDir1;
      pDir1 = NULL;
    return FALSE;
  } // if
  CStringList *pDir2 = GetDirectory(GetFileName(PathName2), eFileAttrib, bRecurseSubdirs);
  if (ChangeDirectory(CurrentDirectory) == 0)
  {
      delete pDir1;
      pDir1 = NULL;
      
      delete pDir2;
      pDir2 = NULL;
     
    return FALSE;
  } // if


  // Compare the directory listings.

  if (pDir1->GetCount() != pDir2->GetCount())
  {
    bRetVal = FALSE;
  } // if
  else
  {
    Sort(pDir1);
    Sort(pDir2);

    POSITION pos2 = pDir2->GetHeadPosition();
    for (POSITION pos1 = pDir1->GetHeadPosition(); pos1 != NULL; )
    {
      CString String1 = pDir1->GetNext(pos1);
      CString String2 = pDir2->GetNext(pos2);
      TRACE(_T("SECFileSystem::CompareDirectories %s %s\n"), (const TCHAR *) String1, (const TCHAR *)String2);

      if (String1 != String2)
      {
        bRetVal = FALSE;
        break;
      } // if
      else
      {
        if (bCompareFileContents)
        {
          if (!CompareFiles(AppendWildcard(GetPath(PathName1), String1), AppendWildcard(GetPath(PathName2), String2)))
          {
            bRetVal = FALSE;
            break;
          } // if
        } // if
      } // else
    } // for
  } // else


  // Clean up.
  delete pDir1;
  pDir1 = NULL;

  delete pDir2;
  pDir2 = NULL;

  return bRetVal;
} // CompareDirectories


//***************************************************************************
//
// Name:    GetFullPathName
//
// Example:  fs.GetFullPathName("c:\\foo\\bar\\what.txt");
//           fs.GetFullPathName("c:\\foo\\..\\bar\\what.txt");
//           fs.GetFullPathName("\\bar\\..\\what.txt");
//           fs.GetFullPathName("..\\bar\\.\\what.txt");
//           fs.GetFullPathName("c:..\\foo\\bar\\what.txt");
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Expands the specified filespec into a full path.
//@rdesc A fully qualified pathname.  Note that the function does not attempt 
// to check if the specified name is a valid filename.
//@comm To get the fully qualified pathname from the specified path and filename.  
// Relative pathnames, missing filesystems, dot, and dot-dot syntaxes are handled.
//@parm const CString& | PathAndFileName | The name and path to a file.
//@ex | CString Path = fs. GetFullPathName("c:\\test\\.\\..\\foo\\bar\\what.txt");
//ASSERT(Path == "c:\\foo\\bar\\what.txt");
//Path = fs. GetFullPathName("\\test\\.\\..\\foo\\bar\\what.txt");
//ASSERT(Path == "c:\\foo\\bar\\what.txt");
//Path = fs. GetFullPathName("test\\.\\..\\foo\\bar\\what.txt");
//ASSERT(Path == "c:\\curdir\\foo\\bar\\what.txt");
//Path = fs. GetFullPathName("what.txt");
//ASSERT(Path == "c:\\curdir\\what.txt");
//@xref <c SECFileSystem>
CString SECFileSystem::GetFullPathName(const CString& PathAndFileName)
{
#ifdef WIN32

  LPTSTR pTemp;
  CString FullPathName;

  ::GetFullPathName((const TCHAR *)PathAndFileName,
                    m_nMaxFileNameLength,
                    FullPathName.GetBufferSetLength(m_nMaxFileNameLength),
                    &pTemp);

  FullPathName.ReleaseBuffer(-1);

  return FullPathName;

#else

	CString FullPathName;

	// Determine the drive.
	CString FileSystem = GetFileSystem(PathAndFileName);
	if (FileSystem == "")
	{
		FileSystem = GetCurrentFileSystem();
	} // if

	// Determine if the path is relative to the root of the filesystem
	// or relative to the current directory of the filesystem.
	int nIndex = PathAndFileName.Find(":\\");
	if (nIndex != -1)
	{
		FullPathName = FileSystem;
	} // if
	else if (PathAndFileName.GetLength()  &&  PathAndFileName[0] == '\\')
	{
		FullPathName = FileSystem;
	} // else if
	else
	{
		FullPathName = GetCurrentDirectory(FileSystem) + "\\";
	} // else

	CString PartialPath = GetPath(PathAndFileName + "\\", FALSE);
	if (PartialPath[0] == '\\')
	{
		PartialPath = PartialPath.Right(PartialPath.GetLength() - 1);
	} // if
	if (PartialPath.GetLength())
	{
		PartialPath = PartialPath.Left(PartialPath.GetLength() - 1);
	} // if

	FullPathName = FullPathName + PartialPath;


	// Change all the slash-dot-slash sequences to slashes.

	BOOL bDone = FALSE;
	while (!bDone)
	{
		int nIndex = FullPathName.Find("\\.\\");
		if (nIndex == -1)
		{
			bDone = TRUE;
		} // if
		else
		{
			FullPathName = FullPathName.Left(nIndex) + FullPathName.Right(FullPathName.GetLength() - nIndex - 2);
		} // else
	} // while


	// Handle all the slash-dot-dot-slash sequences.

	bDone = FALSE;
	while (!bDone)
	{
		int nIndex = FullPathName.Find("\\..\\");
		if (nIndex == -1)
		{
			bDone = TRUE;
		} // if
		else
		{
#if 0
			FullPathName = "";
#else
			PartialPath = FullPathName.Left(nIndex);
			int nIndex2 = PartialPath.ReverseFind('\\');
			if (nIndex2 == -1)
			{
				FullPathName = "";
			} // if
			else
			{
				FullPathName = PartialPath.Left(nIndex2) + FullPathName.Right(FullPathName.GetLength() - nIndex - 3);
			} // else
#endif
		} // else
	} // while

	return FullPathName;

#endif
} // GetFullPathName


//***************************************************************************
//
// Name:    GetFileName
//
// Purpose:  Extract a file name from a path.
//
// Example:  CString FileName = fs.GetFileName("c:\\foo\\bar\\what.txt");
//          ASSERT(FileName == "what.txt");
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the filename and extension of a filespec.
//@rdesc The name of the file after discarding any filesystem and path.
//@parm const CString& | PathAndFileName | Name and possibly path to the file.
//@comm Extracts a file name from a path.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileName>
// <mf SECFileSystem::GetPath> <mf SECFileSystem::GetFileSystem>
// <mf SECFileSystem::GetBaseFileName>
//@ex | CString Path = fs.GetExtension("c:\\foo\\bar\\what.txt");
//ASSERT(Path == "txt");
//CString Path = fs.GetExtension("c:\\foo\\bar\\what.txt.john");
//ASSERT(Path == "john");
CString SECFileSystem::GetFileName(const CString& PathAndFileName)
{
  CString FileName = PathAndFileName;  // Copy to make modifications.

  // Find the last "\" in the string and return everything after it.
  int nIndex = FileName.Find('\\');
  while(nIndex != -1)
  {
    FileName = FileName.Right(FileName.GetLength() - nIndex - 1);

    nIndex = FileName.Find('\\');
  } // while

  return FileName;
} // GetFileName


//***************************************************************************
//
// Name:    GetPath
//
// Purpose:  To return the directory from a path.
//
// Example:  CString Path = fs.GetPath("c:\\foo\\bar\\what.txt");
//          ASSERT(Path == "c:\\foo\\bar\\");
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the path of the specified filespec.
//@rdesc The path in the specified string.
//@parm const CString& | PathAndFileName | The string to parse the path from.
//@parmopt  const BOOL | bIncludeFileSystem | TRUE | Flag indicating whether 
// to include file system ("fs") prefix in path.  Default is TRUE.
//@comm To return the directory from a path and filename.
//@xref <c SECFileSystem> <mf SECFileSystem::GetExtension>
// <mf SECFileSystem::GetFileName> <mf SECFileSystem::GetFileSystem>
// <mf SECFileSystem::GetBaseFileName>
//@ex | CString Path = fs.GetPath("c:\\foo\\bar\\what.txt");
//ASSERT(Path == "c:\\foo\\bar\\");
//Path = fs.GetPath("c:\\foo\\bar\\what\\");
//ASSERT(Path == "c:\\foo\\bar\\what\\");
//Path = fs.GetPath("c:\\foo\\bar\\what\\", FALSE);
//ASSERT(Path == "\\foo\\bar\\what\\");
//@end
CString SECFileSystem::GetPath(const CString& PathAndFileName, const BOOL bIncludeFileSystem /* = TRUE */)
{
  CString FileName = PathAndFileName; // Copy to modify;
  CString  Path = "";

  // Find the last "\" in the string and return everything up to and including it.
  int nIndex = FileName.Find('\\');
  while(nIndex != -1)
  {
    Path = Path + FileName.Left(nIndex + 1);
    FileName = FileName.Right(FileName.GetLength() - nIndex - 1);

    nIndex = FileName.Find('\\');
  } // while


  if (!bIncludeFileSystem)
  {
    // Remove the filesystem name from the front of the path.
    int nIndex = Path.Find(':');
    if (nIndex != -1)
    {
      Path = Path.Right(Path.GetLength() - nIndex - 1);
    } // if
  } // if

  return Path;
} // GetPath


//***************************************************************************
//
// Name:    GetExtension
//
// Purpose:  To return the extension from a path.
//
// Example:  CString Path = fs.GetExtension("c:\\foo\\bar\\what.txt");
//          ASSERT(Path == "txt");
//          CString Path = fs.GetExtension("c:\\foo\\bar\\what.txt.john");
//          ASSERT(Path == "john");
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the extension of the filename.
//@rdesc The extension (if any) of the specified file.  If there are 
// multiple extensions on the specified filename, the last extension is 
//returned.
//@parm const CString& | PathAndFileName | Name and possibly path to the file.
//@comm To return the extension from a path.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileName>
// <mf SECFileSystem::GetPath> <mf SECFileSystem::GetFileSystem>
// <mf SECFileSystem::GetBaseFileName>
//@ex | CString Path = fs.GetExtension("c:\\foo\\bar\\what.txt");
//ASSERT(Path == "txt");
//CString Path = fs.GetExtension("c:\\foo\\bar\\what.txt.john");
//ASSERT(Path == "john");
CString SECFileSystem::GetExtension(const CString& PathAndFileName)
{
  // Find the last "." in the string and return everything after it.
  int nIndex = PathAndFileName.ReverseFind('.');
  CString strExt;

  if (nIndex > 0 && nIndex < (PathAndFileName.GetLength() - 1))
	  strExt = PathAndFileName.Right(PathAndFileName.GetLength() - nIndex - 1);
  else
	  strExt.Empty();

  return strExt;
} // GetExtension


//***************************************************************************
//
// Name:    GetFileSystem
//
// Purpose:  To return the filesystem name from the given path.
//
// Example:  CString Path = fs.GetFileSystem("c:\\foo\\bar\\what.txt");
//          ASSERT(Path == "c:\\");
//          CString Path = fs.GetFileSystem("c:");
//          ASSERT(Path == "c:");
//          CString Path = fs.GetFileSystem("\\foo\\bar\\what.txt");
//          ASSERT(Path == "");
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the filesystem of the specified filespec.
//@rdesc The filesystem part of the specified filename.
//@parm const CString& | PathAndFileName | The name (and possibly path) 
// to a file.
//@comm  To return the filesystem name from the given path.
//@xref <c SECFileSystem> <mf SECFileSystem::GetExtension>
// <mf SECFileSystem::GetPath> <mf SECFileSystem::GetFileName>
// <mf SECFileSystem::GetBaseFileName>
//@ex | CString Path = fs.GetFileSystem("c:\\foo\\bar\\what.txt");
//ASSERT(Path == "c:\\");
//CString Path = fs.GetFileSystem("c:");
//ASSERT(Path == "c:");
//CString Path = fs.GetFileSystem("\\foo\\bar\\what.txt");
//ASSERT(Path == "");
CString SECFileSystem::GetFileSystem(const CString& PathAndFileName)
{
  CString FileSystem;

  // Find the last "." in the string and return everything after it.
  int nIndex = PathAndFileName.Find(_T(":\\"));
  if (nIndex != -1)
  {
    FileSystem = PathAndFileName.Left(nIndex+2);
  } // if
  else
  {
    nIndex = PathAndFileName.Find(':');
    if (nIndex != -1)
    {
      FileSystem = PathAndFileName.Left(nIndex+1);
    } // if
  } // else

  return FileSystem;
} // GetFileSystem


//***************************************************************************
//
// Name:    GetBaseFileName
//
// Purpose:  To return the filename (without extension) given a path.
//
// Example:  CString Path = fs.GetBaseFileName("c:\\foo\\bar\\what.txt");
//          ASSERT(Path == "what");
//          CString Path = fs.GetBaseFileName("what.txt");
//          ASSERT(Path == "what");
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns the filename without path or extension.
//@rdesc he base part of the filename in the specified path.
//@parm const CString& | PathAndFileName  | A filename, possibly 
// containing a path.
//@comm To return the filename (without extension) given a path.
//@xref <c SECFileSystem> <mf SECFileSystem::GetExtension>
// <mf SECFileSystem::GetFileName> <mf SECFileSystem::GetFileSystem>
// <mf SECFileSystem::GetPath>
//@ex | CString Path = fs.GetBaseFileName("c:\\foo\\bar\\what.txt");
//ASSERT(Path == "what");
//CString Path = fs.GetBaseFileName("what.txt");
//ASSERT(Path == "what");
CString SECFileSystem::GetBaseFileName(const CString& PathAndFileName)
{
  CString FileName = GetFileName(PathAndFileName);
  CString Ext = GetExtension(FileName);

  if (FileName.Find('.') != -1)
	return FileName.Left(FileName.GetLength() - Ext.GetLength() - 1);
  else
	return FileName;
} // GetBaseFileName


//***************************************************************************
//
// Name:    FileExists
//
// Purpose:  To determine if a file exists.
//
// Example:
//          BOOL bExists = fs.FileExists("c:\\foo\\bar\\what.txt");
//
// Notes:    A directory is considered a file for purposes of existance.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Determines if a file exists.
//@rdesc Nonzero if file exists; 0 if file does not exist.
//@parm const CString& | PathAndFileName | The name of the file to test 
// the existence of.
//@comm To determine if a file exists.
//@devnote	A directory is considered a file for purposes of existence.
//@xref <c SECFileSystem>  <mf SECFileSystem::DirectoryExists>
//@ex | BOOL bExists = fs.FileExists("c:\\foo\\bar\\what.txt");
BOOL SECFileSystem::FileExists(const CString& PathAndFileName)
{
  CFileStatus FileStatus;

  if (CFile::GetStatus(PathAndFileName, FileStatus) == TRUE)
  {
    return TRUE;
  } // if
  else
  {
    return FALSE;
  } // if
} // FileExists


//***************************************************************************
//
// Name:    FileSystemExists
//
// Purpose:  To determine if a filesystem exists.
//
// Example:
//          BOOL bExists = fs.FileSystemExists("c:\\");
//
// Notes:    None.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns whether the specified file system exists.
//@rdesc Nonzero if file system exists; 0 if file system does not exist. 
//@parm const CString& | FileSystemName | The name of the file system.
//@xref <c SECFileSystem> <mf SECFileSystem::FileExists>
BOOL SECFileSystem::FileSystemExists(const CString& FileSystemName)
{
  CString TempFSName = FileSystemName;  // cast away the const.
  TempFSName.MakeLower();

  BOOL bRetVal = FALSE;
  CStringList *pFSList = GetFileSystemList();
  for (POSITION pos = pFSList->GetHeadPosition(); pos != NULL; )
  {
    CString FSName = pFSList->GetNext(pos);
    if (FSName == TempFSName)
    {
      bRetVal = TRUE;
      break;
    } // if
  } // for

  delete pFSList;
  pFSList = NULL;

  return bRetVal;
} // FileSystemExists


//***************************************************************************
//
// Name:    DirectoryExists
//
// Purpose: To determine if a directory exists.
//
// Example:
//          BOOL bExists = fs.DirectoryExists("c:\\temp");
//
// Notes:   None.
//
//***************************************************************************

//@doc SECFileSystem
//@mfunc Determines if a directory exists.
//@rdesc Nonzero if directory exists; 0 if directory does not exist.
//@parm const CString& | Path | The name of the directory to test 
// the existence of.
//@xref <c SECFileSystem> <mf SECFileSystem::FileExists>
// <mf SECFileSystem::FileSystemExists>
//@ex | BOOL bExists = fs.DirectoryExists("c:\\temp");
BOOL SECFileSystem::DirectoryExists(const CString& Path)
{
#ifdef WIN32
  DWORD dwRetVal = GetFileAttributes( Path );
  if ( dwRetVal == 0xFFFFFFFF )
    return FALSE;

  // SB:  check for directory attribute
  else if (dwRetVal & FILE_ATTRIBUTE_DIRECTORY)
    return TRUE;

  return FALSE;
#else
  CFileStatus FileStatus;

  if (CFile::GetStatus(Path, FileStatus) == TRUE  &&
      FileStatus.m_attribute & directory)
  {
    return TRUE;
  } // if
  else if (FileSystemExists(GetFullPathName(Path)))
  {
    return TRUE;
  } // else if

  return FALSE;
#endif
} // DirectoryExists


//***************************************************************************
//
// Name:    GetDirectory
//
// Purpose:  To return a list of files based on a search string, etc...
//
// Example:  CStringList *pDir = fs.GetDirectory("*.txt", normal, TRUE);
//          (void) fs.GetDirectory("*.doc", normal, TRUE, pDir);
//
// Notes:    The filenames include the path.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns a list of files based on a filespec.
//@rdesc A list of files.  	The filenames include the path.
//@parm const CString& | SearchString | The name of the files to be listed 
// (i.e., *.*, *.txt).
//@parm  const unsigned long | eFileAttrib | The file filter attributes 
// (See <md SECFileSystem::Attribute>).
//@parmopt  const BOOL | bRecurseSubDirs | FALSE | Whether to traverse 
// subdirectories when looking for files.
//@parmopt  CStringList * | pStringList | NULL | A list to place the filenames in.  
// If one is not specified, one is created.
//@comm To return a list of files based on a search string, etc.
//@devnote 	If you do not pass in a CStringList pointer in pStringList, you 
// must delete the returned list.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileList>
// <mf SECFileSystem::GetSubdirList>
//@ex | CStringList *pDir = fs.GetDirectory("*.txt", normal, TRUE);
//(void) fs.GetDirectory("*.doc", normal, TRUE, pDir);
//@end
CStringList  * SECFileSystem::GetDirectory(const CString& SearchString, const unsigned long eFileAttrib, const BOOL bRecurseSubDirs /* = FALSE */, CStringList *pStringList /* = NULL */)
{
  // If they don't pass in a list, create one.
  if (pStringList == NULL)
  {
    pStringList = new CStringList();
  } // if

  // Read the file list.
  CStringList *pFileList = GetFileList(SearchString, eFileAttrib);
  pStringList->AddTail(pFileList);
  delete pFileList;
  pFileList = NULL;

  if (bRecurseSubDirs)
  {
    CString        CurDir = GetPath(SearchString);
    CStringList *  pDirList = GetSubdirList(CurDir);

    // Go through the directories we just got and recurse through them too.
    for (POSITION pos=pDirList->GetHeadPosition(); pos != 0; )
    {
      CString String = pDirList->GetNext(pos);

      // Get file name part of search path
      CString  SearchSpec = GetFileName(SearchString);

      // Do the recursion.
      GetDirectory(AppendWildcard(String, SearchSpec), eFileAttrib, bRecurseSubDirs, pStringList);
    } // for

    delete pDirList;
    pDirList = NULL;
  } // if

  return pStringList;
} // GetDirectory


//***************************************************************************
//
// Name:    GetSubdirList
//
// Purpose:  To return a list of subdirectories of another directory.
//
// Example:
//          CStringList *pDirs = fs.GetSubdirList("c:\\foo", FALSE);
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns a list of directories in the specified directory.
//@rdesc A list of filenames.
//@parm const CString& | SearchDir | The directory to get the filenames from.
//@parmopt const BOOL | bPathInName | TRUE | Whether to prepend the path to 
// the filenames in the list.
//@comm To return a list of subdirectories of another directory.  
//@devnote You must delete the CStringList returned.
//@xref <c SECFileSystem> <mf SECFileSystem::GetFileList>
// <mf SECFileSystem::GetDirectory>
//@ex | CStringList *pDirs = fs.GetSubdirList("c:\\foo", FALSE);
//@end
CStringList * SECFileSystem::GetSubdirList(const CString& SearchDir, const BOOL bPathInName /* = TRUE */)
{
  CFileStatus FileStatus;

  // Read the directory list
  CStringList *  pDirList = new CStringList();

  CString SearchPath = AppendWildcard(SearchDir, "*.*");
  CString *  pString = GetDirectoryEntry(SearchPath, directory);
  while (pString != NULL)
  {
    CString  String;
    CString  FullPath;
    CString  CurDir = GetPath(SearchPath);
    _stprintf(FullPath.GetBufferSetLength(1024), _T("%s%s"), (const TCHAR *)CurDir, (const TCHAR *)(*pString));
    FullPath.ReleaseBuffer(-1);
    if (bPathInName)
    {
      _stprintf(String.GetBufferSetLength(1024), _T("%s%s"), (const TCHAR *)CurDir, (const TCHAR *)(*pString));
      String.ReleaseBuffer(-1);
    } // if
    else
    {
      _stprintf(String.GetBufferSetLength(1024), _T("%s"), (const TCHAR *)(*pString));
      String.ReleaseBuffer(-1);
    } // else

    // If it's not one of the special directories.
    if (*pString != "."  &&  *pString != "..")
    {
      // Get the file type and make sure it's a directory before we add it to the list.
      CFile::GetStatus((const TCHAR *) FullPath, FileStatus);
      if (FileStatus.m_attribute & directory)
      {
        pDirList->AddTail((const TCHAR *) String);
      } // if
    } // if

    // Delete the string we got back from GetDirectoryEntry

    delete pString;
    pString = NULL;
  
    pString = GetDirectoryEntry("", directory);
  } // while

#ifdef WIN32
  if (m_hFind != INVALID_HANDLE_VALUE)
  {
    VERIFY(FindClose(m_hFind));
    m_hFind = INVALID_HANDLE_VALUE;
  } // if
#endif

  return pDirList;
} // GetSubdirList


//***************************************************************************
//
// Name:    GetFileList
//
// Purpose:  Return a list of files given a search path and attribute.
//
// Notes:    This only searches the specified directory.
//          Use GetDirectory() to recurse subdirectories.
//          The filenames include the path.
//
// Example:
//          CStringList *pList = GetFileList("c:\\foo\\bar\\*.cpp", normal);
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Returns a list of files in the specified directory.
//@rdesc A list of files in the specified directory.
//@parm const CString& | SearchString | Path and wildcard to search for.
//@parm  const unsigned long | eFileAttrib | The file attributes to match 
// (See <md SECFileSystem::Attribute>>.
//@comm Return a list of files given a search path and attribute.
//
// This only searches the specified directory.  Use <mf SECFileSystem::GetDirectory>
// to recurse subdirectories.  The filenames include the path.
//@devnote You must delete the returned CStringList.
//@xref <c SECFileSystem> <mf SECFileSystem::GetSubdirList>
// <mf SECFileSystem::GetDirectory>
//@ex | CStringList *pList = GetFileList("c:\\foo\\bar\\*.cpp", normal);
CStringList * SECFileSystem::GetFileList(const CString& SearchString, const unsigned long eFileAttrib)
{
  CStringList *pDirList = new CStringList();

  // Read the file list
  CString    CurDir = GetPath(SearchString);
  CString *  pString = GetDirectoryEntry(SearchString, eFileAttrib);

  CString  String;
  while (pString != NULL)
  {
    if ( *pString != "."  &&  *pString != ".." )
    {
		String=CurDir+*pString;
		pDirList->AddTail(String);
    } // if

    // Delete the string we got back from GetDirectoryEntry
    delete pString;
    pString = NULL;

    pString = GetDirectoryEntry("", eFileAttrib);
  } // while


#ifdef WIN32

  if (m_hFind != INVALID_HANDLE_VALUE)
  {
    VERIFY(FindClose(m_hFind));
    m_hFind = INVALID_HANDLE_VALUE;
  } // if

#endif

  return pDirList;
} // GetFileList


//***************************************************************************
//
// Name:    Sort
//
// Purpose:  To sort the give string list.
//
// Exmaple:
//          fs.Sort(pDirList);
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Sort a CStringList.
//@rdesc void 
//@parm CStringList * | pStringList | A pointer to the CStringList to sort.
//@parm BOOL | bCase | Sort in a case sensitive manner.
//@comm To sort the given string list.
//@ex | CFileSystem::Sort(pDirList);
//@xref <c SECFileSystem>
void SECFileSystem::Sort(CStringList *pStringList,BOOL bCase)
{
  int nListSize = pStringList->GetCount();
  for (int i = 0; i < nListSize-1; i++)
  {
    CString String1 = pStringList->GetAt(pStringList->FindIndex(i));
    for (int j = i+1; j < nListSize; j++)
    {
      CString String2 = pStringList->GetAt(pStringList->FindIndex(j));
      
	  if( bCase )
	  {
		
		  if (String1 > String2)
		  {
			  // Swap.
			  pStringList->SetAt(pStringList->FindIndex(i), String2);
			  pStringList->SetAt(pStringList->FindIndex(j), String1);
			  String1 = String2;
		  }
      }
	  else
	  {
		  if( String1.CompareNoCase(String2) == 1 )
		  {
  			  pStringList->SetAt(pStringList->FindIndex(i), String2);
			  pStringList->SetAt(pStringList->FindIndex(j), String1);
			  String1 = String2;
		  }
	  }//else
    } // for
  } // for
} // Sort


//***************************************************************************
//
// Name:    LoadListBox
//
// Purpose:  To load a list box with the given string list.
//
// Exmaple:
//          fs.LoadListBox(pListBox, pDirList);
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Insert the members of a CStringList into a list box.
//@rdesc void 
//@parm CListBox * | pListBox | The list box to fill.
//@parm  const CStringList * | pStringList | The list to fill the list box with.
//@comm To load a list box with the given string list.
//@xref <c SECFileSystem> <mf SECFileSystem::LoadComboBox>
void SECFileSystem::LoadListBox(CListBox *pListBox, const CStringList * pStringList)
{
  CString String;

  for (POSITION pos=pStringList->GetHeadPosition(); pos != 0; )
  {
    String = pStringList->GetNext(pos);
    pListBox->AddString(String);
  } // for
} // LoadListBox


//***************************************************************************
//
// Name:    LoadComboBox
//
// Purpose:  To load a combo box with the given string list.
//
// Example:
//          fs.LoadListBox(pComboBox1, pDirList);
//
// Notes:    None
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Insert the members of a CStringList into a combo box.
//@rdesc void 
//@parm CComboBox *| pComboBox | The combo box to fill.
//@parm  const CStringList * | pStringList | The list to fill the combo box with.
//@comm To load a combo box with the given string list.
//@xref <c SECFileSystem> <mf SECFileSystem::LoadListBox>
//@ex | CFileSystem::LoadComboBox(pComboBox1, pDirList);
void SECFileSystem::LoadComboBox(CComboBox *pComboBox, const CStringList * pStringList)
{
  CString String;

  for (POSITION pos=pStringList->GetHeadPosition(); pos != 0; )
  {
    String = pStringList->GetNext(pos);
    pComboBox->AddString(String);
  } // for
} // LoadComboBox


//***************************************************************************
//
// Name:    AppendWildcard
//
// Purpose:  To append a wildcard to a path.  It takes into account
//          whether the path has a backslash at the end.
//
// Example:
//          CString foo = fs.AppendWildcard("c:\\foo\\bar", "*.txt");
//          ASSERT(foo == "c:\\foo\\bar\\*.txt");
//          CString foo = fs.AppendWildcard("c:\\foo\\bar\\", "*.txt");
//          ASSERT(foo == "c:\\foo\\bar\\*.txt");
//
// Notes:    If the path is the empty string, the Wildcard is returned.
//
//***************************************************************************
//@doc SECFileSystem
//@mfunc Appends a wildcard to a path.
//@rdesc The new path with the appended wildcard.
//@parm const CString& | Path | Path to append wildcard to.
//@parm  const CString& | Wildcard | Wildcard to append to path.
//@comm To append a wildcard to a path.  It takes into account whether the 
// path has a backslash at the end.
CString SECFileSystem::AppendWildcard(const CString& Path, const CString& Wildcard)
{
  int nLength = Path.GetLength();
  CString RetVal;

  if (nLength == 0  ||  Path.GetAt(nLength - 1) == '\\')
  {
    RetVal = Path + Wildcard;
  } // if
  else
  {
    RetVal = Path + "\\" + Wildcard;
  } // else

  return RetVal;
} // AppendWildcard

