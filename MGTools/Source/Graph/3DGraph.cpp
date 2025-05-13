// 3DGraph.cpp : implementation file

#include "..\..\stdafx.h"
#include "..\..\MGTools.h"
#include "..\..\Include\Graph\def.h"
#include "..\..\Include\Graph\3DGraph.h"
#include "..\..\Include\Utils\Utils.h"
#include "..\..\Include\Utils\DibApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// C3DGraph

IMPLEMENT_DYNCREATE(C3DGraph, C2DGraph)

C3DGraph::C3DGraph(LPCTSTR _iniFile/*NULL*/)
         :C2DGraph(_iniFile)
{
	hDC = NULL;
	mesh_pal = NULL;
    bRender = FALSE;
    minX = maxX = minY = maxY = minZ = maxZ = D_EMPTY;
    LX = LY = LZ = D_EMPTY;
    TransX = TransY = TransZ = 0.f;
    rotLight = 0.f;

	pMatr = NULL;
	ctlpoints = NULL;
	facetnormals = NULL;
	normals = NULL;
    nx = ny = 0;
	isInvertBkColor = FALSE;
	EnablePlay = FALSE;
	ViewColor = VIEW_COLOR;
	WidthLegeng = 80;
    EnableMenuPlay = FALSE;

	IsPointsList = FALSE;
	IsNormalList = FALSE;
	IsPointsListLog = FALSE;
	IsNormalListLog = FALSE;
}

C3DGraph::~C3DGraph()
{
}

void C3DGraph::Delete()
{
  if(pMatr){
    for (int i = 0; i < ny; i++) {
      free(pMatr[i]);
    }
    free(pMatr);
  }
  pMatr = NULL;

  if(ctlpoints){
    for (int i = 0; i < ny; i++) {
     for (int j = 0; j < nx; j++){
      free(ctlpoints[i][j]);
      free(normals[i][j]);
     }
      free(ctlpoints[i]);
      free(normals[i]);
    }
    if(ctlpoints) {free(ctlpoints); ctlpoints = NULL;}
    if(normals) {free(normals); normals = NULL;}
    ctlpoints = NULL;
  }
}

CRuntimeClass* C3DGraph::GetRunTime()
{
	return RUNTIME_CLASS(C3DGraph);
}

BEGIN_MESSAGE_MAP(C3DGraph, C2DGraph)
	//{{AFX_MSG_MAP(C3DGraph)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
    ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(IDMR_InvertBackGround, OnInvertBackground)
    ON_COMMAND(IDMR_ColorView, OnColorView)
    ON_COMMAND(IDMR_MonoChromView, OnMonochomView)
    ON_COMMAND(IDMR_LogarithmScale, OnLogarithmScale)
	ON_COMMAND(IDMR_CopyClipboard, OnCopyClipboard)
	ON_COMMAND(IDMR_WriteMetaFile, OnSaveAs)
	ON_COMMAND(IDMR_Play, OnPlay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// C3DGraph drawing
BOOL C3DGraph::PreCreateWindow(CREATESTRUCT& cs)
{
// OpenGL требует наличия стилей WS_CLIPCHILDREN и WS_CLIPSIBLINGS
    cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	return CScrollView::PreCreateWindow(cs);
}

int C3DGraph::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CScrollView::OnCreate(lpCreateStruct) == -1)
        return -1;


// Формируем контекст рабочей области
  CClientDC* pDC = new CClientDC(this);
  hDC = pDC->GetSafeHdc();
  if(mesh_pal==NULL)
     mesh_pal = CreateColorScalePalette(hDC, COLOR_MESH_SCALE);

  ASSERT(hDC != NULL);

// Устанавливаем формат пикселей
    if (SetPixelFormat(hDC)==FALSE)
        return 0;

 // Создаем и делаем текущим контекст воспроизведени
    if (CreateGLContext(hDC)==FALSE)
        return 0;

    return 0;
}

void C3DGraph::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
}

void C3DGraph::SetBound(float _minX, float _maxX, float _minY, float _maxY)
{
  ScaleXY = 1.;
  if(_maxX < 1 && _maxY < 1)
	  ScaleXY = 100.f;
  minX = _minX*ScaleXY;
  maxX = _maxX*ScaleXY;
  minY = _minY*ScaleXY;
  maxY = _maxY*ScaleXY;
}

BOOL C3DGraph::Set3DMatrix(float* _pMatr, int _nx, int _ny)
{
  Delete();
  IsPointsList = FALSE;
  IsNormalList = FALSE;
  IsPointsListLog = FALSE;
  IsNormalListLog = FALSE;
  int i = 0;
  nx = _nx+2;
  ny = _ny+2;

  if(pMatr==NULL){
     pMatr = (GLfloat**)malloc(sizeof(GLfloat)*(ny));
     for(int i = 0; i < ny; i++) {
      pMatr[i] = (GLfloat*)malloc(sizeof(GLfloat)*(nx));
     }
  }
  realmaxZ = -100000.f;
  realminZ = 100000.f;
  float val;
  minZ = 100000.f;
  maxZ = -100000.f;
  for(i=0; i < _nx*_ny; i++){
	val = (float)_pMatr[i];
    if(val == -1.E18) continue;
    realminZ = __min(val, realminZ);
    realmaxZ = __max(val, realmaxZ);
  }

  float kZ = realmaxZ - realminZ;

 for(i=0; i < ny; i++){
    for(int j=0; j < nx; j++){
  	  if(!i || i >= _ny || !j || j >= _nx){
		  val = min_level;
	  }
	  else{
          val = (float)_pMatr[j*_nx+i];
	  }
      if(val == min_level){
         val = 0.0f;
      }
	  else{
		 val -= realminZ;
	  }
      pMatr[i][j] = (GLfloat)val/kZ;
      minZ = __min(pMatr[i][j], minZ);
      maxZ = __max(pMatr[i][j], maxZ);
    }
  }

  if(maxZ - minZ < 0.00001){
	  RightMatrix = FALSE;
	  return FALSE;
  }
  else
	  RightMatrix = TRUE;

  Coef_z_Log= 0;//(realmaxZ-realminZ)/log((realmaxZ-realminZ+LogDelta)/LogDelta);
  LogShift= 0;//realminZ-Coef_z_Log*log(LogDelta);

  SetDefAngles();
  InitTriangSurface();
  bRender = FALSE;

  return TRUE;
}

BOOL C3DGraph::PreTranslateMessage(MSG* pMsg)
{
/*  switch(pMsg->message)
    {
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MOUSEMOVE:
	  CString toolText;
	  if(GetToolText(toolText)){
		  m_ToolTips.UpdateTipText(LPCTSTR(toolText), this);
          m_ToolTips.RelayEvent(pMsg);
	  }
	  else{
		  m_ToolTips.Pop();
	  }
    }*/
  return CScrollView::PreTranslateMessage(pMsg);
}

void C3DGraph::OnDraw(CDC* pDC)
{
	CRect clientR;
    if(!bRender){
		GetClientRect(clientR);
		pDC->FillRect(&clientR, &CBrush(RGB(0,0,0)));
		return;
    }

   if(ctlpoints)
 	 DrawPicture(pDC);
}

void C3DGraph::DrawPicture(CDC* pDC)
{
   PIXELFORMATDESCRIPTOR pfd;
   int nGLPixelIndex;
   CRect clientR;
   if (!pDC->IsPrinting()){
    GetClientRect(clientR);
    wglMakeCurrent(hDC, hrc);
   }

	if(isInvertBkColor){
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else{
      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

//    if (!pDC->IsPrinting()){
      int W = clientR.Width();
      int H = clientR.Height();
      glViewport(0, 0, W-WidthLegeng, H);
//	}

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float Z, Z1, Z2;
    float minXY, maxXY;
    minXY = 1.e10;
    maxXY = -1.e10;
    minXY = __min(minXY, minX);
    minXY = __min(minXY, minY);
    maxXY = __max(maxXY, maxX);
    maxXY = __max(maxXY, maxY);
	float maxVVis = __max(maxXY, fabs(minXY));
	if(maxVVis > maxZ){
	   viewScaleZ = maxVVis / maxZ;
	}
	maxVVis = __max(maxVVis, maxZ);
	maxVVis *= 2;
	if(EnableLogarithmScale){
	      Z1 = log10(minZ);
	      Z2 = log10(maxZ);
  	      Z = __max(fabs(Z1),fabs(Z2));
      glOrtho(-maxVVis, maxVVis, -maxVVis, maxVVis, -maxVVis, maxVVis);
	}
	else{
  	  Z = __max(fabs(minZ),fabs(maxZ));
      glOrtho(-maxVVis, maxVVis, -maxVVis, maxVVis, -maxVVis, maxVVis);
	}

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(TransX, TransY-Z/4, TransZ);

    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);
    glScalef(viewScale, viewScale, viewScaleZ);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawBuffer(GL_BACK);
      glDisable(GL_BLEND);

	DrawAxis3D();
    DrawLights();
    DrawTriangleSurf();

    if(ViewColor == VIEW_COLOR)
	  DrawLegend();

  glFlush();

    // Swap the buffers in case of double buffering
    nGLPixelIndex = GetPixelFormat(pDC->GetSafeHdc());
    DescribePixelFormat(pDC->GetSafeHdc(), nGLPixelIndex, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
    if (pfd.dwFlags & PFD_DOUBLEBUFFER)
        SwapBuffers(pDC->GetSafeHdc());

//    if (!pDC->IsPrinting() && hDC != NULL && hRC != NULL)
//        SetCurrent(hDC, hRC);                   //Restore last rendering and device contexts
}

void C3DGraph::RefreshPicture()
{
	if(RightMatrix)
       bRender = TRUE;
	Invalidate(FALSE);
}

void C3DGraph::SetDefAngles()
{
      rotX = -60.0f;
      rotY = 0.0f;
      rotZ = -60.0f;
      viewScale = viewScaleZ = 1.f;
}

void C3DGraph::DrawLights()
{
    if(LX == D_EMPTY && LY == D_EMPTY, LZ == D_EMPTY){
      float minXY, maxXY;
      minXY = 1.e10;
      maxXY = -1.e10;
      minXY = __min(minXY, minX);
      minXY = __min(minXY, minY);
      maxXY = __max(maxXY, maxX);
      maxXY = __max(maxXY, maxY);
      LX = maxXY; LY = -maxXY;  LZ = maxZ + maxZ*0.3;
    }
    glEnable(GL_LIGHT0);
    GLfloat light_position[] = {LX, LY, LZ, 0.0f};
    glPushMatrix();
    glRotatef(rotLight, 0.0f, 0.0f, 1.0f);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glPopMatrix();
  glFlush();
}

BOOL C3DGraph::SetMemDcPixelFormat(HDC hMemDC, BOOL bUseAPI)
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
        1,                              // Version of this structure
        PFD_DRAW_TO_BITMAP |            // Draw to bitmap (not to window)
        PFD_SUPPORT_OPENGL |            // Support OpenGL calls in window
        PFD_STEREO_DONTCARE,            // Don't need stereo mode
        PFD_TYPE_RGBA,                  // RGBA Color mode
        24,                             // Number of color bitplanes
        0,0,0,0,0,0,                    // Not used to select mode
        0,                              // Number of alpha bitplanes
        0,                              // Not used to select mode
        64,                             // Number of bitplanes in the accumulation buffer
        0,0,0,0,                        // Not used to select mode
        32,                             // Size of depth buffer
        8,                              // Size of stencil buffer
        0,                              // Size of auxiliary buffer
        PFD_MAIN_PLANE,                 // Draw in main plane
        0,0,0,0 };                      // Not used to select mode

    int nGLPixelIndex = bUseAPI ?
        ::ChoosePixelFormat(hMemDC, &pfd) :
        ChoosePixelFormat(hMemDC, &pfd);
    if (nGLPixelIndex == 0) // Choose default
        nGLPixelIndex = 1;

    if (DescribePixelFormat(hMemDC, nGLPixelIndex,
        sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
        return FALSE;

    if (!::SetPixelFormat(hMemDC, nGLPixelIndex, &pfd))
        return FALSE;

    return TRUE;
}

BOOL C3DGraph::SetPixelFormat(HDC hdc)
{
 // Заполняем поля структуры необходимыми значениями
  static PIXELFORMATDESCRIPTOR pfd =    {
    sizeof(PIXELFORMATDESCRIPTOR),  // размер структуры
    1,                              // номер версии
    PFD_DRAW_TO_WINDOW   |          // поддержка вывода в окно
    PFD_SUPPORT_OPENGL |          // поддержка OpenGL
    PFD_DOUBLEBUFFER,             // двойная буферизаци
    PFD_TYPE_RGBA,         // цвета в режиме RGBA
    24,                    // 24-разряда на цвет
    0, 0, 0, 0, 0, 0,      // биты цвета игнорируютс
    0,                     // не используется альфа параметр
    0,                     // смещение цветов игнорируютс
    0,                     // буфер аккумулятора не используетс
    0, 0, 0, 0,            // биты аккумулятора игнорируютс
    32,                    // 32-разрядный буфер глубины
    0,                     // буфер трафарета не используетс
    0,                     // вспомогательный буфер не используетс
    PFD_MAIN_PLANE,        // основной слой
    0,                     // зарезервирован
    0, 0, 0                // маски слоя игнорируютс
  };

  int pixelFormat;

// Поддерживает ли система необходимый формат пикселей?
  if((pixelFormat = ::ChoosePixelFormat(hdc, &pfd)) == 0){
    MessageBox("С заданным форматом пикселей работать нельзя");
    return FALSE;
  }

  if (::SetPixelFormat(hdc, pixelFormat, &pfd) == FALSE)
    {
      MessageBox("SetPixelFormat failed");
      return FALSE;
    }

  return TRUE;
}

BOOL C3DGraph::CreateGLContext(HDC hdc)
{

// Создаем контекст воспроизведени
  if((hrc = ::wglCreateContext(hdc)) == NULL)
    return FALSE;

// Делаем контекст воспроизведения текущим
  if(::wglMakeCurrent(hdc, hrc) == FALSE)
    return FALSE;

  return TRUE;
}

void C3DGraph::OnDestroy()
{
	if(EnablePlay){
        EnablePlay = bRender = FALSE;
        m_nTimer = 1;
        KillTimer(1);
        KillTimer(2);
        while(m_nTimer){
          MakeLoopMessage();
		}
	}
   Delete();

   if(mesh_pal)
	  ::DeleteObject(HGDIOBJ(mesh_pal));

// Получаем текущий контекст воспроизведени
  HGLRC hrc = ::wglGetCurrentContext();

// Перед удалением он не должен быть текущим
  ::wglMakeCurrent(NULL,  NULL);

// Удаляем контекст воспроизведени
  if (hrc)
    ::wglDeleteContext(hrc);

// Удаляем контекст рабочей области
   CDC* pDC = CClientDC::FromHandle(hDC);
   if(pDC)
    delete pDC;

  CScrollView::OnDestroy();
}

void C3DGraph::OnSize(UINT nType, int cx, int cy)
{
    CScrollView::OnSize(nType, cx, cy);
}
/////////////////////////////////////////////////////////////////////////////
void C3DGraph::InitTriangSurface()
{
  int i, j;
  if(ctlpoints==NULL){
       ctlpoints = (GLfloat***)malloc(sizeof(GLfloat)*ny);
       normals = (GLfloat***)malloc(sizeof(GLfloat)*ny);
       for(i = 0; i < ny; i++) {
        ctlpoints[i] = (GLfloat**)malloc(sizeof(GLfloat)*nx);
        normals[i] = (GLfloat**)malloc(sizeof(GLfloat)*nx);
        for(j = 0; j < nx; j++) {
          ctlpoints[i][j] = (GLfloat*)malloc(sizeof(GLfloat)*3);
          normals[i][j] = (GLfloat*)malloc(sizeof(GLfloat)*3);
        }
       }
  }
  facetnormals = (GLfloat***)malloc(sizeof(GLfloat)*ny);
  for(i = 0; i < ny; i++) {
        facetnormals[i] = (GLfloat**)malloc(sizeof(GLfloat)*nx);
        for(j = 0; j < nx; j++) {
          facetnormals[i][j] = (GLfloat*)malloc(sizeof(GLfloat)*3);
        }
  }

  GLfloat u[3], v[3], n[3], l;
  float dx = float((maxX - minX)/(nx-1));
  float dy = float((maxY - minY)/(ny-1));
  float fj, fi;
  float stx = minX;
  float sty = minY;
  float val;
  i = j = 0;
  for (i = 0; i < ny-1; i++) {
    for (j = 0; j < nx-1; j++) {
        fi = (GLfloat)(stx+i*dx);
        fj = (GLfloat)(sty+j*dy);
        ctlpoints[i][j][0] = fi;
        ctlpoints[i][j][1] = fj;
		val = pMatr[i][j];
        ctlpoints[i][j][2] = val;

        fi = (GLfloat)(stx+(i+1)*dx);
        fj = (GLfloat)(sty+j*dy);
        ctlpoints[i+1][j][0] = fi;
        ctlpoints[i+1][j][1] = fj;
		val = pMatr[i+1][j];
        ctlpoints[i+1][j][2] = val;

        fi = (GLfloat)(stx+i*dx);
        fj = (GLfloat)(sty+(j+1)*dy);
        ctlpoints[i][j+1][0] = fi;
        ctlpoints[i][j+1][1] = fj;
		val = pMatr[i][j+1];
        ctlpoints[i][j+1][2] = val;

        /* get two vectors to cross */
      u[0] = ctlpoints[i][j+1][0] - ctlpoints[i][j][0];
      u[2] = ctlpoints[i][j+1][1] - ctlpoints[i][j][1];
      u[1] = ctlpoints[i][j+1][2] - ctlpoints[i][j][2];

      /* get the normalized cross product */
      normalizedcross(u, v, n);

      v[0] = ctlpoints[i+1][j][0] - ctlpoints[i][j][0];
      v[2] = ctlpoints[i+1][j][1] - ctlpoints[i][j][1];
      v[1] = ctlpoints[i+1][j][2] - ctlpoints[i][j][2];

      /* put the facet normal in the i, j position for later averaging
         with other normals. */
      facetnormals[i][j][0] = n[0];
      facetnormals[i][j][1] = n[2];
      facetnormals[i][j][2] = n[1];
    }
  }

  /* fill in the last vertex & it's facet normal */
  fi = (GLfloat)(stx+i*dx);
  fj = (GLfloat)(sty+j*dy);
  ctlpoints[i][j][0] = fi;
  ctlpoints[i][j][1] = fj;
  val = pMatr[i][j];
  ctlpoints[i][j][2] = val;

  facetnormals[i][j][0] = n[0];
  facetnormals[i][j][1] = n[2];
  facetnormals[i][j][2] = n[1];
  /* calculate normals for the mesh */
  for (i = 1; i < ny-1; i++) {
    for (j = 1; j < nx-1; j++) {
      /* average all the neighboring normals. */
      n[0] = facetnormals[i-1][j-1][0];
      n[2] = facetnormals[i-1][j-1][1];
      n[1] = facetnormals[i-1][j-1][2];

      n[0] += facetnormals[i][j-1][0];
      n[2] += facetnormals[i][j-1][1];
      n[1] += facetnormals[i][j-1][2];

      n[0] += facetnormals[i-1][j][0];
      n[2] += facetnormals[i-1][j][1];
      n[1] += facetnormals[i-1][j][2];

      n[0] += facetnormals[i][j][0];
      n[2] += facetnormals[i][j][1];
      n[1] += facetnormals[i][j][2];

      l = (GLfloat)sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
      normals[i][j][0] = n[0] /= l;
      normals[i][j][1] = n[2] /= l;
      normals[i][j][2] = n[1] /= l;
    }
  }
  /* fill in the normals on the top/bottom edge of the mesh (simply
     copy the one below/above it). */
  for (i = 0; i < ny; i++) {
    normals[i][0][0] = normals[i][1][0];
    normals[i][0][1] = normals[i][1][1];
    normals[i][0][2] = normals[i][1][2];

    normals[i][nx-1][0] = normals[i][nx-2][0];
    normals[i][nx-1][1] = normals[i][nx-2][1];
    normals[i][nx-1][2] = normals[i][nx-2][2];
  }

  /* fill in the normals on the left/right edge of the mesh (simply
     copy the one right/left of it). */
  for (j = 0; j < nx; j++) {
    normals[0][j][0] = normals[1][j][0];
    normals[0][j][1] = normals[1][j][1];
    normals[0][j][2] = normals[1][j][2];

    normals[ny-1][j][0] = normals[ny-2][j][0];
    normals[ny-1][j][1] = normals[ny-2][j][1];
    normals[ny-1][j][2] = normals[ny-2][j][2];
  }

  for (i = 0; i < ny; i++) {
    for (j = 0; j < nx; j++){
      free(facetnormals[i][j]);
    }
      free(facetnormals[i]);
  }
  free(facetnormals);
}

void C3DGraph::TriangSurfList()
{
  int i, j;

  glNewList(1, GL_COMPILE);
  for (i = 0; i < ny - 1; i++) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLE_STRIP);

    int id;
    float pCol[3];
    float z;
    for(j = 0; j < nx; j++) {
      z = ctlpoints[i+1][j][2];
      z = (z-minZ)/(maxZ-minZ);
      id = int(z*PAL_COLORS);
      if(id<0) id = 0;
      else if(id >PAL_COLORS-1) id = PAL_COLORS-1;
      GetPaletteColor(mesh_pal, id, pCol);
      glColor3fv(pCol);
       glNormal3f(normals[i+1][j][0],normals[i+1][j][1],normals[i+1][j][2]);
       glVertex3f(ctlpoints[i+1][j][0],ctlpoints[i+1][j][1],ctlpoints[i+1][j][2]);
      z = ctlpoints[i][j][2];
        z = (z-minZ)/(maxZ-minZ);
      id = int(z*PAL_COLORS);
      if(id<0) id = 0;
      else if(id >PAL_COLORS-1) id = PAL_COLORS-1;
      GetPaletteColor(mesh_pal, id, pCol);
      glColor3fv(pCol);
       glNormal3f(normals[i][j][0],normals[i][j][1],normals[i][j][2]);
       glVertex3f(ctlpoints[i][j][0],ctlpoints[i][j][1],ctlpoints[i][j][2]);
    }
    glEnd();
  }
  glEndList();
  IsPointsList = TRUE;
}

void C3DGraph::TriangSurfListLog()
{
  int i, j;

  glNewList(2, GL_COMPILE);
  for (i = 0; i < ny - 1; i++) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLE_STRIP);

    int id;
    float pCol[3];
    float z;
    float norm;
    for(j = 0; j < nx; j++) {
      z = ctlpoints[i+1][j][2];
      norm = normals[i+1][j][2];
      z = log10(z);
      norm = log10(norm);
      id = int(z*PAL_COLORS);
      if(id<0) id = 0;
      else if(id >PAL_COLORS-1) id = PAL_COLORS-1;
      GetPaletteColor(mesh_pal, id, pCol);
      glColor3fv(pCol);
       glNormal3f(normals[i+1][j][0],normals[i+1][j][1],norm);
       glVertex3f(ctlpoints[i+1][j][0],ctlpoints[i+1][j][1],z);
      z = ctlpoints[i][j][2];
      norm = ctlpoints[i][j][2];
      z = log10(z);
      norm = log10(norm);
      id = int(z*PAL_COLORS);
      if(id<0) id = 0;
      else if(id >PAL_COLORS-1) id = PAL_COLORS-1;
      GetPaletteColor(mesh_pal, id, pCol);
      glColor3fv(pCol);
       glNormal3f(normals[i][j][0],normals[i][j][1],norm);
       glVertex3f(ctlpoints[i][j][0],ctlpoints[i][j][1],z);
    }
    glEnd();
  }
  glEndList();
  IsPointsListLog = TRUE;
}

void C3DGraph::DrawTriangleSurf()
{
  if(!IsPointsList && !isPrinting)
     TriangSurfList();
  if(isPrinting)
     TriangSurfList();
//  if(!IsPointsListLog)
//     TriangSurfListLog();

  glPushMatrix();
  glEnable(GL_LINE_SMOOTH);
  if(ViewColor == VIEW_COLOR){
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    GLfloat o_mat_diffuse[4];
    GLfloat o_mat_specular[4];
    GLfloat o_mat_shininess[1];

    GLfloat o_diffuse[4];
    GLfloat o_specular[4];
//    GLfloat o_shininess[1];
    GLfloat o_ambient[4];

    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, o_mat_diffuse);
    glGetMaterialfv(GL_FRONT, GL_SPECULAR, o_mat_specular);
    glGetMaterialfv(GL_FRONT, GL_SHININESS, o_mat_shininess);

    glGetLightfv(GL_LIGHT0, GL_AMBIENT, o_ambient);
    glGetLightfv(GL_LIGHT0, GL_DIFFUSE, o_diffuse);
    glGetLightfv(GL_LIGHT0, GL_SPECULAR, o_specular);

    GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position[] = { 0.0f, 2.0f, 2.0f, 0.0f };
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_shininess[] = { 30.0f };
    GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

//    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
//    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
//    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
//    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glColor3f(0.0, 0.0, 0.0);
    if(EnableLogarithmScale)
      glCallList(2);
	else
      glCallList(1);
    glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
  }
  else if(ViewColor == VIEW_MONOCHROM){
    GLfloat o_mat_diffuse[4];
    GLfloat o_mat_specular[4];
    GLfloat o_mat_shininess[1];

    GLfloat o_diffuse[4];
    GLfloat o_specular[4];
//    GLfloat o_shininess[1];
    GLfloat o_ambient[4];

    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, o_mat_diffuse);
    glGetMaterialfv(GL_FRONT, GL_SPECULAR, o_mat_specular);
    glGetMaterialfv(GL_FRONT, GL_SHININESS, o_mat_shininess);

    glGetLightfv(GL_LIGHT0, GL_AMBIENT, o_ambient);
    glGetLightfv(GL_LIGHT0, GL_DIFFUSE, o_diffuse);
    glGetLightfv(GL_LIGHT0, GL_SPECULAR, o_specular);

    GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat position[] = { 0.0f, 2.0f, 2.0f, 0.0f };
    GLfloat mat_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat mat_specular[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    GLfloat mat_shininess[] = { 50.0f };
    GLfloat lmodel_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
//    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

    if(EnableLogarithmScale)
      glCallList(2);
	else
      glCallList(1);

    glMaterialfv(GL_FRONT, GL_DIFFUSE, o_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, o_mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, o_mat_shininess);

    glLightfv(GL_LIGHT0, GL_AMBIENT, o_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, o_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, o_specular);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_AUTO_NORMAL);
    glDisable(GL_NORMALIZE);

    glPopMatrix();
  }

  glDisable(GL_LINE_SMOOTH);
  glFlush();
}

/////////////////////////////////////////////////////////////////////////////
// C3DGraph diagnostics

#ifdef _DEBUG
void C3DGraph::AssertValid() const
{
	CScrollView::AssertValid();
}

void C3DGraph::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// C3DGraph message handlers

void normalizedcross(GLfloat* u, GLfloat* v, GLfloat* n)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  GLfloat l;

  /* compute the cross product (u x v for right-handed [ccw]) */
  n[0] = u[1] * v[2] - u[2] * v[1];
  n[1] = u[2] * v[0] - u[0] * v[2];
  n[2] = u[0] * v[1] - u[1] * v[0];

  /* normalize */
  l = (GLfloat)sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
  n[0] /= l;
  n[1] /= l;
  n[2] /= l;
}

void C3DGraph::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_LButtonDown = TRUE;
    m_LDownPos = point;

    SetCapture();

	CScrollView::OnLButtonDown(nFlags, point);
}

void C3DGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
  m_LButtonDown = FALSE;
  ReleaseCapture();

	CScrollView::OnLButtonUp(nFlags, point);
}

void C3DGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	if(GetCapture()==this){
		if (m_LButtonDown){
          if (GetAsyncKeyState(VK_SHIFT) < 0){
            CSize trans = m_LDownPos - point;
            viewScale -= (float)trans.cy / 32.;
			viewScale = fabs(viewScale);
		  }
          else if (GetAsyncKeyState(VK_CONTROL) < 0){
            CSize trans = m_LDownPos - point;
            TransX -= (float)trans.cx / 32.;
            TransY += (float)trans.cy / 32.;
		  }
         else{
			CSize rotate = m_LDownPos - point;
			if(GetAsyncKeyState(0x4C)){ // L (light) - key
				LZ -= (float)rotate.cx;
          		LX -= (float)rotate.cy;
			}
			else{
				rotZ -= (float)rotate.cx / 2.;
				rotX -= (float)rotate.cy / 2.;
			}
		 }
		 m_LDownPos = point;
		 RefreshPicture();
		}
   }
   CScrollView::OnMouseMove(nFlags, point);
}

void C3DGraph::OnRButtonDown(UINT nFlags, CPoint point)
{
    CScrollView::OnRButtonDown(nFlags, point);
	CreateMenu(point);
}

void C3DGraph::CreateMenu(CPoint P)
{
   CMenu Main;
   Main.CreatePopupMenu();

   UINT FlagInvertBkground;
   UINT FlagWriteMetafile;
   UINT FlagClipboard;
   UINT FlagMonoChrom;
   UINT FlagColor;
   UINT FlagLogarithmScale;
   UINT FlagPrint;
   UINT FlagPlay;
   LPCTSTR Title;

   if(EnableLogarithmScale)
	   FlagLogarithmScale = (MF_STRING | MF_CHECKED);
   else
	   FlagLogarithmScale = MF_STRING | MF_GRAYED;

   FlagInvertBkground = MF_STRING;
   FlagWriteMetafile = MF_STRING;
   FlagClipboard = MF_STRING;
   FlagPrint = MF_STRING;
   if(ViewColor == VIEW_COLOR){
	   FlagMonoChrom = MF_STRING;
	   FlagColor = MF_GRAYED | MF_STRING;
   }
   else if(ViewColor == VIEW_MONOCHROM){
	   FlagMonoChrom = MF_GRAYED | MF_STRING;
	   FlagColor = MF_STRING;
   }

   if(EnablePlay)
	   FlagPlay = MF_STRING|MF_CHECKED;
   else
	   FlagPlay = MF_STRING;
   if(LangType == MG_ENG_LANG) Title = _T("Invert Background");
   else Title = _T("Инвертировать фон");
   Main.AppendMenu(FlagInvertBkground, IDMR_InvertBackGround, Title);
   Main.AppendMenu(MF_SEPARATOR);
//   Main.AppendMenu(FlagLogarithmScale, IDMR_LogarithmScale, _T("Logarithm Scale"));
//   Main.AppendMenu(MF_SEPARATOR);
   if(LangType == MG_ENG_LANG) Title = _T("Color View");
   else Title = _T("Цветной вид");
   Main.AppendMenu(FlagColor, IDMR_ColorView, Title);
   if(LangType == MG_ENG_LANG) Title = _T("Monochrom View");
   else Title = _T("Монохромный вид");
   Main.AppendMenu(FlagMonoChrom, IDMR_MonoChromView, Title);
   Main.AppendMenu(MF_SEPARATOR);
   if(LangType == MG_ENG_LANG) Title = _T("Copy");
   else Title = _T("Копировать");
   Main.AppendMenu(FlagWriteMetafile, IDMR_CopyClipboard, Title);
   if(LangType == MG_ENG_LANG) Title = _T("Save As");
   else Title = _T("Сохранить как");
   Main.AppendMenu(FlagWriteMetafile, IDMR_WriteMetaFile, Title);
//   if(LangType == MG_ENG_LANG) Title = _T("Print");
//   else Title = _T("Печать");
//   Main.AppendMenu(FlagPrint, ID_FILE_PRINT, Title);
//   Main.AppendMenu(MF_SEPARATOR);
   if(EnableMenuPlay)
    Main.AppendMenu(FlagPlay, IDMR_Play, _T("Play"));

   ClientToScreen(&P);
   Main.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                              P.x, P.y, this);
}

void C3DGraph::OnInvertBackground()
{
	if(isInvertBkColor)
		isInvertBkColor = FALSE;
	else
		isInvertBkColor = TRUE;

    SetTimer(2, 300, NULL);
}

void C3DGraph::OnLogarithmScale()
{
  if(EnableLogarithmScale) EnableLogarithmScale = FALSE;
  else EnableLogarithmScale = TRUE;

   SetTimer(2, 300, NULL);
}

void C3DGraph::OnColorView()
{
  ViewColor = VIEW_COLOR;

    SetTimer(2, 300, NULL);
}

void C3DGraph::OnMonochomView()
{
	ViewColor = VIEW_MONOCHROM;

    SetTimer(2, 300, NULL);
}

void C3DGraph::OnCopyClipboard()
{
    // Clean clipboard of contents, and copy the DIB.
	MakeLoopMessage();
    if (OpenClipboard())
    {
     BeginWaitCursor();
     EmptyClipboard();
     CDC* pDC = GetDC();
     HBITMAP hBit = CopyWindowToBitmap(m_hWnd, PW_CLIENT);
     HPALETTE hPal = GetSystemPalette();
     HDIB hWDib = BitmapToDIB(hBit, hPal);
     SetClipboardData (CF_DIB, CopyHandle((HANDLE) hWDib));
     ReleaseDC(pDC);
     CloseClipboard();
     HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, hWDib);
     EndWaitCursor();
    }
}

void C3DGraph::OnSaveAs()
{
  LPCTSTR lan;
  BOOL key = FALSE;
  LPCTSTR title = "Save File";
  CFileDialog fileDlg(FALSE);
  lan = "BMP Files (bmp)";
  CString str = lan; str += (TCHAR)NULL;
  str += "*.bmp"; str += (TCHAR)NULL;

  fileDlg.m_ofn.lpstrFilter = str;
//  TCHAR strName[_MAX_PATH];

//  fileDlg.m_ofn.lpstrFile = strName;
  fileDlg.m_ofn.lpstrTitle = title;
  fileDlg.m_ofn.nFilterIndex = 1;

  fileDlg.m_ofn.Flags &= ~OFN_FILEMUSTEXIST;
//  TCHAR Str[200];
  if(!PathForSave.IsEmpty())
    fileDlg.m_ofn.lpstrInitialDir = PathForSave.GetBuffer(PathForSave.GetLength());
  else
    fileDlg.m_ofn.lpstrInitialDir = NULL;
  if (fileDlg.DoModal() == IDOK) {
	 MakeLoopMessage();
     CString Name = fileDlg.GetPathName();
     int iP = Name.ReverseFind('\\');
     CString sav = Name.Left(iP);
//     SavePath(GetWorkPath(), sav, GetIniFile());
     CString ext = fileDlg.GetFileExt();
     if(!ext.IsEmpty()){
        Name = Name.Left(Name.GetLength()-4);
     }
       if(fileDlg.m_ofn.nFilterIndex == 1) Name += ".bmp";

       Name.MakeLower();
       if(Name.Find(".bmp") != -1){
         HBITMAP hBit = CopyWindowToBitmap(m_hWnd, PW_CLIENT);
         HPALETTE hPal = GetSystemPalette();
         HDIB hWDib = BitmapToDIB(hBit, hPal);
           CFile file;
           CFileException fe;
           if (!file.Open(LPCTSTR(Name), CFile::modeCreate|CFile::modeWrite, &fe))
           {
              TCHAR szError[1024];
              fe.GetErrorMessage(szError, 1024);
              MessageBox(szError, "", MB_OK);
              return;
           }
           SaveDIB(hWDib, file);
           file.Close();
           HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, hWDib);
           EndWaitCursor();
       }
  }
  PathForSave.ReleaseBuffer();
}

void C3DGraph::DrawText(BOOL Rot, float x, float y, float z, float scale, char *format, ...)
{
  va_list args;
  char buffer[200], *p;

  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
      glTranslatef(x, y, z);
      glScalef(scale, scale, scale/viewScaleZ);
     glLineWidth(1.0f);
  if(Rot){
//    glRotatef(45.f, 1.0f, 0.0f, 0.0f);
    glRotatef(90.f, 0.0f, 1.0f, 0.0f);
    glRotatef(90.f, 0.0f, 0.0f, 1.0f);
  }
    for(p = buffer; *p; p++)
      glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);

    glPopAttrib();
    glPopMatrix();
}

void C3DGraph::DrawAxis3D()
{
    glLineWidth(1.0f);
    float Scl;
    char buf[16];
    glPushMatrix();
     glPushAttrib(GL_ENABLE_BIT);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
      glEnable(GL_DEPTH_TEST);

      glBegin(GL_LINES);
      if(isInvertBkColor)
		  glColor3f(0.0f, 0.0f, 0.0f);
      else
		  glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(minX, minY, 0.f);
      glVertex3f(GLfloat(maxX+maxX*0.2), minY, 0.f);
      if(isInvertBkColor)
		  glColor3f(0.0f, 0.0f, 0.0f);
      else
		  glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(minX, minY, 0.f);
      glVertex3f(minX, GLfloat(maxY+maxY*0.2), 0.f);
      if(isInvertBkColor)
		  glColor3f(0.0f, 0.0f, 0.0f);
      else
		  glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(minX, minY, 0.f);
      glVertex3f(minX, minY, GLfloat(maxZ));
    glEnd();
    Scl = (maxX - minX)/2000.;
      DrawText(FALSE,GLfloat(maxX+maxX*0.2), minY, 0.f, Scl, "X");
      DrawText(FALSE, minX, GLfloat(maxY+maxY*0.2), 0.f, Scl, "Y");
      DrawText(TRUE, minX, minY, GLfloat(maxZ), Scl, "Z");

    Scl = (maxX - minX)/2850.;
      float z;
      if(EnableLogarithmScale)
        z = log10(realmaxZ);
      else
         z = realmaxZ-realminZ;
      sprintf(buf, "%0.3f", z);
      DrawText(TRUE, minX, minY-minY*0.2, GLfloat(maxZ), Scl, buf);

      glDisable(GL_DEPTH_TEST);
    glFlush();
  glPopAttrib();
  glPopMatrix();

    Scl = (maxX - minX)/2850.;

  glPushMatrix();

    glRotatef(0.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(0.0f, 0.0f, 0.0f, 1.0f);
    sprintf(buf, "%0.3f", minX/ScaleXY);
    DrawText(FALSE, minX, minY+minY*0.1, 0.0f, Scl, buf);
    DrawText(FALSE, minX, minY+minY*0.1, 0.0f, Scl, buf);
    sprintf(buf, "%0.3f", maxX/ScaleXY);
    DrawText(FALSE, maxX, minY+minY*0.1, 0.0f, Scl, buf);
  glPopMatrix();

  glPushMatrix();
    glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
    sprintf(buf, "%0.3f", minY/ScaleXY);
    DrawText(FALSE, minX, maxY-minY*0.05, 0.0f, Scl, buf);
    sprintf(buf, "%0.3f", maxY/ScaleXY);
    DrawText(FALSE, maxX, maxY-minY*0.05, 0.0f, Scl, buf);
    glFlush();
    glPopMatrix();

/*
    glLineStipple(2, 0x1C47);
    glEnable(GL_LINE_STIPPLE);
*/
//    glDisable(GL_LINE_STIPPLE);
}

void C3DGraph::DrawLegend()
{
  CRect Cl; GetClientRect(Cl);
  int W = Cl.Width();
  int H = Cl.Height();

  int W1 = WidthLegeng;

  int Xc = W1/2;
  int Yc = H/2;
  int left = Xc - int(W1*0.1);
  int right = Xc + int(W1*0.1);
  int top = Yc - int(H/3);
  int bottom = Yc + int(H/3);

  glPushMatrix();
     glPushAttrib(GL_ENABLE_BIT);
      glDisable(GL_LIGHTING);
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_DEPTH_TEST);

    glViewport(W-W1, 0, W1, H);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, W1, 0, H);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(1.0f);
      if(isInvertBkColor)
		  glColor3f(0.0f, 0.0f, 0.0f);
      else
		  glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINE_LOOP);
      glVertex2i(left, top);
      glVertex2i(right, top);
      glVertex2i(right, bottom);
      glVertex2i(left, bottom);
    glEnd();

  int id=PAL_COLORS-1;
  float f_id;
  f_id = id;
  float s_id = (float)PAL_COLORS/(bottom-top);
  float pCol[3];
  char buf[16];
  float Scl = 0.07f;
  int mid = top + (bottom - top)/2;

  glBegin(GL_LINES);
  for(int i=bottom-1; i > top; i--){
    GetPaletteColor(mesh_pal, id, pCol);
	f_id -= s_id;
	id = (int)f_id;
    glColor3fv(pCol);
    if(i == mid){
      sprintf(buf, "%0.3f", (realmaxZ + realminZ)/2);
    }
    glVertex2i(left+1, i);
    glVertex2i(right-1, i);
  }
  glEnd();

  if(isInvertBkColor)
	  glColor3f(0.0f, 0.0f, 0.0f);
     else
      glColor3f(1.0f, 1.0f, 1.0f);

  float z;
  if(EnableLogarithmScale)
      z = log10(realminZ);
  else
      z = realminZ-realminZ;
  sprintf(buf, "%0.3f", z);
  DrawText(FALSE, left, top-(bottom-top)*0.05, 0.0f, Scl, buf);
  if(EnableLogarithmScale)
      z = log10(z);
  else
      z = realmaxZ-realminZ;
  sprintf(buf, "%0.3f", z);
  DrawText(FALSE, left,bottom+(bottom-top)*0.02, 0.0f, Scl, buf);

  glFlush();
  glPopAttrib();
  glPopMatrix();
}

void C3DGraph::WriteText(int X, int Y, LPCTSTR Str, UINT Align)
{
   CDC* pDC = CClientDC::FromHandle(hDC);

   if(isInvertBkColor)
	pDC->SetBkColor(RGB(255, 255, 255));
   else
	pDC->SetBkColor(RGB(0, 0, 0));

   pDC->SetTextAlign(Align);
   pDC->TextOut(X,Y,Str,lstrlen(Str));
}

void C3DGraph::OnPlay()
{
    EnablePlay = EnablePlay ? FALSE : TRUE;
    if (EnablePlay)
        SetTimer(1, 20, NULL);
    else{
        KillTimer(1);
		MakeLoopMessage();
	    SetTimer(2, 350, NULL);
    }
}

void C3DGraph::OnTimer(UINT nIDEvent)
{
	if(nIDEvent==1){
	  rotZ -= (float)1.;
//      DrawPicture();
	  m_nTimer = 0;
	}
	else{
	  KillTimer(nIDEvent);
	  m_nTimer = 0;
  	  MakeLoopMessage();
	  RefreshPicture();
	}

    CScrollView::OnTimer(nIDEvent);
}

void C3DGraph::GetCurrent(HDC& hDC, HGLRC& hRC)
{
	hDC = wglGetCurrentDC();
	hRC = wglGetCurrentContext();
}

BOOL C3DGraph::SetCurrent(HDC hDC, HGLRC hRC)
{
	return wglMakeCurrent(hDC, hRC);
}
BOOL C3DGraph::OnPreparePrinting(CPrintInfo* pInfo)
{
	// set the page count if not previously set
    pInfo->SetMaxPage(1);

	// default preparation
	return DoPreparePrinting(pInfo);
}

void C3DGraph::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CMGToolsApp *pApp = (CMGToolsApp *)AfxGetApp();
/*	if (pApp->m_nPrintObject == 2)
	{
		if (pApp->IsKindOf(RUNTIME_CLASS(CMGToolsApp)))
		{
			pApp->BeginPrinting(pDC);
		}
	}*/

	// Note: This value will only be used in CGlDocument::OnPrint
	CRect	mRect;			// Position of this window's client area
	// Get the Views size (client area!)
	CView::GetClientRect(&mRect);
	m_DocPrintHeight = mRect.Height();

	// Gather some general information about the printer the
	// user selected..
	LPDEVMODE* pMode = (LPDEVMODE*) pInfo->m_pPD->m_pd.hDevMode;
	DEVMODE* mode = *pMode;
	CString mMsg;
	TCHAR* pChar = (TCHAR*)&mode->dmDeviceName[0];

	int nBitsPerPixel = pDC->GetDeviceCaps(BITSPIXEL);
	int nColors		= pDC->GetDeviceCaps(NUMCOLORS);
	int nBitPlanes	= pDC->GetDeviceCaps(PLANES);
	int nColorRes	= pDC->GetDeviceCaps(COLORRES);
	int nRasterCaps	= pDC->GetDeviceCaps(RASTERCAPS);

	// Does the device support raster operations?
	if (nRasterCaps & RC_STRETCHDIB)
		m_bStretchDIBits = TRUE;
	else
		m_bStretchDIBits = FALSE;

	if (nRasterCaps & RC_STRETCHBLT)
		m_bStretchBlt = TRUE;
	else
		m_bStretchBlt = FALSE;

/*	TRACE("You selected printer '%s'\n", pChar);
	TRACE("BitsPerPixel           = %d\n", nBitsPerPixel);
	TRACE("NumColor               = %d\n", nColors);
	TRACE("NumBitPlanes           = %d\n", nBitPlanes);
	TRACE("ColorResolution        = %d\n", nColorRes);
	TRACE("Supports StretchBlt    = %d\n", m_bStretchBlt);
	TRACE("Supports StretchDIBits = %d\n", m_bStretchDIBits);

	PrepareBkgnd(TRUE);

	if (!m_bStretchDIBits && !m_bStretchBlt)
	{
		mMsg.Format(IDS_ERROR_PRINTER_NO_SUPPORT_MSG, "\"", pChar, "\" ");
		AfxMessageBox(mMsg, MB_OK | MB_ICONERROR);
	}*/
}

void C3DGraph::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CMGToolsApp *pApp = (CMGToolsApp *)AfxGetApp();
/*	if (pApp->m_nPrintObject == 2)
	{
		if (pApp->IsKindOf(RUNTIME_CLASS(CMGToolsApp)))
		{
			pApp->EndPrinting(pDC);
		}
	}

	PrepareBkgnd(FALSE);

	// Note: This value will only be used in CGlDocument::OnPrint
	m_DocPrintHeight = 0;*/
}

void C3DGraph::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CMGToolsApp *pApp = (CMGToolsApp *)AfxGetApp();

	SIZE	res;			// Device resolution
	RECT	rect;			// Page rectangle
	HBRUSH	brush;			// Background brush for page
	POINT	PageSize;		// Size of printed image
	POINT	ImageOffset;	// Offset from edges for image
	CRect	mRect;			// Position of this window's client area
	int		nXRes, nYRes;
	HDC		_hDC, hMemDC, hTmpDC;
	HGLRC	_hRC, hMemRC;

	BITMAPINFO	bitmapInfo;
	HBITMAP		hDib;
	LPVOID		pBitmapBits;
	double		fac = 1.;		// try <printer device> DPI

	// Set our device context mapping mode.
	// Each logical unit is converted to 1 device pixel. Positive x is to
	// the right; positive y is down.
	pDC->SetMapMode(MM_TEXT);

	// Get the size of the page (in printer device points)
	rect.top	= 0;
	rect.left   = 0;
	rect.bottom	= 0;
	rect.right   = 0;

	// Get the Views size (client area!)
	CView::GetClientRect(&mRect);

	if (pInfo->m_bPreview)
	{
		PRINTDLG PrtDlg;	// Printer dialog

		if (!pApp->GetPrinterDeviceDefaults(&PrtDlg))
		{
			TRACE("No default printer.\n");
			// Just create a display device context.
			// Note that the DIB image will NOT be
			// positioned properly, but it will at least
			// be displayed.

			if (pDC->IsKindOf(RUNTIME_CLASS(CPreviewDC)))
			{
				CPreviewDC *pPrevDC = (CPreviewDC *)pDC;
				//m_hAttribDC hold the print/fax DC so...
				rect.right =  GetDeviceCaps(pPrevDC->m_hAttribDC, HORZRES);
				rect.bottom = GetDeviceCaps(pPrevDC->m_hAttribDC, VERTRES);
				res.cx = GetDeviceCaps(pPrevDC->m_hAttribDC, LOGPIXELSX);
				res.cy = GetDeviceCaps(pPrevDC->m_hAttribDC, LOGPIXELSY);
			}
		}
		else
		{
			// Don't display the dialog.  We create the CPrintDialog so that
			// we can create a printer device context.
			CPrintDialog dlg(FALSE);

			dlg.m_pd.hDevMode  = PrtDlg.hDevMode;
			dlg.m_pd.hDevNames = PrtDlg.hDevNames;

			HDC hdc = dlg.CreatePrinterDC();

			// Get the size of the default printer page
			rect.right  = GetDeviceCaps(hdc, HORZRES);
			rect.bottom = GetDeviceCaps(hdc, VERTRES);

			// Get the resolution of the printer device
			res.cx = GetDeviceCaps(hdc, LOGPIXELSX);
			res.cy = GetDeviceCaps(hdc, LOGPIXELSY);

			// Free the device context
			DeleteDC(hdc);
		}
	}
	else
	{
		// Get the size of the actual printed page
		rect.right  = GetDeviceCaps(pDC->GetSafeHdc(), HORZRES);
		rect.bottom = GetDeviceCaps(pDC->GetSafeHdc(), VERTRES);

		// Get the resolution of the printer device
		res.cx = GetDeviceCaps(pDC->GetSafeHdc(), LOGPIXELSX);
		res.cy = GetDeviceCaps(pDC->GetSafeHdc(), LOGPIXELSY);
	}

	// Determine whether the Views aspect ratio is bigger or smaller than the printers one
	if (mRect.Width()*(rect.bottom-rect.top) > mRect.Height()*(rect.right-rect.left))
	{
		// View area is wider (in x) than Printer area
		nXRes = rect.right-rect.left;
		nYRes = nXRes*mRect.Height()/mRect.Width();

		// Reduce the Resolution if the Bitmap size is bigger than m_nMaxPrintMemSize*1024
		while (3*nXRes*nYRes > (/*pApp->m_nMaxPrintMemSize*/8*1024)*1024)
		{
			nXRes /= 2;
			nYRes /= 2;
			fac /= 2.;
		}
//		ScaleFont(double(nXRes)/double(mRect.Width()));
	}
	else
	{
		// Printer area is wider (in x) than View area
		nYRes = rect.bottom-rect.top;
		nXRes = nYRes*mRect.Width()/mRect.Height();

		// Reduce the Resolution if the Bitmap size is bigger than m_nMaxPrintMemSize*1024
		while (3*nXRes*nYRes > (/*pApp->m_nMaxPrintMemSize*/8*1024)*1024)
		{
			nXRes /= 2;
			nYRes /= 2;
			fac /= 2.;
		}
//		ScaleFont(double(nYRes)/double(mRect.Height()));
	}

	//nXRes = (nXRes + (sizeof(DWORD)-1)) & ~(sizeof(DWORD)-1);	// aligning width to 4 bytes (sizeof(DWORD)) avoids
	nXRes = nXRes & ~(sizeof(DWORD)-1);							// aligning width to 4 bytes (sizeof(DWORD)) avoids
																// pixel garbage at the upper line

	// First of all, initialize the bitmap header information...
	memset(&bitmapInfo, 0, sizeof(BITMAPINFO));
	bitmapInfo.bmiHeader.biSize		= sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biWidth		= nXRes;
	bitmapInfo.bmiHeader.biHeight		= nYRes;
	bitmapInfo.bmiHeader.biPlanes		= 1;
	bitmapInfo.bmiHeader.biBitCount		= 24;
	bitmapInfo.bmiHeader.biCompression	= BI_RGB;
	bitmapInfo.bmiHeader.biSizeImage	= bitmapInfo.bmiHeader.biWidth * bitmapInfo.bmiHeader.biHeight * 3;
	bitmapInfo.bmiHeader.biXPelsPerMeter = int(res.cx*fac/.0254);	// res.cx*fac DPI
	bitmapInfo.bmiHeader.biYPelsPerMeter = int(res.cy*fac/.0254);	// res.cy*fac DPI

	// create DIB
	hTmpDC = ::GetDC(m_hWnd);
	hDib = CreateDIBSection(hTmpDC, &bitmapInfo, DIB_RGB_COLORS, &pBitmapBits, NULL, (DWORD)0);
	::ReleaseDC(m_hWnd, hTmpDC);

	// create memory device context
    CDC* m_pDC = CClientDC::FromHandle(hDC);
	if ((hMemDC = CreateCompatibleDC(m_pDC == NULL ? NULL : m_pDC->GetSafeHdc())) == NULL)
	{
		DeleteObject(hDib);
		return;
	}
	HGDIOBJ hOldDib = SelectObject(hMemDC, hDib);

	// setup pixel format
	if (!SetMemDcPixelFormat(hMemDC) && !SetMemDcPixelFormat(hMemDC, TRUE))
	{
		if (hOldDib != NULL)
			SelectObject(hMemDC, hOldDib);
		DeleteObject(hDib);
		DeleteDC(hMemDC);
		return;
	}

	// create memory rendering context
	if ((hMemRC = wglCreateContext(hMemDC)) == NULL)
	{
		if (hOldDib != NULL)
			SelectObject(hMemDC, hOldDib);
		DeleteObject(hDib);
		DeleteDC(hMemDC);
		return;
	}

	// Store current rendering and device contexts
	GetCurrent(_hDC, _hRC);

	// Make this hMemRC the current OpenGL rendering context.
	SetCurrent(hMemDC, hMemRC);

//	SetOpenGLProperties();

	glViewport(0, 0, nXRes, nYRes);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDrawBuffer(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	// must be created once for hMemDC
//	CreateFontBitmaps();

	BOOL invC = isInvertBkColor;
	isInvertBkColor = TRUE;
	isPrinting = TRUE;
	   OnDraw(pDC);
	isPrinting = FALSE;
	isInvertBkColor = invC;

	glFinish();	// Finish all OpenGL commands

	// the rendering context will be no longer needed
	SetCurrent(NULL, NULL);
	wglDeleteContext(hMemRC);

	// Restore last rendering and device contexts
	if (_hDC != NULL && _hRC != NULL)
		SetCurrent(_hDC, _hRC);

	// Restore the Views original font size
//	UnScaleFont();
//	CreateFontBitmaps();

	// Get the size of our page and clear to White
	brush = CreateSolidBrush(0x00ffffff);		// White brush
	FillRect(pDC->GetSafeHdc(), &rect, brush);	// Clear the page

	PageSize.x = rect.right;
	PageSize.y = PageSize.x * bitmapInfo.bmiHeader.biHeight / bitmapInfo.bmiHeader.biWidth;
	if (PageSize.y > rect.bottom)
	{
		PageSize.y = rect.bottom;
		PageSize.x = PageSize.y * bitmapInfo.bmiHeader.biWidth / bitmapInfo.bmiHeader.biHeight;
	}
	ImageOffset.x = (rect.right - PageSize.x) / 2;
	ImageOffset.y = (rect.bottom - PageSize.y) / 2;

	// Does our device context support StretchDIBits?
	if (m_bStretchDIBits)
	{
		BOOL bRet = StretchDIBits(pDC->GetSafeHdc(),
								ImageOffset.x, ImageOffset.y,
								PageSize.x, PageSize.y,
								0, 0,
								bitmapInfo.bmiHeader.biWidth,
								bitmapInfo.bmiHeader.biHeight,
								(GLubyte*)pBitmapBits,
								&bitmapInfo,
								DIB_RGB_COLORS,
								SRCCOPY);
	}
	else if (m_bStretchBlt)
	{
		HDC hdc;
		HBITMAP bitmap;

		hdc	= CreateCompatibleDC(pDC->GetSafeHdc());
		bitmap = CreateDIBitmap(hdc,
								&(bitmapInfo.bmiHeader),
								CBM_INIT,
								(GLubyte*)pBitmapBits,
								&bitmapInfo,
								DIB_RGB_COLORS);

		HGDIOBJ oldbitmap = SelectObject(hdc, bitmap);
		BOOL bRet = StretchBlt(pDC->GetSafeHdc(),
								ImageOffset.x, ImageOffset.y,
								PageSize.x, PageSize.y,
								hdc,
								0, 0,
								bitmapInfo.bmiHeader.biWidth,
								bitmapInfo.bmiHeader.biHeight,
								SRCCOPY);
		if (oldbitmap != NULL)
			SelectObject(hdc, oldbitmap);

		// Free our bitmap and bitmap device context
		DeleteObject(bitmap);
		DeleteDC(hdc);
	}
		if (hOldDib != NULL)
			SelectObject(hMemDC, hOldDib);

	// Delete our brush, DIB and device context
	DeleteObject(brush);
	DeleteObject(hDib);
	DeleteDC(hMemDC);
}

void C3DGraph::SetBackColor(COLORREF _bkColor)
{
	bkColor = _bkColor;
	if(bkColor == RGB(255,255,255))
		isInvertBkColor = TRUE;
	else
		isInvertBkColor = FALSE;
}
