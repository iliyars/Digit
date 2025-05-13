#include <malloc.h>
#include "..\stdafx.h"
#include "..\AppDef.h"
#include "..\Utils\mutils.h"
#include "ImageCtrls.h"
#include "D:\Numbering\MGTools\Include\Utils\Utils.h"
#include "D:\Numbering\MGTools\Include\Image\SecPcx.h"
#include "D:\Numbering\MGTools\Include\Image\SecJpeg.h"
#include "D:\Numbering\MGTools\Include\Image\SecGif.h"
#include "D:\Numbering\MGTools\Include\Image\SecTarga.h"
#include "D:\Numbering\MGTools\Include\Image\SecTiff.h"
#include "D:\Numbering\MGTools\Include\Utils\Utils.h"
//====================================================================
CImageCtrls::CImageCtrls()
{
	Init();
}

void CImageCtrls::Init()
{
	for(int i=0; i < 256; i++){
	  m_rgbPalette[i].rgbBlue =m_rgbPalette[i].rgbGreen =m_rgbPalette[i].rgbRed = i;
	  m_rgbPalette[i].rgbReserved = 0;
	}
	m_pDIB = NULL;
	PrepareNewImage();
}

void CImageCtrls::PrepareNewImage()
{
    kZoom = 1.;
    kContrast = 1.;
    kBright = 1.;
    kZoomHorz = 1.;
    kZoomVert = 1.;
	ScrollPosPoint = CPoint(-1,-1);

    if(m_pDIB)
		delete m_pDIB;
    m_pDIB = NULL;
    ImageSize.cx = 0;
    ImageSize.cy = 0;
}

CImageCtrls::~CImageCtrls()
{
    if (m_pDIB != NULL)
      delete m_pDIB;
	m_pDIB = NULL;
}

CImageCtrls& CImageCtrls::operator=(const CImageCtrls& rhs)
{
  if(this==&rhs) return *this;
    kZoom = rhs.kZoom;
    kContrast = rhs.kContrast;
    kBright = rhs.kBright;
    kZoomHorz = rhs.kZoomHorz;
    kZoomVert = rhs.kZoomVert;
	ScrollPosPoint = rhs.ScrollPosPoint;
    m_pDIB = rhs.m_pDIB;
    ImageSize = rhs.ImageSize;
    for(int i=0; i < 256; i++){
       m_rgbPalette[i] = rhs.m_rgbPalette[i];
    }

    return *this;
}

void CImageCtrls::SetkZoomHorz(int Procent)
{
	double kZ = Procent*0.01;
	kZoomHorz = kZ;
}

void CImageCtrls::SetkZoomVert(int Procent)
{
	double kZ = Procent*0.01;
	kZoomVert = kZ;
}

CRect CImageCtrls::GetDIBRect()
{
	CRect rcDIB;
	if(ImageSize.cx == 0 && ImageSize.cy == 0)
	  rcDIB = CRect(0,0,ImageSize.cx ,ImageSize.cy);
	else
	  rcDIB = CRect(0,0,ImageSize.cx,ImageSize.cy);
    return rcDIB;
}

BOOL CImageCtrls::ConvertToDIB(CString& name)
{
  char path[_MAX_PATH];
  GetTempPath(_MAX_PATH, path);

  CString nameOTH = name;
  CString ext = name.Right(3);
  ext.MakeUpper();

  name = name.Left(name.GetLength()-3);
  int iPos = name.ReverseFind('\\');
  name = name.Mid(iPos+1);
  CString nameDIB = path + name + CString("bmp");

  SECDib* pDib = new SECDib;
  SECPcx* pPCX = NULL;
  SECJpeg* pJPG = NULL;
  SECGif* pGIF = NULL;
  SECTarga* pTGA = NULL;
  SECTiff* pTIF = NULL;
  BOOL res;
  if(ext == "BMP"){
     res = pDib->LoadImage(LPCTSTR(nameOTH));
	 if(!res){
		 delete pDib;
		 return FALSE;
	 }
  }
  else if(ext == "PCX"){
	  pPCX = new SECPcx;
	  res = pPCX->LoadImage(LPCTSTR(nameOTH));
	  if(!res || !pDib->ConvertImage(pPCX)){
		  if(pPCX) delete pPCX;
		  if(pDib) delete pDib;
		  return FALSE;
	  }
  }
  else if(ext == "JPG"){
	  pJPG = new SECJpeg;
	  res = pJPG->LoadImage(LPCTSTR(nameOTH));
	  if(!res || !pDib->ConvertImage(pJPG)){
		  if(pJPG) delete pJPG;
		  if(pDib) delete pDib;
		  return FALSE;
	  }
  }
   else if(ext == "GIF"){
	  pGIF = new SECGif;
	  res = pGIF->LoadImage(LPCTSTR(nameOTH));
	  if(!res ||!pDib->ConvertImage(pGIF)){
		  if(pGIF) delete pGIF;
		  if(pDib) delete pDib;
		  return FALSE;
	  }
   }
   else if(ext == "TGA"){
	  pTGA = new SECTarga;
	  res = pTGA->LoadImage(LPCTSTR(nameOTH));
	  if(!res || !pDib->ConvertImage(pTGA)){
		  if(pTGA) delete pTGA;
		  if(pDib) delete pDib;
		  return FALSE;
	  }
   }
   else if(ext == "TIF"){
	  pTIF = new SECTiff;
	  res = pTIF->LoadImage(LPCTSTR(nameOTH));
	  if(!res ||!pDib->ConvertImage(pTIF)){
		  if(pTIF) delete pTIF;
		  if(pDib) delete pDib;
		  return FALSE;
	  }
   }

  CFile fl;
  if(fl.Open(LPCTSTR(nameDIB), CFile::modeCreate|CFile::modeWrite)==NULL){
    if(pPCX) delete pPCX;
    if(pDib) delete pDib;
    return FALSE;
  }

  pDib->SaveImage(&fl);
  fl.Close();


   name = nameDIB;

   if(pPCX) delete pPCX;
   if(pDib) delete pDib;
   if(pJPG) delete pJPG;
   if(pGIF) delete pGIF;
   if(pTGA) delete pTGA;
   if(pTIF) delete pTIF;

//  pCtrls->needConvertImage = FALSE;
   return TRUE;
}

BOOL CImageCtrls::ConvertToGrayScale(CDC* pDC, LPCTSTR fname)
{
  int nBitCount = m_pDIB->m_nSrcBitsPerPixel;
  if(nBitCount == 8)
      return TRUE;

  BOOL bPrePadded = m_pDIB->m_bIsPadded;
      if(bPrePadded)
        m_pDIB->PubUnPadBits();

  int nPlanes = m_pDIB->m_nBitPlanes;
  DWORD Size = m_pDIB->m_dwWidth * m_pDIB->m_dwHeight * nBitCount/8;
  DWORD newSize = m_pDIB->m_dwWidth * m_pDIB->m_dwHeight;

  RGBQUAD rgbPix;
  LPBYTE lpPixel;
  LPBYTE lpDIBBits;
  lpDIBBits = (LPBYTE)malloc(newSize*sizeof(BYTE));

  LPBITMAPINFO lpbi;
    // Fill in the BITMAPINFOHEADER
    lpbi = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD))];
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbi->bmiHeader.biWidth = m_pDIB->m_dwWidth;
    lpbi->bmiHeader.biHeight = m_pDIB->m_dwHeight;
    lpbi->bmiHeader.biPlanes = 1;
    lpbi->bmiHeader.biBitCount = 8;
    lpbi->bmiHeader.biCompression = BI_RGB;
    lpbi->bmiHeader.biSizeImage = newSize;
    lpbi->bmiHeader.biXPelsPerMeter = 0;
    lpbi->bmiHeader.biYPelsPerMeter = 0;
    lpbi->bmiHeader.biClrUsed = 0;
    lpbi->bmiHeader.biClrImportant = 0;

    // Fill in the color table
    UINT uUsage = DIB_RGB_COLORS;
    memcpy( lpbi->bmiColors, m_rgbPalette, sizeof(RGBQUAD) * 256 );

    HBITMAP hBitmap = CreateDIBSection(pDC->m_hDC, lpbi, uUsage, (void **)lpDIBBits, NULL, 0 );
    CBitmap* pBitmap = CBitmap::FromHandle(hBitmap);

    SECDib* pDib = new SECDib;
    pDib->CreateFromBitmap(pDC, pBitmap);

  int n = -1;
  unsigned char Pixel=0;
  if(nBitCount == 16){
    for(DWORD idx=0; idx < Size; idx +=2){
      lpPixel = m_pDIB->m_lpSrcBits + idx;
      rgbPix.rgbBlue = (BYTE)*lpPixel;
      rgbPix.rgbGreen = (BYTE)*(lpPixel+1);
      Pixel = __max(rgbPix.rgbBlue, rgbPix.rgbGreen);
//	  Pixel = (rgbPix.rgbBlue + rgbPix.rgbGreen)/2;
      ++n;
      lpDIBBits[n] = Pixel;
    }
  }
  if(nBitCount == 24){
    for(DWORD idx=0; idx < Size; idx +=3){
      lpPixel = m_pDIB->m_lpSrcBits + idx;
      rgbPix.rgbBlue = (BYTE)*lpPixel;
      rgbPix.rgbGreen = (BYTE)*(lpPixel+1);
      rgbPix.rgbRed = (BYTE)*(lpPixel+2);
//	  Pixel = (rgbPix.rgbBlue + rgbPix.rgbGreen + rgbPix.rgbRed)/3;
      Pixel = __max(rgbPix.rgbRed, rgbPix.rgbGreen);
      Pixel = __max(Pixel, rgbPix.rgbBlue);
      ++n;
      lpDIBBits[n] = Pixel;
    }
  }
  else{
      delete pDib;
      delete [] (BYTE *)lpbi;
      free(lpDIBBits);
      ::DeleteObject((HGDIOBJ)hBitmap);
      return FALSE;
  }

    memcpy(pDib->m_lpSrcBits, lpDIBBits, newSize);
	pDib->m_bIsPadded = 0;
    pDib->SaveImage(fname);
    delete pDib;
    delete [] (BYTE *)lpbi;
    free(lpDIBBits);
    ::DeleteObject((HGDIOBJ)hBitmap);

    if(m_pDIB)
		delete m_pDIB;
    m_pDIB = new SECDib;
    m_pDIB->LoadImage(fname);
    return TRUE;
}

bool CImageCtrls::LoadImage(CString& fname)
{
    //Note, could have done new SECDIB(filename) too?

    if(m_pDIB)
		delete m_pDIB;
    m_pDIB = new SECDib;

    if(!ConvertToDIB(fname))
          return false;
	
    if (m_pDIB->LoadImage(LPCTSTR(fname)) == FALSE){
		TCHAR buffer[256];
		_stprintf(buffer,_T("Imagetst could not load DIB/BMP file: %s!"), LPCTSTR(fname));
		AfxMessageBox(buffer,MB_ICONEXCLAMATION|MB_OK);
		return false;
    }

    ImageSize.cx = (int)m_pDIB->m_dwWidth;
    ImageSize.cy = (int)m_pDIB->m_dwHeight;

	return true;
}

void CImageCtrls::SaveImage(LPCTSTR fname)
{
	if(!m_pDIB)
		return;
	 int Type;
     CString FileExt = fname;
	 FileExt = FileExt.Right(3);
     FileExt.MakeUpper();
     if(FileExt == "BMP"){
      Type = T_BMP;
     }
     else if(FileExt == "PCX"){
         Type = T_PCX;
     }
     else if(FileExt == "JPG"){
         Type = T_JPG;
     }
     else if(FileExt == "GIF"){
        Type = T_GIF;
     }
     else if(FileExt == "TGA"){
         Type = T_TGA;
     }
     else if(FileExt == "TIF"){
         Type = T_TIF;
     }
	switch(Type){
	  case T_BMP: OnFileSaveAsDib(fname); break;
	  case T_PCX: OnFileSaveAsPcx(fname); break;
	  case T_JPG: OnFileSaveAsJpeg(fname); break;
	  case T_GIF: OnFileSaveAsGif(fname); break;
	  case T_TGA: OnFileSaveAsTga(fname); break;
	  case T_TIF: OnFileSaveAsTiff(fname); break;
	}
}

void CImageCtrls::OnFileSaveAsDib(LPCTSTR fname)
{
	if(fname){
			m_pDIB->SaveImage(fname);
	}
	else{
		CFileDialog dlg(FALSE, _T("dib"), _T("*.dib"));
		if (dlg.DoModal() == IDOK)
		{
			m_pDIB->SaveImage(dlg.GetPathName());
		}
	}
}

void CImageCtrls::OnFileSaveAsGif(LPCTSTR fname)
{
	if(fname){
			SECGif *pGif = new SECGif();
			if (!pGif)
				return;
			pGif->ConvertImage(m_pDIB);
			pGif->SaveImage(fname);
			m_pDIB->ConvertImage(pGif);
			delete pGif;
	}
	else{
		CFileDialog dlg(FALSE, _T("gif"), _T("*.gif"));
		if (dlg.DoModal() == IDOK)
		{
			SECGif *pGif = new SECGif();
			if (!pGif)
				return;
			pGif->ConvertImage(m_pDIB);
			pGif->SaveImage(dlg.GetPathName());
			m_pDIB->ConvertImage(pGif);
			delete pGif;
		}
	}
}

void CImageCtrls::OnFileSaveAsJpeg(LPCTSTR fname)
{
	if(fname){
			SECJpeg *pJpeg = new SECJpeg();
			if (!pJpeg)
				return;
			pJpeg->ConvertImage(m_pDIB);
			pJpeg->SaveImage(fname);
			m_pDIB->ConvertImage(pJpeg);
			delete pJpeg;
	}
	else{
		CFileDialog dlg(FALSE, _T("jpg"), _T("*.jpg"));
		if (dlg.DoModal() == IDOK)
		{
			SECJpeg *pJpeg = new SECJpeg();
			if (!pJpeg)
				return;
			pJpeg->ConvertImage(m_pDIB);
			pJpeg->SaveImage(dlg.GetPathName());
			m_pDIB->ConvertImage(pJpeg);
			delete pJpeg;
		}
	}
}


void CImageCtrls::OnFileSaveAsPcx(LPCTSTR fname)
{
	if(fname){
			SECPcx *pPcx = new SECPcx();
			if (!pPcx)
				return;
			pPcx->ConvertImage(m_pDIB);
			pPcx->SaveImage(fname);
			m_pDIB->ConvertImage(pPcx);
			delete pPcx;
	}
	else{
		CFileDialog dlg(FALSE, _T("pcx"), _T("*.pcx"));
		if (dlg.DoModal() == IDOK)
		{
			SECPcx *pPcx = new SECPcx();
			if (!pPcx)
				return;
			pPcx->ConvertImage(m_pDIB);
			pPcx->SaveImage(dlg.GetPathName());
			m_pDIB->ConvertImage(pPcx);
			delete pPcx;
		}
	}
}

void CImageCtrls::OnFileSaveAsTga(LPCTSTR fname)
{
	if(fname){
			SECTarga *pTga= new SECTarga();
			if (!pTga)
				return;
			pTga->ConvertImage(m_pDIB);
			pTga->SaveImage(fname);
			m_pDIB->ConvertImage(pTga);
			delete pTga;
	}
	else{
		CFileDialog dlg(FALSE, _T("tga"), _T("*.tga"));
		if (dlg.DoModal() == IDOK)
		{
			SECTarga *pTga= new SECTarga();
			if (!pTga)
				return;
			pTga->ConvertImage(m_pDIB);
			pTga->SaveImage(dlg.GetPathName());
			m_pDIB->ConvertImage(pTga);
			delete pTga;
		}
	}
}

void CImageCtrls::OnFileSaveAsTiff(LPCTSTR fname)
{
	if(fname){
			SECTiff *pTiff= new SECTiff();
			if (!pTiff)
				return;
			pTiff->ConvertImage(m_pDIB);
			pTiff->SaveImage(fname);
			m_pDIB->ConvertImage(pTiff);
			delete pTiff;
	}
	else{
		CFileDialog dlg(FALSE, _T("tif"), _T("*.tif"));
		if (dlg.DoModal() == IDOK)
		{
			SECTiff *pTiff= new SECTiff();
			if (!pTiff)
				return;
			pTiff->ConvertImage(m_pDIB);
			pTiff->SaveImage(dlg.GetPathName());
			m_pDIB->ConvertImage(pTiff);
			delete pTiff;
		}
	}
}

/*****************  ��������� PCX  **************/
struct pcxheader {
                 char       manuf;          /* �ᥣ�� =10 ��� Paintbrush     */
                 char       hard;           /* ���ଠ�� � ���ᨨ           */
                 char       encod;          /* ��㯯���� ����஢����(=1)     */
                 char       bitpx;          /* ��� �� ���                  */
                 int        x1;             /* ������� ���⨭��(������.)    */
                 int        y1;
                 int        x2;
                 int        y2;
                 int        hres;           /* ��ਧ.ࠧ�襭�� ��ᯫ��      */
                 int        vres;           /* ���⨪.ࠧ�襭�� ��ᯫ��     */
                 char       clrma[48];      /* ������                       */
                 char       vmode;          /* (����������)                */
                 char       nplanes;        /* ���-�� ���᪮�⥩ (���.2.5=0) */
                 int        bplin;          /* ���� �� ��ப�                */
                 int        palinfo;        /* ����.� ������ (1=�.,2=��)*/
                 int        shres;          /* ����襭�� ᪠����           */
                 int        svres;          /*                               */
                 char       xtra[54];       /* ���.���⮥ ���� (䨫���)     */
                 }      hh;


/*========================================================================*/
/*
void CImageCtrls::OnFileSaveAsPcx(LPCTSTR fname)
{
  int             i,j,k,n;
  unsigned char   *img,*pc;
  int width, height;
  char   rgb[256*4];   

  FILE* file;
  file = fopen(fname, "wb");
  
  width = m_pDIB->dwGetWidth() + 1;
  height = m_pDIB->dwGetHeight() + 1;

  hh.manuf=10;
  hh.hard=5;
  hh.encod=1;
  hh.bitpx=8;
  hh.x1=0;
  hh.y1=0;
  hh.x2=width-1;
  hh.y2=height-1;
  hh.hres=0;
  hh.vres=0;
  hh.vmode=0;
  hh.nplanes=1;
  hh.bplin=width;
  hh.palinfo=2;
  hh.shres=0;
  hh.svres=0;
  fwrite(&hh,128,1, file);

  unsigned char* lpDIBBits = (unsigned char*)m_pDIB->m_lpSrcBits;
  pc=(unsigned char*) malloc((long)width*2*sizeof(unsigned char));
  img=lpDIBBits+512;
  for(i=0;i<height;i++)
  {
  for(k=j=0;k<width;k++)
      {
      if(img[k]==img[k+1] && k+1<width)
        {
        for(n=1;n<63 && k+1<width && img[k]==img[k+1];n++,k++);
        pc[j++]=n+0xC0;
        pc[j++]=img[k];
        }
      else
        {
        if(img[k]>=(unsigned char)0xC0) pc[j++]=0xC1;
        pc[j++]=img[k];
        }
      }
  fwrite(pc,j,1,file);
  img+=512;
  }
for(i=0;i<256;i++)
  {
  rgb[(i+i+i)]=i;
  rgb[(i+i+i)+1]=i;
  rgb[(i+i+i)+2]=i;
  }
 fwrite("\14",1,1,file);
 fwrite(rgb,3,256,file);
 free(pc);
}
*/
/*========================================================================*/

