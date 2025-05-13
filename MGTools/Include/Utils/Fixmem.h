#if !defined(__FIXMem_H_)
#define __FIXMem_H_

#include <afxtempl.h>

template <class T>
class Mem : public CArray<T, T>
{
protected:
	bool bAutoDelete;

	int SizeX;     // ������ �� �
	int SizeY;     // ������ �� Y

public:

	Mem() {SizeX=SizeY=0; bAutoDelete=TRUE;} // ����������� ()
	Mem(int x,int y) {Set(x,y); bAutoDelete=TRUE;} // ��������� ������
	Mem(int x) {Set(x); bAutoDelete=TRUE;} // ���������� ������
	Mem(Mem<T> &M) { 
		SizeX=SizeY=0; 
		bAutoDelete=TRUE;
		operator=(M);
	}
	virtual ~Mem(){ Delete(); }

	int GetIndex(int x, int y=1) 
	{
		return x+y*SizeX;
	}

	void EnableAutoDelete(BOOL bDelete) // ���������� �������� ��������� �������
		{bAutoDelete=bDelete;}
	BOOL IsAutoDelete() {return bAutoDelete;}

	// ���������� ������ ������� x*y ���������.
	BOOL Set(int x, int y=1) // ���������� ����������� �������
	{
		if(x<0) x=0;
		if(y<0) y=0;

		SizeX=x; SizeY=y;

		SetSize(x*y);
		return TRUE;
	}
	
	void SetZero(int x, int y=1)
	{
		Set(x, y);
		for(int i=0; i<CArray<T, T>::GetSize(); i++) 
			SetAt(i, T());
	};

	
	int ReSet(int newX, int newY=1) // �������������� ������ �������
	{
		Set(newX, newY);
		return CArray<T, T>::GetSize();
	}

	void Init(T Vinit) // ������������� �������
	{
		for(int i=0; i<CArray<T, T>::GetSize(); i++) SetAt(i, Vinit);
	}

//	T& operator []( int nIndex ) { return ElementAt(nIndex); }
//	T operator []( int nIndex ) { return (T)GetData()[nIndex]; }

	T  &operator()(int x,int y)
	{
#ifdef MEM_CHECK_RANGE
		if(x<0 || x>=SizeX || y<0 || y>=SizeY) {
			char Buf[80];
			sprintf(Buf,"Mem -- out of range\n"
				"x=%d, SizeX=%d\n"
				"y=%d, SizeY=%d", x, SizeX, y, SizeY);
			::MessageBox(NULL,Buf, "Warning",MB_ICONSTOP|MB_OK);
		}
#endif
		return ElementAt(x+y*SizeX);
	}

	T  &operator()(int x)
	{
#ifdef MEM_CHECK_RANGE
		if(x>=SizeX*SizeY) {
			char Buf[80];
			sprintf(Buf,"Mem -- out of range\n"
				"x=%d, SizeX=%d", x, SizeX*SizeY);
			::MessageBox(NULL,Buf, "Warning",MB_ICONSTOP|MB_OK);
		}
#endif
		return ElementAt(x);
	};
	
	Mem<T>  &operator=(Mem<T> &M)
	{
		int N=M.GetSize();
		Set(M.GetNx(), M.GetNy());

		for(int i=0; i<N; i++)
			SetAt(i, M.GetAt(i));
		return *this;
	};

	int GetNx() {return SizeX;} // ����������� �� X
	int GetNy() {return SizeY;} // ����������� �� Y
	int GetSize() {return SizeX*SizeY;}	// ����� ����� �������
	int GetNrow() {return SizeX;}
	int GetNcol() {return SizeY;}
	T *GetpMem(){return GetData();}

	void Delete() // �������� ���� ��������� �������
	{
		if(bAutoDelete) {
			RemoveAll();
			SizeX=SizeY=0;
		}
	}

	int Add(T item=T())	// �������� ������� � ������
	{
		CArray<T, T>::Add(item);
		SizeX=CArray<T, T>::GetSize();
		SizeY=1;
		return SizeX;
	}

	void Insert( int nIndex, T newElement, int nCount = 1 ) {
		InsertAt(nIndex, newElement, nCount);
		SizeX+=nCount;
		SizeY=1;
	}

	void Insert( int nStartIndex, Mem* pNewArray ) {
		InsertAt(nStartIndex, pNewArray);
		SizeX+=pNewArray->GetSize();
		SizeY=1;
	}
	int AddRow(T item=T()) // �������� ������ � �������
	{
		int Nx=GetNx();
		int Ny=GetNy();
		
		Set(Nx, Ny+1);
		
		int iShift=Ny*Nx;
		for(int x=0; x<Nx; x++) 
			SetAt(x+iShift, item);
		
		return Ny+1;
	}

	int AddCol(T item=T()) // �������� �������
	{
		int Nx=GetNx();
		int Ny=GetNy();
		
		Set(Nx+1, Ny);
		
		for(int y=0; y<Ny; y++) 
			SetAt(Nx+y*SizeX, item);
		
		return Nx+1;
	}
	
	int InsertRow(int iBeforeRow, T item=T()) // �������� ������
	{
		int Nx=GetNx();
		int Ny=GetNy();
		
		if(iBeforeRow>=Ny) {
			return AddRow(item);
		}
		Set(Nx, Ny+1);
		
		for(int y=Ny; y>iBeforeRow; y--) {
			int y1=y-1;
			for(int x=0; x<Nx; x++)
				SetAt(x+y*SizeX, GetAt(x+y1*SizeX));
		}
		for(int x=0; x<Nx; x++) 
			SetAt(x+iBeforeRow*SizeX, item);
		
		return Ny+1;
	}

	int InsertCol(int iBeforeCol, T item=T()) // �������� �������
	{
		int Nx=GetNx();
		int Ny=GetNy();
		
		if(iBeforeCol>=Nx) {
			return AddCol(item);
		}
		Set(Nx+1, Ny);
		
		for(int x=Nx; x>iBeforeCol; x--) {
			int x1=x-1;
			for(int y=0; y<Ny; y++)
				SetAt(x+y*SizeX, GetAt(x1+y*SizeX));
		}
		for(int y=0; y<Ny; y++)
			SetAt(iBeforeCol+y*SizeX, item);
		
		return Nx+1;
	}

	int DeleteItem(int i, int iCount=1) // ������� �������
	{
		RemoveAt(i, iCount);
		Set(CArray<T, T>::GetSize());
		
		return CArray<T, T>::GetSize();
	}

	int DeleteRow(int yRow) // ������� ������
	{
		int Ny_after=GetNy()-1;
		int Nx=GetNx();
		for(int y=yRow; y<Ny_after; y++) {
			for(int x=0; x<Nx; x++) {
				SetAt(x+y*SizeX, GetAt(x+(y+1)*SizeX));
			}
		}
		Set(Nx, Ny_after);
		return Ny_after;
	}

	int DeleteCol(int xCol) // ������� �������
	{
		int Nx_after=GetNx()-1;
		int Ny=GetNy();
		for(int x=xCol; x<Nx_after; x++) {
			for(int y=0; y<Ny; y++) {
				SetAt(x+y*SizeX, GetAt(x+1+y*SizeX));
			}
		}
		Set(Nx_after, Ny);
		
		return Nx_after;
	}
};

typedef Mem<bool> MemBool;
typedef Mem<BOOL> MemBOOL;
typedef Mem<int> MemInt;
typedef Mem<float> MemFloat;
typedef Mem<double> MemDouble;
typedef Mem<CString> MemString;

#endif

///////////////////////////////////////////////////////////////////////
