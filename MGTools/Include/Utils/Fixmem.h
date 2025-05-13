#if !defined(__FIXMem_H_)
#define __FIXMem_H_

#include <afxtempl.h>

template <class T>
class Mem : public CArray<T, T>
{
protected:
	bool bAutoDelete;

	int SizeX;     // –азмер по ’
	int SizeY;     // –азмер по Y

public:

	Mem() {SizeX=SizeY=0; bAutoDelete=TRUE;} // конструктор ()
	Mem(int x,int y) {Set(x,y); bAutoDelete=TRUE;} // двумерный массив
	Mem(int x) {Set(x); bAutoDelete=TRUE;} // одномерный массив
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

	void EnableAutoDelete(BOOL bDelete) // разрешение удаление элементов массива
		{bAutoDelete=bDelete;}
	BOOL IsAutoDelete() {return bAutoDelete;}

	// ”становить размер массива x*y элементов.
	BOOL Set(int x, int y=1) // установить размерность массива
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

	
	int ReSet(int newX, int newY=1) // переустановить размер массива
	{
		Set(newX, newY);
		return CArray<T, T>::GetSize();
	}

	void Init(T Vinit) // инициализаци€ массива
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

	int GetNx() {return SizeX;} // размерность по X
	int GetNy() {return SizeY;} // размерность по Y
	int GetSize() {return SizeX*SizeY;}	// обща€ длина массива
	int GetNrow() {return SizeX;}
	int GetNcol() {return SizeY;}
	T *GetpMem(){return GetData();}

	void Delete() // удаление всех элементов массива
	{
		if(bAutoDelete) {
			RemoveAll();
			SizeX=SizeY=0;
		}
	}

	int Add(T item=T())	// добавить элемент в массив
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
	int AddRow(T item=T()) // добавить строку к массиву
	{
		int Nx=GetNx();
		int Ny=GetNy();
		
		Set(Nx, Ny+1);
		
		int iShift=Ny*Nx;
		for(int x=0; x<Nx; x++) 
			SetAt(x+iShift, item);
		
		return Ny+1;
	}

	int AddCol(T item=T()) // добавить столбец
	{
		int Nx=GetNx();
		int Ny=GetNy();
		
		Set(Nx+1, Ny);
		
		for(int y=0; y<Ny; y++) 
			SetAt(Nx+y*SizeX, item);
		
		return Nx+1;
	}
	
	int InsertRow(int iBeforeRow, T item=T()) // вставить строку
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

	int InsertCol(int iBeforeCol, T item=T()) // вставить столбец
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

	int DeleteItem(int i, int iCount=1) // удалить элемент
	{
		RemoveAt(i, iCount);
		Set(CArray<T, T>::GetSize());
		
		return CArray<T, T>::GetSize();
	}

	int DeleteRow(int yRow) // удалить строку
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

	int DeleteCol(int xCol) // удалить столбец
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
