#include "GetTimeDate.h"

//==============================================================
void GetTimeDate (CString &Time, CString &Date)
  {
  CString Str;
  CTime tm = CTime::GetCurrentTime();
  Date = tm.Format(_T("%d.%m.%Y"));
  Time = tm.Format(_T("%H:%M:%S"));


  /*Str = Tm.Format("%c");
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
  Date.Replace('/', '.');*/
  }
//==============================================================
