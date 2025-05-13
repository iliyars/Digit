#include "GetTimeDate.h"

//==============================================================
void GetTimeDate (CString &Time, CString &Date)
  {
  CString Str;
  CTime Tm = CTime::GetCurrentTime();
  Str = Tm.Format("%c");
  int iPos = Str.Find(' ');
  Time = Str.Mid(iPos);
  Date = Str.Left(iPos);
  char C;
  C = Date[0];
  Date.SetAt(0, Date[3]);
  Date.SetAt(3, C);
  C = Date[1];
  Date.SetAt(1, Date[4]);
  Date.SetAt(4, C);
  Date.Replace('/', '.');
  }
//==============================================================
