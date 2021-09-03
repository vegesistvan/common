// SqliteDB.cpp: implementation of the CSqliteDB class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SqliteDB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef void			(* SQLITEDB_UPDATE_HOOK)			(sqlite3*, HOOK_CALLBACK, void*);
typedef INT				(* SQLITEDB_SET_AUTHORIZER)			(sqlite3*, AUTH_CALLBACK, void*);
typedef const char *	(* SQLITEDB_LIBVERSION)				(void);
typedef INT				(* SQLITEDB_OPEN_V2)				(const char *, sqlite3 **, int, const char *);
typedef INT				(* SQLITEDB_OPEN)					(LPCTSTR, sqlite3 **);
typedef INT				(* SQLITEDB_EXEC)					(sqlite3*, const char *, sqlite3_callback, void *, char **);
typedef INT				(* SQLITEDB_CLOSE)					(sqlite3*);
typedef INT				(* SQLITEDB_COLUMN_COUNT)			(sqlite3_stmt*);
typedef INT				(* SQLITEDB_PREPARE)				(sqlite3*, LPCTSTR, int, sqlite3_stmt**, LPCTSTR*);
typedef INT				(* SQLITEDB_FINALIZE)				(sqlite3_stmt*);
typedef LPCTSTR			(* SQLITEDB_COLUMN_NAME)			(sqlite3_stmt*, int iCol);
typedef INT				(* SQLITEDB_COLUMN_TYPE)			(sqlite3_stmt*, int iCol);
typedef INT				(* SQLITEDB_STEP)					(sqlite3_stmt*);
typedef INT				(* SQLITEDB_COLUMN_INT)				(sqlite3_stmt*, int iCol);//nem hasznaljuk. 64 bitest hasznalunk mindig
typedef __int64			(* SQLITEDB_COLUMN_INT64)			(sqlite3_stmt*, int iCol);
typedef LPCTSTR			(* SQLITEDB_COLUMN_TEXT)			(sqlite3_stmt*, int iCol);
typedef CONST VOID *	(* SQLITEDB_COLUMN_BLOB)			(sqlite3_stmt*, int iCol);
typedef DOUBLE			(* SQLITEDB_COLUMN_DOUBLE)			(sqlite3_stmt*, int iCol);
typedef INT				(* SQLITEDB_KEY)					(sqlite3* , const void *, int);
typedef INT				(* SQLITEDB_REKEY)					(sqlite3* , const void *, int);
typedef LPCTSTR			(* SQLITEDB_ERRMSG)					(sqlite3 *);
typedef LPCTSTR			(* SQLITEDB_COLUMN_DATABASE_NAME)	(sqlite3_stmt*, int iCol);
typedef LPCTSTR			(* SQLITEDB_COLUMN_TABLE_NAME)		(sqlite3_stmt*, int iCol);
typedef LPCTSTR			(* SQLITEDB_COLUMN_ORIGIN_NAME)		(sqlite3_stmt*, int iCol); 
typedef INT				(* SQLITEDB_BIND_ZEROBLOB)			(sqlite3_stmt*, int, int );
typedef __int64			(* SQLITEDB_LAST_INSERT_ROWID)		(sqlite3* );
typedef INT				(* SQLITEDB_BLOB_OPEN )				(sqlite3*, const char*, const char*, const char*, sqlite3_int64, int, sqlite3_blob ** );
typedef INT				(* SQLITEDB_BLOB_WRITE )			(sqlite3_blob*, const void*, int, int ); 
typedef INT				(* SQLITEDB_BLOB_CLOSE )			(sqlite3_blob* );
typedef INT				(* SQLITEDB_BLOB_BYTES)				(sqlite3_blob* ); 
typedef INT				(* SQLITEDB_BLOB_READ)				(sqlite3_blob*, void*, int, int ); 


#ifdef EXTERNAL_LIB

HINSTANCE hDLL;	// a dll betolteshez kell

SQLITEDB_UPDATE_HOOK			SqliteDBUpdateHook;
SQLITEDB_SET_AUTHORIZER			SqliteDBSetAuthorizer;
SQLITEDB_LIBVERSION				SqliteDBLibversion;
SQLITEDB_OPEN					SqliteDBOpen;
SQLITEDB_OPEN_V2				SqliteDBOpenV2;
SQLITEDB_EXEC					SqliteDBExecute;
SQLITEDB_CLOSE					SqliteDBClose;
SQLITEDB_COLUMN_COUNT			SqliteDBColumnCount;    // Function pointer
SQLITEDB_PREPARE				SqliteDBPrepare;
SQLITEDB_FINALIZE				SqliteDBFinalize;
SQLITEDB_COLUMN_NAME			SqliteDBColumnName;
SQLITEDB_COLUMN_TYPE			SqliteDBColumnType;
SQLITEDB_STEP					SqliteDBStep;
SQLITEDB_COLUMN_INT				SqliteDBColumnInt;//nem hasznaljuk. 64 bitest hasznalunk mindig
SQLITEDB_COLUMN_INT64			SqliteDBColumnInt64;
SQLITEDB_COLUMN_TEXT			SqliteDBColumnText;
SQLITEDB_COLUMN_BLOB			SqliteDBColumnBlob;
SQLITEDB_COLUMN_DOUBLE			SqliteDBColumnDouble;
SQLITEDB_KEY					SqliteDBKey;
SQLITEDB_REKEY					SqliteDBRekey;
SQLITEDB_ERRMSG					SqliteDBErrmsg;
SQLITEDB_COLUMN_DATABASE_NAME	SqliteDBColumnDatabaseName;
SQLITEDB_COLUMN_TABLE_NAME		SqliteDBColumnTableName;
SQLITEDB_COLUMN_ORIGIN_NAME		SqliteDBColumnOriginName;
SQLITEDB_BIND_ZEROBLOB			SqliteDBBindZeroblob;
SQLITEDB_LAST_INSERT_ROWID		SqliteDBLastInsertRowid;
SQLITEDB_BLOB_OPEN				SqliteDBBlobOpen;
SQLITEDB_BLOB_WRITE				SqliteDBBlobWrite;
SQLITEDB_BLOB_CLOSE				SqliteDBBlobClose;
SQLITEDB_BLOB_BYTES				SqliteDBBlobBytes;
SQLITEDB_BLOB_READ				SqliteDBBlobRead;

#else

SQLITEDB_UPDATE_HOOK			SqliteDBUpdateHook=(SQLITEDB_UPDATE_HOOK) &sqlite3_update_hook;
SQLITEDB_SET_AUTHORIZER			SqliteDBSetAuthorizer=(SQLITEDB_SET_AUTHORIZER) &sqlite3_set_authorizer;
SQLITEDB_LIBVERSION				SqliteDBLibversion=(SQLITEDB_LIBVERSION) &sqlite3_libversion;
SQLITEDB_OPEN_V2				SqliteDBOpenV2=(SQLITEDB_OPEN_V2) &sqlite3_open_v2;
SQLITEDB_EXEC					SqliteDBExecute=(SQLITEDB_EXEC) &sqlite3_exec;
SQLITEDB_CLOSE					SqliteDBClose=(SQLITEDB_CLOSE ) &sqlite3_close;
SQLITEDB_COLUMN_COUNT			SqliteDBColumnCount=(SQLITEDB_COLUMN_COUNT) &sqlite3_column_count;    // Function pointer
SQLITEDB_FINALIZE				SqliteDBFinalize=(SQLITEDB_FINALIZE) &sqlite3_finalize;
SQLITEDB_COLUMN_TYPE			SqliteDBColumnType=(SQLITEDB_COLUMN_TYPE) &sqlite3_column_type;
SQLITEDB_STEP					SqliteDBStep=(SQLITEDB_STEP) &sqlite3_step;
SQLITEDB_COLUMN_INT				SqliteDBColumnInt=(SQLITEDB_COLUMN_INT) &sqlite3_column_int;//nem hasznaljuk. 64 bitest hasznalunk mindig
SQLITEDB_COLUMN_INT64			SqliteDBColumnInt64=(SQLITEDB_COLUMN_INT64) &sqlite3_column_int64;
SQLITEDB_COLUMN_BLOB			SqliteDBColumnBlob=(SQLITEDB_COLUMN_BLOB) &sqlite3_column_blob;
SQLITEDB_COLUMN_DOUBLE			SqliteDBColumnDouble=(SQLITEDB_COLUMN_DOUBLE) &sqlite3_column_double;

#if defined(_UNICODE) || defined(UNICODE)
	SQLITEDB_OPEN					SqliteDBOpen=(SQLITEDB_OPEN) &sqlite3_open16;
	SQLITEDB_PREPARE				SqliteDBPrepare=(SQLITEDB_PREPARE) &sqlite3_prepare16;
	SQLITEDB_COLUMN_NAME			SqliteDBColumnName=(SQLITEDB_COLUMN_NAME) &sqlite3_column_name16;
	SQLITEDB_ERRMSG					SqliteDBErrmsg=(SQLITEDB_ERRMSG) &sqlite3_errmsg16;
	SQLITEDB_COLUMN_TEXT			SqliteDBColumnText=(SQLITEDB_COLUMN_TEXT) &sqlite3_column_text16;
	#ifdef SQLITE_METADATA
		SQLITEDB_COLUMN_DATABASE_NAME	SqliteDBColumnDatabaseName=(SQLITEDB_COLUMN_DATABASE_NAME) &sqlite3_column_database_name16;
		SQLITEDB_COLUMN_TABLE_NAME		SqliteDBColumnTableName=(SQLITEDB_COLUMN_TABLE_NAME) &sqlite3_column_table_name16;
		SQLITEDB_COLUMN_ORIGIN_NAME		SqliteDBColumnOriginName=(SQLITEDB_COLUMN_ORIGIN_NAME) &sqlite3_column_origin_name16;
	#endif
#else
	SQLITEDB_OPEN					SqliteDBOpen=(SQLITEDB_OPEN) &sqlite3_open;
	SQLITEDB_PREPARE				SqliteDBPrepare=(SQLITEDB_PREPARE) &sqlite3_prepare;
	SQLITEDB_COLUMN_NAME			SqliteDBColumnName=(SQLITEDB_COLUMN_NAME) &sqlite3_column_name;
	SQLITEDB_ERRMSG					SqliteDBErrmsg=(SQLITEDB_ERRMSG) &sqlite3_errmsg;
	SQLITEDB_COLUMN_TEXT			SqliteDBColumnText=(SQLITEDB_COLUMN_TEXT) &sqlite3_column_text;
	#ifdef SQLITE_METADATA
		SQLITEDB_COLUMN_DATABASE_NAME	SqliteDBColumnDatabaseName=(SQLITEDB_COLUMN_DATABASE_NAME) &sqlite3_column_database_name;
		SQLITEDB_COLUMN_TABLE_NAME		SqliteDBColumnTableName=(SQLITEDB_COLUMN_TABLE_NAME) &sqlite3_column_table_name;
		SQLITEDB_COLUMN_ORIGIN_NAME		SqliteDBColumnOriginName=(SQLITEDB_COLUMN_ORIGIN_NAME) &sqlite3_column_origin_name;
	#endif
#endif

#ifdef ENCRYPTION
	SQLITEDB_KEY					SqliteDBKey=(SQLITEDB_KEY) &sqlite3_key;
	SQLITEDB_REKEY					SqliteDBRekey=(SQLITEDB_REKEY) &sqlite3_rekey;
#else
	SQLITEDB_KEY					SqliteDBKey=NULL;
	SQLITEDB_REKEY					SqliteDBRekey=NULL;
#endif

#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSqliteDB::CSqliteDB()
{
	InitInstance();
}

CSqliteDB::~CSqliteDB()
{
#ifdef EXTERNAL_LIB
	if (m_bLibLoaded)
	{
		FreeLibrary(hDLL);
		m_bLibLoaded = false;
	}
#endif

	m_bConnected = 0;
}

bool CSqliteDB::InitInstance()// konstruktor hivja meg. Alapbeallitasok
{
	ErrMsg=_T("");
	m_bConnected = 0;
	m_bLibLoaded = true;
#ifdef EXTERNAL_LIB
	m_bLibLoaded = false;
	m_bLibLoaded = InitLibrary();
#endif

	if(!SqliteDBKey || !SqliteDBRekey)
		m_encryption = false;
	else
		m_encryption = true;
	
	return false;
}

bool CSqliteDB::IsLibLoaded()// true, ha a dll sikeresen betoltodott
{
	return m_bLibLoaded;
}

bool CSqliteDB::InitLibrary()// az sqlitedll betoltese amennyiben szukseg van ra
{
#ifdef EXTERNAL_LIB

	#ifdef _M_X64 
	hDLL = LoadLibrary( L"sqlite64");
	#else 
	hDLL = LoadLibrary( L"sqlite32");
	#endif

//	hDLL = LoadLibrary(_T("sqlitedll"));

	if (hDLL)
	{
		SqliteDBUpdateHook		= (SQLITEDB_UPDATE_HOOK)GetProcAddress(hDLL, "sqlite3_update_hook");
		SqliteDBSetAuthorizer	= (SQLITEDB_SET_AUTHORIZER)GetProcAddress(hDLL, "sqlite3_set_authorizer");
		SqliteDBLibversion		= (SQLITEDB_LIBVERSION)GetProcAddress(hDLL, "sqlite3_libversion");
		SqliteDBOpenV2			= (SQLITEDB_OPEN_V2)GetProcAddress(hDLL, "sqlite3_open_v2");
		SqliteDBExecute			= (SQLITEDB_EXEC)GetProcAddress(hDLL, "sqlite3_exec");
		SqliteDBClose			= (SQLITEDB_CLOSE)GetProcAddress(hDLL, "sqlite3_close");
		SqliteDBColumnCount		= (SQLITEDB_COLUMN_COUNT)GetProcAddress(hDLL, "sqlite3_column_count");
		SqliteDBFinalize		= (SQLITEDB_FINALIZE)GetProcAddress(hDLL, "sqlite3_finalize");
		SqliteDBColumnType		= (SQLITEDB_COLUMN_TYPE)GetProcAddress(hDLL, "sqlite3_column_type");
		SqliteDBStep			= (SQLITEDB_STEP)GetProcAddress(hDLL, "sqlite3_step");
		SqliteDBColumnInt		= (SQLITEDB_COLUMN_INT)GetProcAddress(hDLL, "sqlite3_column_int");
		SqliteDBColumnInt64		= (SQLITEDB_COLUMN_INT64)GetProcAddress(hDLL, "sqlite3_column_int64");
		SqliteDBColumnBlob		= (SQLITEDB_COLUMN_BLOB)GetProcAddress(hDLL, "sqlite3_column_blob");
		SqliteDBColumnDouble	= (SQLITEDB_COLUMN_DOUBLE)GetProcAddress(hDLL, "sqlite3_column_double");
		SqliteDBBindZeroblob	= (SQLITEDB_BIND_ZEROBLOB)GetProcAddress( hDLL, "sqlite3_bind_zeroblob" ); 
		SqliteDBLastInsertRowid = (SQLITEDB_LAST_INSERT_ROWID)GetProcAddress( hDLL, "sqlite3_last_insert_rowid" );
		SqliteDBBlobOpen		= (SQLITEDB_BLOB_OPEN)GetProcAddress( hDLL, "sqlite3_blob_open" );
		SqliteDBBlobWrite		= (SQLITEDB_BLOB_WRITE)GetProcAddress( hDLL, "sqlite3_blob_write" );
		SqliteDBBlobClose		= (SQLITEDB_BLOB_CLOSE)GetProcAddress( hDLL, "sqlite3_blob_close" );
		SqliteDBBlobBytes		= (SQLITEDB_BLOB_BYTES)GetProcAddress( hDLL, "sqlite3_blob_bytes" );
		SqliteDBBlobRead		= (SQLITEDB_BLOB_READ)GetProcAddress( hDLL, "sqlite3_blob_read" );


#if defined(_UNICODE) || defined(UNICODE)
		SqliteDBOpen = (SQLITEDB_OPEN)GetProcAddress(hDLL, "sqlite3_open16");
		SqliteDBPrepare = (SQLITEDB_PREPARE)GetProcAddress(hDLL, "sqlite3_prepare16");
		SqliteDBColumnName = (SQLITEDB_COLUMN_NAME)GetProcAddress(hDLL, "sqlite3_column_name16");
		SqliteDBErrmsg = (SQLITEDB_ERRMSG)GetProcAddress(hDLL, "sqlite3_errmsg16");
		SqliteDBColumnText = (SQLITEDB_COLUMN_TEXT)GetProcAddress(hDLL, "sqlite3_column_text16");
		SqliteDBColumnDatabaseName = (SQLITEDB_COLUMN_DATABASE_NAME)GetProcAddress(hDLL, "sqlite3_column_database_name16");
		SqliteDBColumnTableName = (SQLITEDB_COLUMN_TABLE_NAME)GetProcAddress(hDLL, "sqlite3_column_table_name16");
		SqliteDBColumnOriginName = (SQLITEDB_COLUMN_ORIGIN_NAME)GetProcAddress(hDLL, "sqlite3_column_origin_name16");
#else
		SqliteDBOpen = (SQLITEDB_OPEN)GetProcAddress(hDLL, "sqlite3_open");
		SqliteDBPrepare = (SQLITEDB_PREPARE)GetProcAddress(hDLL, "sqlite3_prepare");
		SqliteDBColumnName = (SQLITEDB_COLUMN_NAME)GetProcAddress(hDLL, "sqlite3_column_name");
		SqliteDBErrmsg = (SQLITEDB_ERRMSG)GetProcAddress(hDLL, "sqlite3_errmsg");
		SqliteDBColumnText = (SQLITEDB_COLUMN_TEXT)GetProcAddress(hDLL, "sqlite3_column_text");
		SqliteDBColumnDatabaseName = (SQLITEDB_COLUMN_DATABASE_NAME)GetProcAddress(hDLL, "sqlite3_column_database_name");
		SqliteDBColumnTableName = (SQLITEDB_COLUMN_TABLE_NAME)GetProcAddress(hDLL, "sqlite3_column_table_name");
		SqliteDBColumnOriginName = (SQLITEDB_COLUMN_ORIGIN_NAME)GetProcAddress(hDLL, "sqlite3_column_origin_name");
#endif
		
		SqliteDBKey = (SQLITEDB_KEY)GetProcAddress(hDLL, "sqlite3_key");
		SqliteDBRekey = (SQLITEDB_REKEY)GetProcAddress(hDLL, "sqlite3_rekey");

		if (!SqliteDBOpen || !SqliteDBExecute || !SqliteDBSetAuthorizer || !SqliteDBUpdateHook
				|| !SqliteDBLibversion	|| !SqliteDBOpenV2
				|| !SqliteDBClose		|| !SqliteDBColumnCount
				|| !SqliteDBPrepare		|| !SqliteDBFinalize
				|| !SqliteDBColumnName	|| !SqliteDBColumnType
				|| !SqliteDBStep		|| !SqliteDBColumnInt
				|| !SqliteDBColumnText	|| !SqliteDBColumnBlob
				|| !SqliteDBColumnDouble|| !SqliteDBErrmsg )
			
			m_bLibLoaded = false;
		else
			m_bLibLoaded = true;
	}

	if (m_bLibLoaded == false)
		FreeLibrary (hDLL);

#endif
	return m_bLibLoaded;

}

int CSqliteDB::IsConnected()//true, ha csatlakozva vagyunk valamelyik adatbazishoz
{
	return m_bConnected;
}

CString CSqliteDB::GetDbName()
{
	return dbPathName;
}

bool CSqliteDB::Connect(LPCTSTR strDBPathName, LPCTSTR strDBFilePassword, int Flags)//adatbazis megnyitasa (csatlakozas)
{
	if (m_bLibLoaded == false)
	{
		ErrMsg=_T("Can't load sqlite library");
		return true;
	}

	CString dbName=strDBPathName;
	CString dbFilePass=strDBFilePassword;

#if !defined (_UNICODE) && !defined (UNICODE)
	dbName=UnicodeToUtf8SMultiToUnicode(strDBPathName));
#endif

	INT result;
	if(Flags==6)//default flags az open-hez (readwritecreate)
		result=SqliteDBOpen(dbName, &db);	// If a non zero is returned, some problem occured
	else
	{
		CStringA strUtf8;
#if defined (_UNICODE) || defined (UNICODE)
		strUtf8=UnicodeToUtf8S(strDBPathName);// az openV2 csak utf8-al mukodik
#else
		strUtf8=dbName;
#endif		
		result=SqliteDBOpenV2(strUtf8, &db, Flags,0);
	}

	if (result)
	{
		ErrMsg=SqliteDBErrmsg(db);
		SqliteDBClose(db);
		return true;
	}
	
	if (dbFilePass.Compare(_T(""))!=0)
	{
		if(m_encryption)
		{
			CStringA strPass;
#if !defined(_UNICODE) && !defined(UNICODE)
			strPass=UnicodeToUtf8S(MultiToUnicodeS(strDBFilePassword));
#else
			strPass=UnicodeToUtf8S(strDBFilePassword);
#endif
			SqliteDBKey(db, strPass,strPass.GetLength());
		}
		else
		{
			ErrMsg.Format(_T("Database Encryption Not Supported !"));
			SqliteDBClose(db);
			return true;
		}
	}

	m_bConnected|= 1;
	dbPathName=strDBPathName;
	return false;
}

int CSqliteDB::ChangePass(LPCTSTR strNewPass)// adatbazis jelszavanak megvaltoztatasa
{
	if ((m_bConnected&1)==1)
	{
		if(m_encryption)
		{
			CStringA str;
#if !defined(_UNICODE) && !defined(UNICODE)
			str=UnicodeToUtf8S(MultiToUnicodeS(strNewPass));
#else
			str=UnicodeToUtf8S(strNewPass);
#endif
			return SqliteDBRekey(db, str,str.GetLength());
		}
		else
		{			
			ErrMsg.Format(_T("Database Encryption Not Supported !"));
			return true;
		}
	}
	return true;
}

LPCTSTR CSqliteDB::GetLastError()// a legutobbi hibat adja vissza, mely az adatbazissal kapcsolatban tortent
{
	return ErrMsg;
}

CString CSqliteDB::GetLibversion()
{
	CString str;
	const char* libver=SqliteDBLibversion();
#if defined (_UNICODE) || defined (UNICODE)
	str=Utf8ToUnicodeS(libver);
#else
	str=UnicodeToMultiS(Utf8ToUnicodeS(libver));
#endif
	return str;
}

int CSqliteDB::Close()// megnyitott adatbazis bezarasa
{
	int iRes=0;
	if(m_bConnected & 1)
		iRes=SqliteDBClose(db);
	if(!iRes)//ha sikerul bezarni az adatbazist
		m_bConnected-=m_bConnected&1;
	return iRes;
}

int CSqliteDB::Execute(LPCTSTR strQuery)  // sql parancs kiadasa adatbazis modositas celjabol. lekerdezes nem lehetseges
{
	if((IsConnected()&1)!=1)
	{
		ErrMsg=_T("Database not connected");
		return 1;
	}
	
	// A UTF-16 version of sqlite_exec does not exist at this time,
	// so we will convert the string if necessary.
	CStringA str;
#if !defined(_UNICODE) && !defined(UNICODE)
	str=UnicodeToUtf8S(MultiToUnicodeS(strQuery));
#else
	//str=UnicodeToUtf8S(strQuery);
	str=UnicodeToUtf8S(strQuery);
#endif

	int nRetValue;
	char* zErrMsg;
	nRetValue=SqliteDBExecute (db, str, NULL, 0, &zErrMsg);
	
#if defined(_UNICODE) || defined(UNICODE)
	ErrMsg=Utf8ToUnicodeS(zErrMsg).GetBuffer(0);
#else
	ErrMsg=zErrMsg;
#endif
	
	return nRetValue;
}

bool CSqliteDB::SetAuthorizer(AUTH_CALLBACK authFunct,void* parentObj,sqlite3* pDB)
{
	if(!parentObj)
		parentObj=this;

	if(!pDB)
		pDB=db;
	SqliteDBSetAuthorizer(pDB,authFunct,parentObj);
	
	return false;
}

bool CSqliteDB::SetHookCallback(HOOK_CALLBACK alertFunct,void* parentObj,sqlite3* pDB)
{
	if(!parentObj)
		parentObj=this;

	if(!pDB)
		pDB=db;
	SqliteDBUpdateHook(pDB,alertFunct,parentObj);
	
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////
// CSqliteDBRecordSet Class
//////////////////////////////////////////////////////////////////////

CSqliteDBRecordSet::CSqliteDBRecordSet()
{
	InitInstance();
}

CSqliteDBRecordSet::~CSqliteDBRecordSet()
{
	if (!m_bIsEmpty)
		Clear();
}

bool CSqliteDBRecordSet::Clear()// torli a rekorszettet
{
	if (m_bIsEmpty)
		return true;

/*	for(UINT i=0;i<orsTable.size();i++)
		delete[] orsTable[i];
	orsTable.clear();*/

	while (!orsTable.empty())
	{
		delete[] orsTable.back();
		orsTable.pop_back();
	}
	
	nColumnsType.RemoveAll();
	strColumnsList.RemoveAll();

	m_nColumnsCount = strColumnsList.GetSize();
	m_nCurrentRecord = 0;
	m_nRecordsCount = 0;
	
	m_bIsEmpty=true;
	return false;
}

bool CSqliteDBRecordSet::InitInstance()//alapertekek beallitasa
{
	ErrMsg=_T("");
	m_nCurrentRecord = 0;
	m_nRecordsCount = 0;
	m_nColumnsCount = 0;
	m_bIsEmpty = true;

	return false;
}

UINT CSqliteDBRecordSet::Query(LPCTSTR strQueryString, void *ptrDB, int firstElement, int maxElement)// a rekordszet feltoltese sql paranccsal
{
	if (((CSqliteDB*)ptrDB)->IsLibLoaded() == false)
		return RSQUERY_NOLIBLOADED;

	if ((((CSqliteDB*)ptrDB)->IsConnected()&1)!=1)
		return RSQUERY_NOCONNECT;

	if (!m_bIsEmpty)
		Clear();
	
	CString strBuffer=strQueryString; //temp str
	sqlite3_stmt *stmtByteCode; // bytecode
	INT nCount = 0; //temp szamlalo

#if !defined(_UNICODE) && !defined(UNICODE)
	strBuffer=UnicodeToUtf8S(MultiToUnicodeS(strQueryString));
#endif

	int result=SqliteDBPrepare(((CSqliteDB*)ptrDB)->db, strBuffer, -1, &stmtByteCode, NULL); //atalakitjuk a lekerdezest bytekodda

	if(result)
	{
		ErrMsg=SqliteDBErrmsg(((CSqliteDB*)ptrDB)->db);//ha nem sikerul a hibat taroljuk
		return RSQUERY_PREPAREERROR;
	}
	
	nCount = SqliteDBColumnCount(stmtByteCode); //ennyi oszlop van a lekerdezes eredmenyeben
	for (LONG i=0; i < nCount; i++)
	{
#if !defined(_UNICODE) && !defined(UNICODE)
		strBuffer=UnicodeToMultiS(Utf8ToUnicodeS(SqliteDBColumnName (stmtByteCode, i))); // az oszlopnevet
#else
		strBuffer = SqliteDBColumnName (stmtByteCode, i); // az oszlopnevet
#endif
		strColumnsList.Add (strBuffer);//hozzaadjuk az oszlopnevek tombjehez
	}
	m_nColumnsCount = strColumnsList.GetSize();//taroljuk az oszlopok szamat
	
	// return, If there is no field in the table
	if (m_nColumnsCount == 0)
		return RSQUERY_INVALIDQRY;
	m_bIsEmpty = false;//mar nem ures az objektum



	bool bTypesSaved=false; //temp boolean
	LONG tmpRowCount=0;
	while (SqliteDBStep (stmtByteCode) != SQLITE_DONE)	// vegigmegyunk az eredmenytabla sorain
	{
		if(firstElement>tmpRowCount)
		{
			tmpRowCount++;
			continue;
		}
//		CStringArray *strRSRow = new CStringArray();//letrehozunk a sor szamara egy uj tombot
		for (int i=0; i < m_nColumnsCount; i++) //vegigmegyunk a sor oszlopain
		{
			if (!bTypesSaved)	// elmentjuk az oszlopok tipusat. de csak az elso sor eseten
			{
			//	nType = SqliteDBColumnType (stmtByteCode, i);	// column_type
			//	nFieldsType.Add((UINT)nType);
				nColumnsType.Add(SqliteDBColumnType (stmtByteCode, i));
				if ((LONG)i == m_nColumnsCount-1)
					bTypesSaved = true;
			}
			
			double fVal=0;
			switch (SqliteDBColumnType (stmtByteCode, i))
			{
				case SQLITE_INTEGER:
				case SQLITE_FLOAT:
					fVal=SqliteDBColumnDouble (stmtByteCode, i);
					break;
			}
/*			int fType=SqliteDBColumnType (stmtByteCode, i);// minden mezo erteket a tipusa alapjan adunk vissza
			switch (fType)// elofordulhat, hogy egy oszlopon belul kulonbozo tipusok vannak.
			//switch (nColumnsType.GetAt(i))
			{
				case SQLITE_INTEGER:
					strBuffer.Format(_T("%I64d"), SqliteDBColumnInt64 (stmtByteCode, i));
					strRSRow->Add(strBuffer);
					break;
				case SQLITE_FLOAT:
					strBuffer.Format(_T("%f"), SqliteDBColumnDouble (stmtByteCode, i));
					strRSRow->Add(strBuffer);
					break;
				case SQLITE_TEXT:*/
#if !defined(_UNICODE) && !defined(UNICODE)
					strBuffer = UnicodeToMultiS(Utf8ToUnicodeS(SqliteDBColumnText (stmtByteCode, i)));
#else
					strBuffer = SqliteDBColumnText (stmtByteCode, i);
#endif
					//strRSRow->Add(strBuffer);


int tLen=strBuffer.GetLength()+1;
LPTSTR sT=new TCHAR[tLen];
_tcscpy_s(sT,tLen,strBuffer.GetBuffer());
orsTable.push_back(sT);

//AfxMessageBox(stringVector.at(m_nRecordsCount*m_nColumnsCount+i));
//LPTSTR sT=new TCHAR[_tcslen(sTxt)];
//_tcscpy_s(sT,_tcslen(sT),sTxt);
//strArray=(LPCTSTR*)realloc(strArray,(m_nRecordsCount*m_nColumnsCount+i+1)*sizeof(LPCTSTR));
//strArray[m_nRecordsCount*m_nColumnsCount+i]=sT;
//AfxMessageBox(strArray[m_nRecordsCount*m_nColumnsCount+i]);
/*					break;
				case SQLITE_BLOB:
					strBuffer = (LPCTSTR) SqliteDBColumnBlob (stmtByteCode, i);
					strRSRow->Add(strBuffer);
					break;
				case SQLITE_NULL:
					strRSRow->Add(_T(""));
					break;
			}*/
		}
		m_nRecordsCount++; //sorok szama
		tmpRowCount++;//
		if(m_nRecordsCount>=maxElement)
			break;
	}

	m_nCurrentRecord = 0;// az aktualis sor a 0.

	if(SqliteDBFinalize(stmtByteCode)) //torli a prepare-el keszult bytekodot

	{
		ErrMsg=SqliteDBErrmsg(((CSqliteDB*)ptrDB)->db);//ha nem sikerul a hibat taroljuk
		return RSQUERY_FINALIZEERROR;
	}

	return RSQUERY_SUCCESS;
}

LPCTSTR CSqliteDBRecordSet::GetLastError()// a legutolso hibajelzes, mely a rekordszettel kapcsolatos
{
	return ErrMsg;
}

bool CSqliteDBRecordSet::IsLast()//true, ha az utolso soron allunk
{
	if (m_bIsEmpty||m_nRecordsCount==0)// ha ures, vagy nincs benne sor
		return true;
	return (m_nCurrentRecord == m_nRecordsCount-1);
}

bool CSqliteDBRecordSet::IsFirst()// true, ha az elso soron allunk
{
	if (m_bIsEmpty)
		return true;
	return (m_nCurrentRecord == 0);
}

LPCTSTR CSqliteDBRecordSet::GetFieldString(LONG nIndex)// visszaadja a pillanatnyi sor, es a megadott oszlop altal kijelolt elemet
{
	if (m_bIsEmpty||m_nCurrentRecord < 0 || m_nCurrentRecord >= m_nRecordsCount || nIndex >= m_nColumnsCount)
		return _T("");

	return orsTable[m_nCurrentRecord*m_nColumnsCount+nIndex];
}

double CSqliteDBRecordSet::GetFieldValue(LONG nIndex)// visszaadja a pillanatnyi sor, es a megadott oszlop altal kijelolt elemet
{
	if (m_bIsEmpty||m_nCurrentRecord < 0 || m_nCurrentRecord >= m_nRecordsCount || nIndex >= m_nColumnsCount)
		return 0;

	//return orsTable[m_nCurrentRecord*m_nColumnsCount+nIndex].fValue;
	return _tstof(orsTable[m_nCurrentRecord*m_nColumnsCount+nIndex]);
}

LONG CSqliteDBRecordSet::GetColumnIndex(LPCTSTR strColName)//az oszlop nev alapjan visszaadja az oszlop szamat
{
	if (!ColumnsCount())
		return -1;
	int nIndex=-1;
	for (UINT i=0;i<ColumnsCount();i++)
	{
		if(strColumnsList.GetAt(i)==strColName)
		{
			nIndex=i;
			i=ColumnsCount();
		}
	}
	return nIndex;
}

UINT CSqliteDBRecordSet::ColumnsCount()//a lekerdezesben szereplo oszlopok szama
{
	return m_nColumnsCount;
}

LPCTSTR CSqliteDBRecordSet::GetColumnName(LONG nIndex)// az oszlop szam alapjan visszaadja az oszlop nevet
{
	if(m_bIsEmpty)
		return _T("");
	if (nIndex < strColumnsList.GetSize())
		return strColumnsList.GetAt(nIndex);
	else
		return _T("");
}

INT CSqliteDBRecordSet::GetColumnType(LONG nIndex)//az oszlop szam alapjan visszaadja az oszlop tipusat, melyet az elso sor alapjan hataroz meg
{
	if(m_bIsEmpty)
		return 0;
	if (nIndex < nColumnsType.GetSize())
		return (INT)nColumnsType.GetAt(nIndex);
	else
		return 0;
}

UINT CSqliteDBRecordSet::RecordsCount()//a rekordszet sorainak szama
{
	if (m_bIsEmpty)
		return 0;
	return m_nRecordsCount;
}

void CSqliteDBRecordSet::MoveFirst()//ugras a recordszet elso sorara
{
	m_nCurrentRecord = 0;
}

void CSqliteDBRecordSet::MoveLast()//ugras a recordszet utolso sorara
{
m_nCurrentRecord = m_nRecordsCount - 1;
}

void CSqliteDBRecordSet::MoveNext()//ugras a recordszet kovetkezo sorara
{
	if (m_nCurrentRecord < m_nRecordsCount-1)
		m_nCurrentRecord++;
}

void CSqliteDBRecordSet::MovePrevious()//ugras a recordszet elozo sorara
{
	if (m_nCurrentRecord > 0)
		m_nCurrentRecord--;
}

void CSqliteDBRecordSet::MoveTo(LONG nIndex)//ugras a recordszet megadott sorara
{
	if (nIndex < m_nRecordsCount && nIndex >=0)
		m_nCurrentRecord=nIndex;
}

CUIntArray* CSqliteDBRecordSet::_GetColTypeArray()
{
	return &nColumnsType;
}
CStringArray* CSqliteDBRecordSet::_GetColListArray()
{
	return &strColumnsList;
}
std::vector<TCHAR*>* CSqliteDBRecordSet::_GetResTableArray()
{
	return &orsTable;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// Global functions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CStringW MultiToUnicodeS(LPCSTR multi)
{
	CStringW utf16(_T(""));
	if(!multi)
		return utf16;
	DWORD dwNum = MultiByteToWideChar (GetACP(), 0, multi, -1, NULL, 0);//Get the size of the string by setting the 4th parameter to -1
	WCHAR *unistring = new WCHAR[dwNum];//ebben taroljuk az unicode formatumot
	MultiByteToWideChar(GetACP(), 0, multi, -1, unistring, dwNum); // ANSI to UNICODE

	
  	utf16 = CStringW(unistring);
	delete [] unistring;

	return utf16; //utf16 encoded string
}

CStringW Utf8ToUnicodeS(LPCSTR utf8)
{
	CStringW utf16(_T(""));
	if(!utf8)
		return utf16;
	DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, utf8, -1, NULL, 0);//Get the size of the string by setting the 4th parameter to -1
	WCHAR *unistring = new WCHAR[dwNum];//ebben taroljuk az unicode formatumot
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unistring, dwNum); // ANSI to UNICODE

  	utf16 = CStringW(unistring);
	delete [] unistring;

	return utf16; //utf16 encoded string
}

CStringA UnicodeToUtf8S(LPCWSTR unicode)
{
	CStringA utf8(_T(""));
	if(!unicode)
		return utf8;
	DWORD dwNum=WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);// az utf8 string ennyi karakterbol fog allni.
	char *utf8string = new char[dwNum]; //ebben taroljuk az ansi stringet
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8string, dwNum, 0, 0); // UNICODE to UTF-8

	
  	utf8 = CStringA(utf8string);

	delete [] utf8string;
	return utf8; //utf8 encoded string
}

CStringA UnicodeToMultiS(LPCWSTR unicode)
{
	CStringA utf8(_T(""));
	if(!unicode)
		return utf8;
	DWORD dwNum=WideCharToMultiByte(GetACP(), 0, unicode, -1, NULL, 0, NULL, NULL);// az utf8 string ennyi karakterbol fog allni.
	char *multistring = new char[dwNum]; //ebben taroljuk az ansi stringet
	WideCharToMultiByte(GetACP(), 0, unicode, -1, multistring, dwNum, 0, 0); // UNICODE to UTF-8

	CStringA multi(_T(""));
  	multi = CStringA(multistring);

	delete [] multistring;
	return multi; //utf8 encoded string
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
// A kiválasztott fájlt a megadott db table táblájának blob típusú column mezõjébe insertálja
int CSqliteDB::blobInsert( char* table, char* column, BLOBSTAT* stat )
{
	CFileDialog dlg( TRUE, L".*", NULL, OFN_HIDEREADONLY | OFN_EXPLORER,
		L"jpg files(*.jpg)|*.jpg|pdf files(*.pdf)|*.pdf|All Files (*.*)|*.*||" );
	if( dlg.DoModal( ) == IDCANCEL ) return 0;
	POSITION pos = dlg.GetStartPosition( );

	CString str;
	CString picSpec;
	CString ext(L"");
	int		ix;
	int rc;

	picSpec = dlg.GetNextPathName( pos );
	FILE* f;
	_wfopen_s( &f, picSpec,  L"rb");
	if(NULL == f)
	{
		str.Format( L"Couldn't open file %s\n", picSpec );
		AfxMessageBox( str );
		return 0;
	}
	fseek(f, 0, SEEK_END);
	long filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	if( ( ix = picSpec.ReverseFind( '.' ) ) != -1 )
	{
		ext = picSpec.Mid( ix + 1 );
		ext = ext.MakeLower();
	}
	stat->fileSpec	= picSpec;
	stat->size		= filesize; 
	stat->ext		= ext;

///////////////
	CString insert_sql;
	sqlite3_stmt *insert_stmt;

	CString tableS;
	tableS = table;
	CString columnS;
	columnS = column;

	if( !IsConnected() )
	{
		AfxMessageBox( L"Nincs connect!" );
		return 0;
	}


	insert_sql.Format( L"INSERT INTO %s ( %s ) VALUES ( (?) )", tableS, columnS );
	rc = SqliteDBPrepare( db, insert_sql, -1, &insert_stmt, NULL);
	if(SQLITE_OK != rc)
	{
		str.Format( L"Can't prepare insert statment %s (%i): %s\n", insert_sql, rc, SqliteDBErrmsg(db) );
		AfxMessageBox( str );
		return 0;
	}
	// Bind a block of zeros the size of the file we're going to insert later
	SqliteDBBindZeroblob( insert_stmt, 1, filesize );
	if(SQLITE_DONE != (rc = SqliteDBStep( insert_stmt )))
	{
		str.Format( L"Insert statement didn't work (%i): %s\n", rc, SqliteDBErrmsg(db));
		AfxMessageBox( str );
		return 0;
	}
	_int64 rowid = SqliteDBLastInsertRowid(db);

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid
	sqlite3_blob *blob;
	rc = SqliteDBBlobOpen( db, "main", table, column,  rowid, 1, &blob );
	if(SQLITE_OK != rc)
	{
		str.Format( L"Couldn't get blob handle (%i): %s\n", rc, SqliteDBErrmsg(db));
		AfxMessageBox( str );
		return 0;
	}

	const int BLOCKSIZE = 1024;
	int len;
	void *block = malloc(BLOCKSIZE);
	
	int offset = 0;

	while(0 < (len = fread(block, 1, BLOCKSIZE, f)))
	{
		if(SQLITE_OK != (rc = SqliteDBBlobWrite( blob, block, len, offset )))
		{
			str.Format( L"Error writing to blob handle. Offset %i, len %i\n", offset, len) ;
			AfxMessageBox( str );
			return 0;
		}
		offset+=len;
	}
	SqliteDBBlobClose(blob);
	return rowid;
}
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int CSqliteDB::blobInsert( char* table, char* column, BLOBSTAT* stat )
{
	CString str;
	CString insert_sql;
	sqlite3_stmt *insert_stmt;
	int rc;

	if( !IsConnected() )
	{
		AfxMessageBox( L"Nincs connect!" );
		return 0;
	}

	CString tableS;
	CString columnS;
	tableS = table;
	columnS = column;
	insert_sql.Format( L"INSERT INTO %s ( %s ) VALUES ( (?) )", tableS, columnS );
	rc = SqliteDBPrepare( db, insert_sql, -1, &insert_stmt, NULL);
	if(SQLITE_OK != rc)
	{
		str.Format( L"Can't prepare insert statment %s (%i): %s\n", insert_sql, rc, SqliteDBErrmsg(db) );
		AfxMessageBox( str );
		return 0;
	}
	// Bind a block of zeros the size of the file we're going to insert later
	SqliteDBBindZeroblob( insert_stmt, 1, (int)stat->size );
	if(SQLITE_DONE != (rc = SqliteDBStep( insert_stmt )))
	{
		str.Format( L"Insert statement didn't work (%i): %s\n", rc, SqliteDBErrmsg(db));
		AfxMessageBox( str );
		return 0;
	}
	_int64 rowid = SqliteDBLastInsertRowid(db);

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid
	sqlite3_blob *blob;
	rc = SqliteDBBlobOpen( db, "main", table, column,  rowid, 1, &blob );
	if(SQLITE_OK != rc)
	{
		str.Format( L"Couldn't get blob handle (%i): %s\n", rc, SqliteDBErrmsg(db));
		AfxMessageBox( str );
		return 0;
	}

	const int BLOCKSIZE = 1024;
	int len;
	void *block = malloc(BLOCKSIZE);
	
	int offset = 0;
	FILE* f;
	_wfopen_s( &f, stat->fileSpec,  L"rb");
	if(NULL == f)
	{
		str.Format( L"Couldn't open file %s\n", stat->fileSpec );
		AfxMessageBox( str );
		return 0;
	}

	while(0 < (len = fread(block, 1, BLOCKSIZE, f)))
	{
		if(SQLITE_OK != (rc = SqliteDBBlobWrite( blob, block, len, offset )))
		{
			str.Format( L"Error writing to blob handle. Offset %i, len %i\n", offset, len) ;
			AfxMessageBox( str );
			return 0;
		}
		offset+=len;
	}
	SqliteDBBlobClose(blob);
	return (int)rowid;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void* CSqliteDB::blobRead( char* table, char* column, CString rowid, _int64* size )
{
	CString str;
	int rc;

	sqlite3_blob *blob;
	rc = SqliteDBBlobOpen(db, "main", table, column, _wtoi(rowid), 1, &blob);
	if(SQLITE_OK != rc)
	{
		str.Format( L"Couldn't get blob handle (%i): %s\n", rc, SqliteDBErrmsg(db));
		AfxMessageBox( str );
//		SqliteDBClose(db);
		return NULL;
	}

	int blob_size;

	blob_size = SqliteDBBlobBytes( blob );
	if( blob_size == 0  )
	{
		AfxMessageBox( L"blob_size 0!" );
		return NULL;
	}

	*size = blob_size;

	void *block = malloc(blob_size);
	rc = SqliteDBBlobRead( blob, block, blob_size, 0 );
	if( rc != SQLITE_OK )
	{
		AfxMessageBox( L"blob_read failed!" );
		return NULL;
	}

	SqliteDBBlobClose( blob );
	return block;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
_int64 CSqliteDB::lastInsertRowid()
{
	return( SqliteDBLastInsertRowid(db) );
}