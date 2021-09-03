#pragma once
// SqliteDB.h: interface for the CSqliteDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SQLITEDB_H__930379D8_BDD1_4973_93FF_041F3F3811E4__INCLUDED_)
#define AFX_SQLITEDB_H__930379D8_BDD1_4973_93FF_041F3F3811E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "sqlite3.h"

#define RSQUERY_SUCCESS			0
#define RSQUERY_NOLIBLOADED		1
#define RSQUERY_NOCONNECT		2
#define RSQUERY_PREPAREERROR	3
#define RSQUERY_INVALIDQRY		4
#define RSQUERY_FINALIZEERROR	5


#define EXTERNAL_LIB	//ha dll-kent akarjuk hasznalni az sqlite-ot akkor hasznaljuk ezt az opciot


typedef struct
{
	CString filename;
	CString fileSpec;
	CString ext;
	u_int64	size;
}BLOBSTAT;

//a kovetkezo opcioknak akkor van ertelme, ha kozvetlenul forditjuk az sqliteot az sqlitedb-be
//#define ENCRYPTION //ha megvannak a titkosítast tamogato kiegeszitesek, akkor engedelyezzuk ezt, es hasznaljuk a   SQLITE_ENABLE_RTREE;SQLITE_HAS_CODEC;SQLITE3ENCRYPT_EXPORTS;SQLITE_ENABLE_FTS3;SQLITE_ENABLE_FTS3_PARENTHESIS;SQLITE_SECURE_DELETE;SQLITE_SOUNDEX;CODEC_TYPE=CODEC_TYPE_AES128; preprocessor directivakat
//#define SQLITE_METADATA  //ha akarunk metadatat, akkor engedelyezzuk ezt az opciot, es hasznaljuk a SQLITE_ENABLE_COLUMN_METADATA preprocessor directivat
//kozvetlen sqlite forditasnal ne hasznaljunk precompiled headereket

////////////////////////////////////////
///   CSqliteDB CLASS
////////////////////////////////////////

typedef int (*AUTH_CALLBACK)(void*,int,const char*,const char*,const char*,const char*);//az authorizer callback fuggveny tipus deffinicioja
typedef void (*HOOK_CALLBACK) (void *,int ,char const *,char const *,INT64);//az adatbazis modosulasat jelzo callback fuggveny tipus deffinicioja

class CSqliteDB  
{
public:
	CSqliteDB();
	virtual ~CSqliteDB();

public:
	virtual int Execute(LPCTSTR strQuery);
	virtual int Close();
	bool Connect(LPCTSTR strDBPathName, LPCTSTR strDBFilePassword = NULL, int Flags=SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
	CString GetDbName();
	int IsConnected();
	bool IsLibLoaded();
	virtual int ChangePass(LPCTSTR strNewPass);
	LPCTSTR GetLastError();
	virtual CString GetLibversion();
	bool SetAuthorizer(AUTH_CALLBACK authFunct,void* parentObj=NULL,sqlite3* pDB=NULL);
	bool SetHookCallback(HOOK_CALLBACK alertFunct,void* parentObj=NULL,sqlite3* pDB=NULL);

	void*	readBlob( CString id, int table_id, int* blobLength );
	int		blobInsert( char* table, char* field, BLOBSTAT* stat );
//	int		blobInsert( CString table, CString column, BLOBSTAT* stat );

	void*	blobRead( char* table, char* column, CString rowid, _int64* size );
	_int64	lastInsertRowid();

public:
	sqlite3 *db;

private:
	bool InitLibrary();
	bool InitInstance();
	CString dbPathName;
	
private:
	bool m_bLibLoaded;

protected:
	int m_bConnected;  //1, ha van kapcsolat az adatbazissal, 2 ha van kapcsolat a szerverrel, 3 ha mindketto
	CString ErrMsg;
	bool m_encryption;
};

////////////////////////////////////////
///   CSqliteDBRecordSet CLASS
////////////////////////////////////////
/*
#ifndef FIELD_TYPE
#define FIELD_TYPE
typedef struct FIELD
{
	FIELD (TCHAR* x_, double y_) :fText(x_), fValue(y_) {}
	TCHAR* fText;
	double fValue;
} FIELD;
#endif
*/
class CSqliteDBRecordSet  
{
public:
	LONG GetColumnIndex(LPCTSTR strColName);
	bool IsFirst();
	void MoveTo(LONG nIndex);
	void MovePrevious();
	void MoveNext();
	void MoveLast();
	void MoveFirst();
	UINT RecordsCount();
	INT GetColumnType (LONG nIndex);
	LPCTSTR GetColumnName(LONG nIndex);
	UINT ColumnsCount();//az oszlopok szama
	LPCTSTR GetFieldString (LONG nIndex);//a currentrecord sorban a nindexedik elem szovege
	double GetFieldValue(LONG nIndex);// a currentrecord sorban a nindexedik elem erteke
	bool IsLast();//ha nincs tobb elem
	LPCTSTR GetLastError();//utolso hibauzenet
	virtual UINT Query(LPCTSTR strQueryString, void *ptrDB, int firstElement=0, int maxElement=INT_MAX);//lekerdezes
	bool Clear();//torli a lekerdezes adatait
	CUIntArray* _GetColTypeArray();
	CStringArray* _GetColListArray();
	std::vector<TCHAR*>* _GetResTableArray();
	

	CSqliteDBRecordSet();//constructor
	virtual ~CSqliteDBRecordSet();//destructor

private:
	bool InitInstance();
	LONG m_nCurrentRecord;//a pillanatnyi sor

protected:
	LONG m_nRecordsCount;// a sorok szama
	LONG m_nColumnsCount;//az oszlopok szama
	CString ErrMsg;//hibauzenetek

protected:


	std::vector<TCHAR*> orsTable; //az eredmenytabla
	CUIntArray nColumnsType;//az oszlopok tipusai
	CStringArray strColumnsList;// az oszlopok nevei
	bool m_bIsEmpty;//ha ures a tabla, se oszlop, se sor nincs benne
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  GLOBAL FUNCTIONS  /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// unicode, multibyte stb.
//////////////////////////////////////////////////////////////////////
CStringW MultiToUnicodeS(LPCSTR multi);
CStringW Utf8ToUnicodeS(LPCSTR utf8);
CStringA UnicodeToUtf8S(LPCWSTR unicode);
CStringA UnicodeToMultiS(LPCWSTR unicode);

#endif // !defined(AFX_SQLITEDB_H__930379D8_BDD1_4973_93FF_041F3F3811E4__INCLUDED_)
