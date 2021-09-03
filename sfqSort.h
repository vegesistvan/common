#pragma once
// sfqSort.h: interface for the CsfqSort class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FQSORT_H__A99F6ACA_19A6_4BA5_BDC4_D2A6985349A2__INCLUDED_)
#define AFX_FQSORT_H__A99F6ACA_19A6_4BA5_BDC4_D2A6985349A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CsfqSort  
{
public:
	void Sort(int *iPointer,int nItems,void* pInfo,int (*cmpFuncion)(const void *,const void*,const void*));
	CsfqSort();
	virtual ~CsfqSort();

private:
	void Swap(int *i,int *j);
	//int (*compareFuncion)(const void*,const void *,const void *);
	int (*compareFuncion)(const void*,const void *,const void *);
	int buffer;
	//int *iP;
	void quicksort(int l, int r);

	void qusort(int lo, int hi);

	void MedianOfThreePartition(int p, int r);

	void QuickSort(int l, int r);
	void InsertionSort(int lo0, int hi0);

	void ShellSort(int xx, int n);


	


	void *infoP;
	int* A;

};

#endif // !defined(AFX_FQSORT_H__A99F6ACA_19A6_4BA5_BDC4_D2A6985349A2__INCLUDED_)
