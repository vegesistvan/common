// sfqSort.cpp: implementation of the sfqSort class.
//
//////////////////////////////////////////////////////////////////////

/*
Pelda az osztaly hasznalatara:(kompatibilis a qsort fuggvennyel)
#include "sfqSort.h"  //csatoljuk a programhoz az osztalyt

LPCTSTR* tomb=NULL;//letrehozunk egy ket elembol allo tombot. ezt fogjuk sorbarakni
tomb=new LPCTSTR[2];
tomb[0]="maci"; 
tomb[1]="bela"; 

CsfqSort valtozo; //peldanyositjuk az osztalyt.
valtozo.Sort(tomb,2,sizeof(LPCTSTR), cmpr); //ez rendezi a tombot novekvo sorrendbe
//parameterek: a tomb, elemszam,valtozo merete, osszehasonlito fuggveny

//az osszehasonlito fuggvenyt is meg kell irni a programunkban.
//fontos, hogy a fuggvenyt statikusan deklaraljuk a header fileban
//a fuggvenynek 2 parametere van, az elso es a masodik elem memoriacime.
//ez alapjan kell nekunk meghatarozni, hogy melyik a nagyobb.(a tipusat mi ugyis tudjuk)
//ha egyenloek, a visszateresi ertek 0
//ha az elso nagyobb, mint a masodik, a visszateresi ertek pozitiv,
//ha a masodik nagyobb, mint az elso, a visszateresi ertek negativ.

//pl:
static int CSqliteGridDlg::cmpr (const void * a, const void * b)
{
	//return ( strcmp(*(LPCTSTR*)a,*(LPCTSTR*)b));//sztringek eseten
	return ( *(int*)a - *(int*)b );//egesz szamok eseten

	//ha a>b ,az eredmeny: +
	//ha a<b ,az eredmeny: -
	//ha a=b ,az eredmeny: 0
}





*/



#include "stdafx.h"
#include "sfqSort.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CsfqSort::CsfqSort()
{
}

CsfqSort::~CsfqSort()
{
}

void CsfqSort::Sort(int *iPointer, int nItems,void* pInfo, int (*cmpFuncion)(const void *,const void *,const void *))
{
	infoP=pInfo;
	A=iPointer;
	compareFuncion=cmpFuncion;

	//////   ****   algoritmusok    ****** //////////
	//quicksort(0,nItems-1);//2. gyors, rendezett tombnel nagyon lassu

	qusort(0,nItems-1);//1. quicksort 3way 3median
	InsertionSort(0,nItems-1);//ezt meg kell hivni a qusort utan;

	//MedianOfThreePartition(0,nItems);//4
	
	//QuickSort(0,nItems-1);//3. regi altalam hasznalt. egesz gyors. az insertationt meg kell hivni utanna
	//InsertionSort(0,nItems-1);//ezt meg kell hivni a QuickSort utan;
	
	//ShellSort(0,nItems);//5. rendezettnel gyors

}

void CsfqSort::Swap(int *i, int *j)
{
	buffer=*i;
	*i=*j;
	*j=buffer;
}

////////////////////////////////////////////////////////////////////////////////////////
void CsfqSort::quicksort(int l, int r)//3 way fast, but slow at ordered array (http://www.sorting-algorithms.com/static/QuicksortIsOptimal.pdf)
{
	int i = l-1, j = r, p = l-1, q = r;
	if (r <= l)
		return;
	int v=A[r];
	//memcpy(v,&a[r*vS],vS);//Item v = a[r];
	for (;;)
	{
		while (compareFuncion(infoP,&A[++i],&v)<0) ;//(a[++i] < v) ;
		while (compareFuncion(infoP,&A[--j],&v)>0)  //(v < a[--j])
			if (j == l)
				break;

		if (i >= j)
			break;
		Swap(&A[i], &A[j]);
		if (compareFuncion(infoP,&A[i],&v)==0)//(a[i] == v)
		{
			p++;
			Swap(&A[p], &A[i]);
		}
		
		if (compareFuncion(infoP,&A[j],&v)==0)//(v == a[j])
		{
			q--;
			Swap(&A[j], &A[q]);
		}
	}
	Swap(&A[i], &A[r]);
	j = i-1;
	i = i+1;
	for (int k = l; k < p; k++, j--)
		Swap(&A[k], &A[j]);
	for (int k = r-1; k > q; k--, i++)
		Swap(&A[i], &A[k]);
	quicksort(l, j);
	quicksort(i, r);
}

/*
void CsfqSort::quicksort(char *a, int l, int r)
{
	int i = l-1, j = r, p = l-1, q = r;
	if (r <= l)
		return;

	memcpy(v,&a[r*vS],vS);//Item v = a[r];
	for (;;)
	{
		while (compareFuncion(&a[vS*(++i)],v)<0) ;//       (a[++i] < v) ;
		while (compareFuncion(&a[(--j)*vS],v)>0)  //(v < a[--j])
			if (j == l)
				break;

		if (i >= j)
			break;
		Swap(&a[i*vS], &a[j*vS]);
		if (compareFuncion(&a[i*vS],v)==0)//(a[i] == v)
		{
			p++;
			Swap(&a[p*vS], &a[i*vS]);
		}
		
		if (compareFuncion(&a[j*vS],v)==0)//(v == a[j])
		{
			q--;
			Swap(&a[j*vS], &a[q*vS]);
		}
	}
	Swap(&a[i*vS], &a[r*vS]);
	j = i-1;
	i = i+1;
	for (int k = l; k < p; k++, j--)
		Swap(&a[k*vS], &a[j*vS]);
	for (int k = r-1; k > q; k--, i++)
		Swap(&a[i*vS], &a[k*vS]);
	quicksort(a, l, j);
	quicksort(a, i, r);
}*/
////////////////////////////////////////////////////////////////////////////////////////
void CsfqSort::qusort(int lo, int hi)//simlier 3 way fast
{
	if((hi-lo)<8)
/*	{
		InsertionSort(lo,hi);
		return;
	}*/
	//if(hi<=lo)
		return;
	
	int lt = lo, gt = hi;
	int v = A[lo];
	//int i = lo;


int i = (hi+lo)/2;
if (compareFuncion(infoP,&A[lo],&A[i])>0) Swap(&A[lo],&A[i]);     // Tri-Median Methode!
if (compareFuncion(infoP,&A[lo],&A[hi])>0) Swap(&A[lo],&A[hi]);
if (compareFuncion(infoP,&A[i],&A[hi])>0) Swap(&A[i],&A[hi]);

gt = hi-1;
Swap(&A[i],&A[gt]);
i = lo;
v=A[gt];






	while (i <= gt)
	{
		int cmp = compareFuncion(infoP,&A[i],&v);
		if (cmp < 0)
			Swap(&A[lt++], &A[i++]);
			//exch(a, lt++, i++);
		else if (cmp > 0)
			Swap(&A[i], &A[gt--]);
			//exch(a, i, gt--);
		else i++;
	}
	qusort(lo, lt - 1);
	qusort(gt + 1, hi);
}
////////////////////////////////////////////////////////////////////////////////////////
void CsfqSort::MedianOfThreePartition(int p, int r)//tomb, elso rendezendo elem, rendezendo elemek szama
{
	if(r-p<2) return;

	int x=A[p];
	int y=A[(r-p)/2+p];
	int z=A[r-1];

	//char* x=&a[p*vS],y=&a[((r-p)/2+p)*vS],z=&a[(r-1)*vS]
	int i=p-1,j=r;//x:az elso elem, y:a rendezendo resz kozepso eleme, z:utolso elem, i: elso elem elotti hely, j: utolso elem utani hely

	int cxy=compareFuncion(infoP,&x,&y);//+ ha x>y   - ha x<y
	int cyz=compareFuncion(infoP,&y,&z);//+ ha y>z   - ha y<z
	int cxz=compareFuncion(infoP,&x,&z);//+ ha x>z   - ha x<z

	if((cxy<0 && cyz<0) || (cyz>0 && cxy>0))
	//if (y>x && y<z || y>z && y<x )//ha a kozepso elem nagyobb, mint az elso elem es kisebb, mint az utolso VAGY a kozepso elem nagyobb, mint az utolso es kisebb, mint az elso
		//x=y;//akkor az elso elem legyen a kozepsovel egyenlo
		x=y;
	else if ((cxz<0 && cyz>0) || (cyz<0 && cxz>0))
	//else if (z>x && z<y || z>y && z<x )//kulonben ha az utolso elem nagyobb mint az elso es kisebb mint a kozepso VAGY az utolso nagyobb, mint a kozepso es kisebb, mint az elso
		//x=z;//elso legyen egyenlo az utolsoval
		x=z;
	
	do
	{
		while (compareFuncion(infoP,&A[--j],&x)>0);
		while (compareFuncion(infoP,&A[++i],&x)<0);
	
		if  (i < j)//ha az utolso hely mutatoja kisebb, mint az elsoje
			Swap(&A[i],&A[j]);//akkor felcsereljuk a ket elem tartalmat

	} while (i < j);

	MedianOfThreePartition(p,j+1);
	MedianOfThreePartition(j+1,r);
}
////////////////////////////////////////////////////////////////////////////////////////
void CsfqSort::QuickSort(int l, int r)//old one
{
        int M = 4;
        int i,j,v;
        
        if ((r-l)>M)
        {
                i = (r+l)/2;
                if (compareFuncion(infoP,&A[l],&A[i])>0) Swap(&A[l],&A[i]);     // Tri-Median Methode!
                if (compareFuncion(infoP,&A[l],&A[r])>0) Swap(&A[l],&A[r]);
                if (compareFuncion(infoP,&A[i],&A[r])>0) Swap(&A[i],&A[r]);

                j = r-1;
                Swap(&A[i],&A[j]);
                i = l;
				v=A[j];
                for(;;)
                {
						while(compareFuncion(infoP,&A[++i],&v)<0);
						while(compareFuncion(infoP,&A[--j],&v)>0);

                        if (j<i) break;
                        Swap (&A[i],&A[j]);
                }
                Swap(&A[i],&A[r-1]);
                QuickSort(l,j);
                QuickSort(i+1,r);
        }
}

void CsfqSort::InsertionSort(int lo0, int hi0)
{
	int i,j,v;

	for (i=lo0+1;i<=hi0;i++)
	{
		v=A[i];
		j=i;
		while ((j>lo0) && (compareFuncion(infoP,&A[j-1],&v)>0))
		{
			A[j]=A[j-1];
			j--;
		}
		A[j]=v;
	}
}

////////////////////////////////////////////////////////////////////////////////////////
void CsfqSort::ShellSort(int xx, int n)
{
    int h, i, j, k;
    for (h = n; h /= 2;)
	{
        for (i = h; i < n; i++)
		{
            k = A[i];
            for (j = i; j >= h &&  (compareFuncion(infoP,&k ,&A[j - h])<0)  ; j -= h)
			{
                A[j] = A[j - h];
            }
            A[j] = k;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////
