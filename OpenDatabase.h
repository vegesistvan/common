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

// az openDatabase met�dus l�trehoz vagy megnyit �s ellen�riz egy adtab�zis.

// A met�dus megh�v�sa el�tt meg kell adni az al�bbi publikus v�ltoz�k �rt�keit:

// CString		m_databseSpec		az adatb�zis f�jl specifik�ci�ja
// CSqliteDB*	m_connDb			a megnyitand� adatb�zis handle
// DB*			m_databaseTables	az adatb�zis t�bl�i
// INDEXES*		m_databaseIndexes	az adatb�zis indexei
// int			m_numberOfTables	az adatb�zis t�bl�inak sz�ma
// int			m_numberOfIndexes	az adatb�zis indexeinek sz�ma
//
// A DB �s INDEX strukt�r�k a structures.h include f�jlban vannak defini�lva.
// 
// Ha a megadott adatb�zis f�jl l�tezik, akkor a megnyit�st k�vet�en az al�bbi ellen�rz�seket v�gz iel:
// 1. A megnyitott adatb�zis azokat a t�bl�kat tartalmazza-e, amik a DB struktur�ban meg vannak adva?
// 2. Az egyes t�bl�k azokat az oszlopokat tartalmazza-e amik a DB struktur�ban meg vannak adva?
// 3. Ellen�rzi az adatb�zis integrit�s�t: PRAGMA integrity_check
//
// Ha a megadott adatb�zis f�jl nem l�tezik, akkor l�trehozza azt a megadott t�bl�kkal �s oszlopokkal.
//
// Ha a met�dus h�v�sa volt hib�s vagy sqlite hiba, akkor hibajlez�s ut�n abort�l.
// Visszadatott �rt�kek:
// true : hib�tlan adatb�zis megnyitva
// false: a megnyitott adatb�zis volt hib�s, m�sik adatb�zist lehet k�rni
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


