#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <afxcoll.h>
#include <afxwin.h>
#include "SqliteDBEx.h"


CSqliteDBRecordSetEx recset;

CString getPresentDateTime(CSqliteDB* conn);
BOOL query(CSqliteDB* conn, CString command);