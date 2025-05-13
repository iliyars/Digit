#include "..\..\StdAfx.h"
#include "D:\WinInterf0602\MGTools\Include\Utils\Iofile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CString GetStrFragment(CString* pStr, int nTab, TCHAR cTab, TCHAR eStr)
{
   int i,j;
   CString s;
   i = j = 0;
	for(int iTab=0; iTab < nTab; iTab++) {
      i=j;
      while (i < pStr->GetLength() &&  pStr->GetAt(i) == cTab && pStr->GetAt(i) != eStr) i++;
      j=i;
      while (j < pStr->GetLength() && pStr->GetAt(j) != cTab && pStr->GetAt(j) != eStr) j++;
   }
   s = pStr->Mid(i,j-i);
   return s;
}
//================================= Input - Output ===================================
CMIOFile::Mode;

CMIOFile::CMIOFile(LPCTSTR FileName, int nMode)
{
  switch(nMode) {
    case READ:
      ifs = new std::fstream(FileName, std::ios::in /*|std::ios::nocreate*/);
      fb = ifs->rdbuf();
	  break;
    case WRITE:
      ifs = new std::fstream(FileName, std::ios::out);
      fb = ifs->rdbuf();
	  break;
    case READ_WRITE:
  	  ifs = new std::fstream(FileName, std::ios::out| std::ios::in| std::ios::ate /*|std::ios::nocreate*/);
      fb = ifs->rdbuf();
  }
  buf = new char[NumMaxLine+1];
  itbreak =";";
}

CMIOFile::~CMIOFile()
{
  if(buf) delete buf;
  if(IsFileOpen()) CloseFile();
  delete ifs;
}

BOOL CMIOFile::IsFileOpen()
{
	if(fb->is_open()) return TRUE;
	else return FALSE;
}

void CMIOFile::SetItemBreak(CString _itbreak)
{
 itbreak = _itbreak;
}

/*
void CMIOFile::SetUpOnBegR()
{
	if(ifs->eof()){
  	  ifs->clear(0);
    }
	ifs->seekg(0, std::ios::beg);

}

void CMIOFile::SetUpOnBegW()
{
	if(ifs->eof()){
		ifs->clear(0);
    }
	ifs->seekp(0, std::ios::beg);
}
*/
CString CMIOFile::GetStrDownTo(CString str)
{
    CString WStr="";
	int sl = -1;
	while(sl == -1 && WStr != GetItemBreak()){
      WStr = WStr + GetNextStr();
	  sl = WStr.Find(str);
	  if(sl != -1) WStr = WStr.Mid(0,sl-1);
	}
	return WStr;
}
/*
CString CMIOFile::GetNextStr(int MaxLine)
{
  int sl;
  CString str;
  if(MaxLine > NumMaxLine) {
     if(buf) delete buf; buf=0;
     buf = new char[MaxLine+1];
  }
  str = "EOF";
  if(!ifs->eof()) {
     ifs->getline(buf, MaxLine);
     str = CString(buf);
     sl = str.Find(itbreak);
     if(sl != -1) str = str.Mid(0,sl-1);
  }
  return str;
}
*/
CString CMIOFile::SetUpOnStrWith(CString* FStr, int MaxLine, BOOL SetOnBeg)
{
  CString s;
  int nPos = -1;
  if (SetOnBeg) 
    SetUpOnBegR();
  do {
	 s = GetNextStr(MaxLine);
     nPos = s.Find(*FStr);
	 if(nPos != -1) {
		 return s;
	 }
  } while (s != "EOF");
  s= "EOF";
  return s;
}


CString CMIOFile::GetStrWithSubStr(CString* SStr, int MaxLine,BOOL CutPat, BOOL SetOnBeg)
{
  CString s;
     s = SetUpOnStrWith(SStr, MaxLine, SetOnBeg);
	 if(CutPat && s != "EOF") {
        s =s.Mid(SStr->GetLength());
	 }
  return s;
}

CString CMIOFile::GetStrInSecWith(CString* NameSec, CString* FStr, int MaxLine,
                                  BOOL CutPat, BOOL SetOnBeg)
{
  CString s;
  int nPos = -1;
  int nBreak;
  int nStop, lStop, rStop;
  if(SetOnBeg) {
   s = SetUpOnStrWith(NameSec, MaxLine);
   if(s == "EOF") return s;
  }
  do {
      s = GetNextStr(MaxLine);
      lStop = s.Find("[");
      rStop = s.Find("]");
	  nStop = rStop - lStop;
      nPos = s.Find(*FStr);
  } while (s != "EOF" && nPos == -1 && nStop == 0);
  if(CutPat) {
     nBreak = s.Find("[");
     nPos = FStr->GetLength();
     s = s.Mid(nPos);
  }
  return s;
}

CString CMIOFile::GetSectionNamed(CString* SStr, int MaxLine, BOOL CutPat)
{
  CString s;
  CString ts="";
  s = SetUpOnStrWith(SStr);
  if(s != "EOF"){
    s = GetStrWithSubStr(SStr, MaxLine, CutPat);
    if (/*s[0] != '\x0' &&*/ s != "")
      ts += s;
	  do {
        s = GetNextStr(MaxLine);
        if(s != itbreak && s != "")ts = ts + s + " ";
		else break;
      } while (s != "EOF");
  }
  return ts;
}

void CMIOFile::WriteStr(CString* str)
{
   int nS = str->GetLength();
    ifs->write(str->GetBuffer(nS), nS);
    str->ReleaseBuffer();
}

void CMIOFile::SetStrInSecWith(CString* NameSec, CString* FStr, LPCTSTR Content,
                               int MaxLine, BOOL CutPat, BOOL SetOnBeg)
{
 CString Str, BufStr;
 int iPos, bPos, ePos; 
 Str = GetStrInSecWith(NameSec, FStr, MaxLine, CutPat, SetOnBeg);
 iPos = Str.GetLength()+2;
 SeekFromCurLocR(-iPos);
 bPos = GetCurLocR();
 SetUpOnBegW();
 while (bPos != GetCurLocR())BufStr += (char)fb->sbumpc();
 for(int i=0; i < iPos-1; i++)fb->stossc();
 BufStr += *FStr + CString(Content);
 ePos = fb->sbumpc();
 while (EOF != ePos){BufStr += (char)ePos; ePos = fb->sbumpc();}
 SetUpOnBegW();
 WriteStr(&BufStr);
}

CString* CMIOFile::CutStrToFinger(CString* SStr)
{
   char sumb;
   int iPos=0;          
   for(int i=0; i < SStr->GetLength(); i++) {
      sumb = SStr->GetAt(i);
		if(sumb == '.' ||sumb == '0' ||sumb == '1' ||sumb == '2' ||sumb == '3' ||
			sumb == '4' ||sumb == '5' ||sumb == '6' ||sumb == '7' ||sumb == '8' ||
			sumb == '9') {
				iPos = i;
				break;
      }
   }
   return &SStr->Mid(iPos);
}
