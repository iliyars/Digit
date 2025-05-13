#ifndef _READWRITEDATA_H_
#define _READWRITEDATA_H_

#include "NunberInfo.h"
#include "CTIOFile.h"

BOOL ReadZAPData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo); 
BOOL ReadWinZAPData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo); 
BOOL ReadDosZAPData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo); 
BOOL ReadFRNData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo); 
void WriteWinZAPData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo);
void WriteDosZAPData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo);
void WriteFRNData(const CString &FileName, NUMBERING_INTERFEROGRAM_INFO &IntInfo);
void WritePolygon(CTextIOFile &Fl, XYPolygon &Plg, int NPntInStr = 4); 
void WriteSection(CTextIOFile &Fl, SAMPLE_DATA &Sampl, int NPntInStr = 6); 
void WriteFringe(CTextIOFile &Fl, SAMPLE_DATA &Sampl); 
#endif
