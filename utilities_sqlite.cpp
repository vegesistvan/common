
#include "utilities_sqlite.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getPresentDateTime( CSqliteDB* conn )
{
	CString command;
	CString	dateTime;


	command = L"SELECT datetime('now','localtime')";
	if (!query( conn, command )) return L"";
	dateTime = recset.GetFieldString(0);
	dateTime.Replace('-', '.');
	return dateTime;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL query( CSqliteDB* conn, CString command)
{
	CString str;
	if( recset.Query( command, conn ))
	{
		str.Format(L"%s\n%s", command, recset.GetLastError());
		AfxMessageBox(str);
		return FALSE;
	}
	return TRUE;
}