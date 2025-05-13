#ifndef _IO_FILE_H
#define _IO_FILE_H

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA
#include "..\..\StdAfx.h"

#ifndef __WINDOWS_H
  #include "windows.h"
#endif

#ifndef __FSTREAM_H
  #include "fstream"
#endif

#define NumMaxLine 100 // Max lenght of string to be read.

/*
   The function to inflate the initial array (Arr) of n elements
   by some ones (nInf).
   Returns the pointer to the inflated array.
*/

CString GetStrFragment(CString* pStr, int nTab, TCHAR cTab=' ', TCHAR eStr='\n');
/*
   The class to read a data file
*/
class CMIOFile
{
  private:
      LPCTSTR FileName;   // name of file to be read
	  std::fstream* ifs;    // pointer to open stream
	  std::filebuf* fb;      // pointer to buffer
      CString itbreak;   // some sumbols to set stop position
      char* buf;
  protected:
     int strCount;

  public:
     CMIOFile(LPCTSTR _FileName, int nMode);
     ~CMIOFile();
     static enum Mode{ READ, WRITE, READ_WRITE};

     BOOL IsFileOpen(); // Checks if file is open
     void CloseFile(){fb->close();} // Closes file

      long GetCurLocR() {return ifs->tellg();} // Gets the current position of cursor in file
      void SetUpOnBegR(BOOL DropCount=TRUE);
      void SetUpOnEndR() {ifs->seekg(0, std::ios::end);} // Sets cursor up on end of file
      void SeekFromBegR(int nStr){ifs->seekg(nStr, std::ios::beg);} // Shifts cursor by n string from begin of file
      void SeekFromCurLocR(int nStr){ifs->seekg(nStr, std::ios::cur);}// Shifts cursor by n string from current position
      void SeekFromEndR(int nStr){ifs->seekg(nStr, std::ios::end);}// Shifts cursor by n string from end

      long GetCurLocW() {return ifs->tellp();} // Gets the current position of cursor in file
      void SetUpOnBegW(BOOL DropCount=TRUE); // Sets cursor up on begin of file
      void SetUpOnEndW(){ifs->seekp(0, std::ios::end);} // Sets cursor up on end of file
      void SeekFromBegW(int nStr){ifs->seekp(nStr, std::ios::beg);} // Shifts cursor by n string from begin of file
      void SeekFromCurLocW(int nStr){ifs->seekp(nStr, std::ios::cur);}// Shifts cursor by n string from current position
      void SeekFromEndW(int nStr){ifs->seekp(nStr, std::ios::end);}// Shifts cursor by n string from end

      void SetItemBreak(CString _itbreak); // Sets some sumbols on which reading of string to be stoped
      CString GetItemBreak(){ return itbreak;}// Gets some sumbols on which reading of string to be stoped
      CString GetStrDownTo(CString str);//Reads string down from last sumbol of str
      CString SetUpOnStrWith(CString* FStr, int MaxLine=NumMaxLine, BOOL SetOnBeg=TRUE);
      //Sets cursor on string with sumbols contained in FStr
      CString GetStrWithSubStr(CString* SStr, int MaxLine=NumMaxLine, BOOL CutPat=TRUE, BOOL SetOnBeg=TRUE);
                               //Gets string containning SStr
							  //(if CutPat being TRUE the string to be cut from last sumbol of SStr)
      CString GetStrInSecWith(CString* NameSec, CString* FStr,
                             int MaxLine=NumMaxLine, BOOL CutPat=TRUE, BOOL SetOnBeg=TRUE);
      CString GetSectionNamed(CString* SStr, int MaxLine=NumMaxLine, BOOL CutPat=TRUE);
      // Gets section with name as SStr
      CString GetPrevStr(int MaxLine=NumMaxLine, BOOL CutBreak=TRUE); // Gets next string of file
      CString GetNextStr(int MaxLine=NumMaxLine, BOOL CutBreak=TRUE, BOOL Counting=TRUE); // Gets next string of file
      CString* CutStrToFinger(CString* SStr); // Cuts string up to first finger

      void SetStrInSecWith(CString* NameSec, CString* FStr, LPCTSTR Content,
                               int MaxLine, BOOL CutPat, BOOL SetOnBeg);
      void WriteStr(CString* str); // Writes string into file
};

#undef AFX_DATA
#define AFX_DATA
#endif
