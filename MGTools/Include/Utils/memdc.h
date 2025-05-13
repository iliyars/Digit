// MemDC.h: interface for the CMemDC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMDC_H__82753431_5ECB_11D3_AF7E_008048E3C7D8__INCLUDED_)
#define AFX_MEMDC_H__82753431_5ECB_11D3_AF7E_008048E3C7D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMemDC : public CDC  
  {
  public:
    CMemDC(CDC* pDC) : CDC(), m_oldBitmap(NULL), m_pDC(pDC)
      {
      ASSERT(m_pDC != NULL);  // If you asserted here, you passed in a NULL CDC.
      
      m_bMemDC = !pDC->IsPrinting();
      if (m_bMemDC)
        {
        // Create a Memory DC
        CreateCompatibleDC(pDC);
        pDC->GetClipBox(&m_rect);
        m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
        m_oldBitmap = SelectObject(&m_bitmap);
        SetWindowOrg(m_rect.left, m_rect.top);
        } 
      else 
        {
        // Make a copy of the relevent parts of the current DC for printing
        m_bPrinting = pDC->m_bPrinting;
        m_hDC = pDC->m_hDC;
        m_hAttribDC = pDC->m_hAttribDC;
        }
      }
    
    CMemDC(CDC* pDC,  const CRect& rcBounds) : CDC(), m_oldBitmap(NULL), m_pDC(pDC),  m_rect(rcBounds)
      {
      ASSERT(m_pDC != NULL);  // If you asserted here, you passed in a NULL CDC.
      
      m_bMemDC = !pDC->IsPrinting();
      if (m_bMemDC)
        {
        // Create a Memory DC
        CreateCompatibleDC(pDC);
        m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
        m_oldBitmap = SelectObject(&m_bitmap);
        SetWindowOrg(m_rect.left, m_rect.top);
        } 
      else 
        {
        // Make a copy of the relevent parts of the current DC for printing
        m_bPrinting = pDC->m_bPrinting;
        m_hDC = pDC->m_hDC;
        m_hAttribDC = pDC->m_hAttribDC;
        }
      }
    
    ~CMemDC()
      {
      if (m_bMemDC) 
        {
        // Copy the offscreen bitmap onto the screen.
        m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
          this, m_rect.left, m_rect.top, SRCCOPY);
        // Swap back the original bitmap.
        SelectObject(m_oldBitmap);
        } 
      else 
        {
        // All we need to do is replace the DC with an illegal value,
        // this keeps us from accidently deleting the handles associated with
        // the CDC that was passed to the constructor.
        m_hDC = m_hAttribDC = NULL;
        }
      }
    
    // Allow usage as a pointer
    CMemDC* operator->() { return this; }
    
    // Allow usage as a pointer
    operator CMemDC*() { return this; }
    
  private:
    CBitmap m_bitmap;	 // Offscreen bitmap
    CBitmap* m_oldBitmap;	 // bitmap originally found in CMemDC
    CDC* m_pDC;		 // Saves CDC passed in constructor
    CRect m_rect;		 // Rectangle of drawing area.
    BOOL m_bMemDC;	 // TRUE if CDC really is a Memory DC.
  };

#endif // !defined(AFX_MEMDC_H__82753431_5ECB_11D3_AF7E_008048E3C7D8__INCLUDED_)
