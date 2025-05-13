#include <math.h>
#include "matin1.h"
//=====================================================================
/***********************************************************************************
 ФУНКЦИЯ:    Matin1

 НАЗНАЧЕНИЕ:
             Решение системы линейных уравнений
                           
 АРГУМЕНТЫ:
          На входе: 
             double *A - указатель на матрицу 
                int N1 - число строк в матрице
                int N2 - число столбцов свободных членов

          На выходе:
               Решение системы, сдержащееся в N2 последних строках матрицы A
           int NeError - признак отсутствия ошибки
                         NeError = 1 - нет ошибки
                         NeError = 0 - система несовместима
        double &Determ - детерминант системы

 ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ: нет                           
************************************************************************************/
void Matin1(double *A, int N1, int N2, int &NeError, double &Determ)
{
  int Dim, Emat, Pivcol, Pivc_L1, Pivc_L2, Lpiv, Icol, I3, I2, I1, Jcol, Main;
  int i;
  double Pivot, Swap;
  int *Index = new int[N1];
  for(int j=0; j < N1; j++) Index[j] = 0;
  double Deter = 1.;
  int N = N1;
  Emat = N + N2;
  Dim = N1;
  int Nmin1 = N-1;
// THE ROUTINE DOES ITS OWN EVALUATION FOR DOUBLE SUBSCRIPTING OF
// ARRAY A.
  Pivcol = 1 - Dim;
//     MAIN LOOP TO INVERT THE MATRIX
  for(Main = 1; Main < N+1; Main++) {
     Pivot = 0.;
     Pivcol = Pivcol + Dim;
// SEARCH FOR NEXT PIVOT IN COLUMN MAIN.
     Pivc_L1=Pivcol + Main - 1;
     Pivc_L2=Pivcol + Nmin1;
     for(i = Pivc_L1; i < Pivc_L2+1; i++) {
        if((fabs(A[i-1]) - fabs(Pivot)) > 0.) {
          Pivot = A[i-1];
          Lpiv = i;
        }
     }
        if(!Pivot) {
          NeError = 0;
          Determ = Deter;
          if(Index) delete[] Index;
          return;
        }
// IS PIVOT DIFFERENT FROM ZERO
// GET THE PIVOT-LINE INDICATOR AND SWAP LINES IF NECESSARY
     Icol = Lpiv - Pivcol + 1;
//     Icol = Lpiv - Pivcol;
     Index[Main-1] = Icol;
     if((Icol - Main) > 0.) {
//     COMPLEMENT THE DETERMINANT
       Deter = -Deter;
//     POINTER TO LINE PIVOT FOUND
       Icol = Icol - Dim;
//     POINTER TO EXACT PIVOT LINE
       I3 = Main - Dim;
       for(i=1; i < Emat+1; i++) {
          Icol = Icol + Dim;
          I3 = I3 + Dim;
          Swap = A[I3-1];
          A[I3-1] = A[Icol-1];
          A[Icol-1] = Swap;
       }
     }
// COMPUTE DETERMINANT
     Pivot = 1. / Pivot;
// TRANSFORM PIVOT COLUMN
     I3 = Pivcol + Nmin1;
     for(i = Pivcol; i < I3+1; i++) A[i-1] = -A[i-1] * Pivot;
     A[Pivc_L1-1] = Pivot;
// PIVOT ELEMENT TRANSFORMED
//
//     NOW CONVERT REST OF THE MATRIX
      I1=Main-Dim;
//     POINTER TO PIVOT LINE ELEMENTS
      Icol=1-Dim;
//     GENERAL COLUMN POINTER
      for(i = 1; i < Emat+1; i++) {
         Icol = Icol + Dim;
         I1 = I1 + Dim;
//     POINTERS MOVED
         if(i - Main) {
           Jcol = Icol + Nmin1;
           Swap = A[I1-1];
           I3 = Pivcol - 1;
           for(I2 = Icol; I2 < Jcol+1; I2++) {
              I3++;
              A[I2-1] = A[I2-1] + Swap*A[I3-1];
           }
           A[I1-1] = Swap * Pivot;
         }
      }
  }
//     NOW REARRANGE THE MATRIX TO GET RIGHT INVERS
  for(I1=1; I1 < N+1; I1++) {
     Main = N + 1 - I1;
     Lpiv = Index[Main-1];
     if(Lpiv - Main) {
       Icol = (Lpiv-1) * Dim+1;
       Jcol = Icol + Nmin1;
       Pivcol = Main  * Dim+1 - Icol;
       for(I2 = Icol-1; I2 < Jcol; I2++) {
          I3 = I2 + Pivcol;
          Swap = A[I2];
          A[I2] = A[I3];
          A[I3] = Swap;
       }
     }
  }
  Determ = Deter;
  NeError = 1;
  if(Index) delete[] Index;
  return;
}
//=====================================================================
