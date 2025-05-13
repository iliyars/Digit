#include "..\..\StdAfx.h"
#include "..\..\Include\Utils\LOgoBmp.h"
#include "..\..\Include\Utils\memdc.h"

//////////////////
// Get size (width, height) of bitmap.
// extern fn works for ordinary CBitmap objects.
//
static CSize PLGetBitmapSize(CBitmap* pBitmap)
  {
  BITMAP bm;
  return pBitmap->GetBitmap(&bm) ? CSize(bm.bmWidth, bm.bmHeight) : CSize(0, 0);
  }

//////////////////
// You can use this static function to draw ordinary
// CBitmaps as well as CDibs
//
static BOOL PLDrawBitmap(CDC& dc, CBitmap* pBitmap, const CRect* rcDst, const CRect* rcSrc, DWORD dwRop)
  {
  // Compute rectangles where NULL specified
  CRect rc;
  if (!rcSrc) 
    {
    // if no source rect, use whole bitmap
    rc = CRect(CPoint(0,0), PLGetBitmapSize(pBitmap));
    rcSrc = &rc;
    }
  if (!rcDst) 
    {
    // if no destination rect, use source
    rcDst = rcSrc;
    }
  
  // Create memory DC
  CDC memdc;
  memdc.CreateCompatibleDC(&dc);
  CBitmap* pOldBm = memdc.SelectObject(pBitmap);
  
  // Blast bits from memory DC to target DC.
  // Use StretchBlt if size is different.
  //
  BOOL bRet = FALSE;
  if (rcDst->Size() == rcSrc->Size()) 
    {
    bRet = dc.BitBlt(rcDst->left, rcDst->top, rcDst->Width(), rcDst->Height(),
      &memdc, rcSrc->left, rcSrc->top, dwRop);
    } 
  else 
    {
    dc.SetStretchBltMode(COLORONCOLOR);
    bRet = dc.StretchBlt(rcDst->left, rcDst->top, rcDst->Width(),
      rcDst->Height(), &memdc, rcSrc->left, rcSrc->top, rcSrc->Width(),
      rcSrc->Height(), dwRop);
    }
  memdc.SelectObject(pOldBm);
  
  return bRet;
  }

void FillLogoInDC(CBitmap& m_LogoBitmap, CDC* pDC, CRect rc)
{
    CMemDC memDC(pDC); 
    
    CBrush *pOldBrush = NULL;
    CBrush brush;
    
    brush.CreateSolidBrush(RGB(101, 126, 158));
    pOldBrush = memDC.SelectObject(&brush);
    memDC.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATCOPY);
    
    if (pOldBrush != NULL)
      memDC.SelectObject(pOldBrush);
    brush.DeleteObject();
    
    CSize bsz = PLGetBitmapSize(&m_LogoBitmap);
/*
    CRect dest;
    dest.left = (rc.Width() - bsz.cx) / 2;
    dest.top = (rc.Height() - bsz.cy) / 2;  
    dest.right = dest.left + bsz.cx;
    dest.bottom = dest.top + bsz.cy;
*/    
    int start_col, end_col, start_row, end_row;

    start_col = (rc.left - 1) / bsz.cx;
    if (start_col < 0)
      start_col = 0;
    start_row = (rc.top - 1) / bsz.cy;
    if (start_row < 0)
      start_row = 0;
    end_col = (rc.right + 1) / bsz.cx;
    end_row = (rc.bottom + 1) / bsz.cy;

    int x = start_col * bsz.cx;
    for (int i = start_col; i <= end_col; i++, x += bsz.cx)
      {
      int y = start_row * bsz.cy;
      for (int j = start_row; j <= end_row; j++, y += bsz.cy)
        {
        CRect dest;
        dest.left = x;
        dest.top = y;
        dest.right = dest.left + bsz.cx;
        dest.bottom = dest.top + bsz.cy;
        PLDrawBitmap(memDC, &m_LogoBitmap, &dest, NULL, SRCCOPY);
        }
      }     

//    PLDrawBitmap(memDC, &m_LogoBitmap, &dest, NULL, SRCCOPY);
}
