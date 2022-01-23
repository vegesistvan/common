// OpenDatabase.cpp : implementation file
//

#include "stdafx.h"
#include "OpenDatabase.h"
#include "utilities.h"

enum
{
	MASTERTABLE_TYPE = 0,						// 'table' or 'index'
	MASTERTABLE_NAME,
	MASTERTABLE_TBL_NAME,
	MASTERTABLE_ROOTPAGE,
	MASTERTABLE_SQL							// CREATE .....
};
//int splitToSubstrings( CStringArray* A, CString line, TCHAR sep );
IMPLEMENT_DYNAMIC(COpenDatabase, CWnd)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COpenDatabase::COpenDatabase()
{
	m_dbPathName.Empty();
	m_connDB			= NULL;
	m_numberOfIndexes	= 0;
	m_numberOfTables	= 0;
	m_databaseTables	= NULL;
	m_databaseIndexes	= NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
COpenDatabase::~COpenDatabase()
{
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(COpenDatabase, CWnd)
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COpenDatabase::openDatabase() 
{
	bool ret = true;
	if( m_dbPathName.IsEmpty() )
	{
		AfxMessageBox( L"Az adatb�zis f�j specifik�ci�ja nincs megadva!" );
		exit( -1 );
	}
	if( m_connDB == NULL )
	{
		AfxMessageBox( L"Az adatb�zis f�j handle-je nincs megadva!" );
		exit( -1 );
	}

	if( m_databaseTables == NULL )
	{
		AfxMessageBox( L"Az adatb�zis t�bl�k DB strukt�r�ja ( m_databaseTables ) nincs megadva!" );
		exit( -1 );
	}
	if( !m_numberOfTables )
	{
		AfxMessageBox( L"A t�bl�k sz�ma ( m_numberOfTables ) nincs megadva!" );
		exit( -1 );
	}


	if( m_connDB->IsConnected()) m_connDB->Close();
	
	if( m_connDB->Connect( m_dbPathName, L"" ) )
	{
		str.Format( L"%s (key:%s)\nconnect error\n%s", m_dbPathName, L"", m_connDB->GetLastError() );
		AfxMessageBox(str);
		exit( -1 );
	}

	m_tabla		= new CSqliteDBRecordSet;
	



	m_command = L"SELECT * FROM sqlite_master WHERE type == 'table' ORDER BY tbl_name";  // ez az�rt is kell, mert ha hiba van, akkor ez ad hibajelz�st!!
	if( !query( m_command ) ) exit( -1 );

	if( !m_tabla->RecordsCount() )		// az adatb�zis �res
	{
		m_command.Format(L"PRAGMA encoding = 'UTF-8'" );
		if( !execute( m_command ) ) exit( -1 );

		for( int i = 0; i < m_numberOfTables; ++i )	// az �sszes t�bl�a l�trehoz�sa
		{
			if( !createTable( m_databaseTables[i].name, (COLUMN*)m_databaseTables[ i ].columns, m_databaseTables[ i ].size ) )
				exit( -1 );
		}
		for( int i = 0; i < m_numberOfIndexes; ++i )
		{
			m_command.Format( L"CREATE INDEX %s ON %s (%s)", m_databaseIndexes[i].name, m_databaseIndexes[i].table, m_databaseIndexes[i].column ); 
			if( !execute( m_command ) ) exit( -1 );
		}
	}
	else
	{
		if( !checkStructure( m_tabla, m_databaseTables, m_numberOfTables, m_dbPathName ) )
		{
			WriteProfileString( L"Settings",L"databasespec", L"" ); 
			ret = false;
		}
		if( !checkIntegrity() ) ret = false;
	}
	return ret;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL COpenDatabase::createTable( CString tablename, COLUMN *tableStruct, int n )
{
	str.Format( L"CREATE TABLE IF NOT EXISTS %s (", tablename );
	for( int i=0;  i < n; ++i )
	{
		m_command=str;
		str.Format(L"%s '%s' %s",m_command, tableStruct[i].name,tableStruct[i].type);
		if(i < n-1 ) str = str +L","; 
	}
	m_command = str + L")"; 

	if( !execute( m_command  ) ) return FALSE;
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COpenDatabase::checkStructure( CSqliteDBRecordSet* rec, const DB* fileStructure, int tableNumberDB, CString fileSpec )
{
	bool ret = true;
	int tableCnt = rec->RecordsCount();
	
	if( tableCnt != tableNumberDB )
	{
		 str.Format( L"%s\nadatb�zis f�jlban %d t�bla van, pedig %d-nek kellen lenni!\n", fileSpec, tableCnt, tableNumberDB );
		 str +=  L"\nJel�lj ki egy m�sik adatb�zis f�jlt vagy adj meg egy �jat!";
		 AfxMessageBox( str );
		 return false;
	}


	CString createCommand;
	CString columnName;
	int		numOfColumnsDB;
	std::vector<CString> vColumns;
	CString tableName;

	for( UINT i = 0; i < rec->RecordsCount(); ++i, rec->MoveNext() )
	{
		tableName = rec->GetFieldString( MASTERTABLE_NAME );
		if( tableName != fileStructure[i].name )
		{
			str.Format( L"%s\nadatb�zis f�jlban a %d. t�bla n�v '%s', pedig '%s'-nek kellen lenni!\n", fileSpec, i+1, tableName, fileStructure[i].name );
			str +=  L"\nJel�lj ki egy m�sik adatb�zis f�jlt vagy adj meg egy �jat!";
			AfxMessageBox( str );
			ret =  false;
			break;
		}
		createCommand	= rec->GetFieldString( MASTERTABLE_SQL );
		createColumnVector( createCommand, &vColumns );
		numOfColumnsDB	= fileStructure[i].size;

		if( numOfColumnsDB != vColumns.size() )
		{
			str.Format( L"%s\nadatb�zis f�jlban '%s' t�bl�j�ban %d oszlop van,\n pedig %d-nek kellen lenni!\n", fileSpec, tableName, vColumns.size(), numOfColumnsDB );
			str +=  L"\nJel�lj ki egy m�sik adatb�zis f�jlt vagy adj meg egy �jat!";
			AfxMessageBox( str );
			ret = false;
			break;
		}

		for( UINT j = 0 ; j < vColumns.size(); ++j )
		{
			columnName = fileStructure[i].columns[j].name;
			if( vColumns.at(j).MakeLower() != columnName.MakeLower() )
			{
				str.Format( L"A %s\nadatb�zis f�jl '%s' t�bl�j�ban a %d. oszlop '%s'\npedig '%s'-nek kellen lenni!\n", fileSpec, tableName, j+1, vColumns.at(j), columnName );
				str +=  L"\nJel�lj ki egy m�sik adatb�zis f�jlt vagy adj meg egy �jat!";
				AfxMessageBox( str );
				ret = false;
				break;
			}
		}
		if( !ret ) break;
	}
	return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COpenDatabase::createColumnVector( CString list, std::vector<CString>* vColumns )
{
	// az oszlop n�v 3 f�le lehet:
	// 1. 'oszlop'		pl. CREATE TABLE authors ( 'author' TEXT,'nationality_id' TEXT )
	// 2. [oszlop]		pl. CREATE TABLE authors ( [author] TEXT,[nationality_id] TEXT )
	// 3. oszlop		pl. CREATE TABLE authors ( author TEXT, nationality_id TEXT )

	CStringArray A;
	int n;
	int i;

	int pos1;
	int pos2;
	if( ( pos1 = list.Find( '(' ) ) == -1 )
	{
		str.Format( L"%s\n hib�s!!", list );
		AfxMessageBox( str );
		return false;
	}
	if( ( pos2 = list.Find( ')' ) ) == -1 )
	{
		str.Format( L"%s\n hib�s!!", list );
		AfxMessageBox( str );
		return false;
	}
	str = list.Mid( pos1 + 1, pos2-pos1 - 1 );

	vColumns->clear();

//	n = splitToSubstrings( &A, str, ',' );
	n = splitCString( str, ',', false, &A );
	for( i = 0; i < n; ++i )
	{
		str = A.GetAt( i );
		str.Trim();
		if( ( pos1 = str.Find( ' ' ) ) != -1 )
			str = str.Left( pos1 );
		str.Remove( '[' );
		str.Remove( ']' );
		str.Remove( '\'' );
		str.Trim();
		vColumns->push_back( str );
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COpenDatabase::checkIntegrity()
{
	bool hiba = false;
	CString result;
	return true;   // munk�hoz kikapcsolom

	if( !query( L"PRAGMA integrity_check" ) ) goto z;
	result = m_tabla->GetFieldString( 0 );
	if( result != L"ok" )
	{
		hiba = true;
		str = L"Els� integrity check eredm�nye:\n\n";
		str += result;
		str += L"\n";
		if( !execute( L"REINDEX" ) ) goto z;
		
		if( !query( L"PRAGMA integrity_check" ) ) goto z;
		result = m_tabla->GetFieldString( 0 );
		if( result != L"ok" )
		{
			if( !execute( L"VACUUM" ) ) goto z;
			if( !query( L"PRAGMA integrity_check" ) ) goto z;
			result = m_tabla->GetFieldString( 0 );
			if( result != L"ok" )
			{
				str += L"\n\nREINDEX ut�n is probl�ma van. VACUUM ut�na integrity check-ben is probl�ma van:\n\n";
				str += L"\nAdatveszt�s t�rt�nhetett!!";
			}
		}
		else
		{
			str = L"REINDEX megoldotta a probl�m�t!";
		}
	}

z:	if( hiba )
	{
		AfxMessageBox( str );
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COpenDatabase::query( CString command )
{
	if( m_tabla->Query(command, m_connDB ))
	{
		str.Format(L"%s\n%s",command, m_tabla->GetLastError());
		AfxMessageBox(str);
		return FALSE;
	}
	return TRUE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool COpenDatabase::execute(CString command)
{
	if (m_connDB->Execute(command))
	{
		str.Format(_T("%s\n%s"), command, m_connDB->GetLastError());
		AfxMessageBox(str);
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
int splitToSubstrings( CStringArray* A, CString line, TCHAR sep )
{
	CString substring;

	A->RemoveAll();
	int i = 0; // substring index to extract
	while (AfxExtractSubString( substring, line, i, sep ))
	{
		++i;
		A->Add( substring );
	}
	return i;
}
*/