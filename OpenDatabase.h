#pragma once
#include "SqliteDBEx.h"

typedef struct
{
	CString name;
	CString type;
}COLUMN;

typedef struct
{
	TCHAR* name;
	const COLUMN* columns;
	int		size;
}DB;

typedef struct
{
	TCHAR* table;
	TCHAR* column;
	TCHAR* name;
}INDEXES;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// az openDatabase metódus létrehoz vagy megnyit és ellenõriz egy adtabázis.

// A metódus meghívása elõtt meg kell adni az alábbi publikus változók értékeit:

// CString		m_databseSpec		az adatbázis fájl specifikációja
// CSqliteDB*	m_connDb			a megnyitandó adatbázis handle
// DB*			m_databaseTables	az adatbázis táblái
// INDEXES*		m_databaseIndexes	az adatbázis indexei
// int			m_numberOfTables	az adatbázis tábláinak száma
// int			m_numberOfIndexes	az adatbázis indexeinek száma
//
// A DB és INDEX struktúrák a structures.h include fájlban vannak definiálva.
// 
// Ha a megadott adatbázis fájl létezik, akkor a megnyitást követõen az alábbi ellenõrzéseket végz iel:
// 1. A megnyitott adatbázis azokat a táblákat tartalmazza-e, amik a DB strukturában meg vannak adva?
// 2. Az egyes táblák azokat az oszlopokat tartalmazza-e amik a DB strukturában meg vannak adva?
// 3. Ellenõrzi az adatbázis integritását: PRAGMA integrity_check
//
// Ha a megadott adatbázis fájl nem létezik, akkor létrehozza azt a megadott táblákkal és oszlopokkal.
//
// Ha a metódus hívása volt hibás vagy sqlite hiba, akkor hibajlezés után abortál.
// Visszadatott értékek:
// true : hibátlan adatbázis megnyitva
// false: a megnyitott adatbázis volt hibás, másik adatbázist lehet kérni
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class COpenDatabase : public CWnd
{
	DECLARE_DYNAMIC(COpenDatabase)

public:
	COpenDatabase();
	virtual ~COpenDatabase();

	CString			m_databaseSpec;
	CSqliteDB*		m_connDB;
	const DB*		m_databaseTables;		
	const INDEXES*	m_databaseIndexes;
	int				m_numberOfTables;
	int				m_numberOfIndexes;

		
	bool openDatabase();

	
protected:

	CString str;
	CString m_command;

	CSqliteDBRecordSet*	m_tabla;

	BOOL createTable( CString tablename, COLUMN *tableStruct, int n );
	bool checkStructure( CSqliteDBRecordSet* rec, const DB* fileStructure, int tableNumberDB, CString fileSpec );
	bool createColumnVector( CString list, std::vector<CString>* vColumns );
	bool checkIntegrity();

	bool query( CString command );
	bool execute( CString command );

	

	DECLARE_MESSAGE_MAP()
};


