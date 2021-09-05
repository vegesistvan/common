#include "utilities.h"
/*
#include <iostream>


int splitCString(CString cLine, TCHAR sep, bool emptyItem, CStringArray* A);

int main()
{
	CString name(L"Alma Körte Dinnye");
	CStringArray A;
	splitCString(name, ' ', false, &A);

	std::cout << "Hello World!\n";
}
*/


// Input:	s		
//			delim   
//			emptyItem		true|false 
// Output:	elems	vector
// 
//Az "s" stringet a "delim" karakterrel megadot separator szerint felbontja és az elems vektorba tesz.
// Ha emptyItem értéke: false - az egymás után következõ szeparator karaktereket 1-nek értelmezi
//						true - az egymás után következõ szeparator karakterek közötti üres szavakat is elfogadja
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void splitstring( const std::wstring& s, wchar_t delim, bool emptyItem, std::vector<std::wstring>* elems)
{
	elems->clear();
	std::wstringstream ss(s);
	std::wstring item;
	while (std::getline(ss, item, delim))
	{
		if (item.length() || emptyItem)
			elems->push_back(item);
	}
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void split(CString line, wchar_t delim, bool emptyItem, std::vector<CString>* elems)
{
	elems->clear();
	std::wstring s(line, line.GetLength());
	std::wstringstream ss(s);
	std::wstring itemString;
	CString item;
	while (std::getline(ss, itemString, delim))
	{
		item = itemString.c_str();

		if (item.GetLength() || !item.IsEmpty())
			elems->push_back(item);
	}
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
int splitCString( CString cLine, TCHAR sep, bool emptyItem, CStringArray* A )
{
	CString word;
	int index = 0;
	A->RemoveAll();
	cLine.Trim();
	if (!cLine.IsEmpty())
	{
		while( AfxExtractSubString(word, cLine, index, sep) )
		{
			if( !word.IsEmpty() || emptyItem ) A->Add(word);
			++index;
		}
	}
	return (int)A->GetCount();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Columns in 'line' are put into 'arrayP'. 
// Columns are separated by 'sep' character.
// 'single' == TRUE:	one separating character between columns. Between two adjecent separator an empty column is assumed 
//			== FALSE:	may be more separating characters between columns, which considered one separation
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int wordList( CStringArray * arrayP, CString line, char sep, BOOL single  )
{
	int pos = 0;			// position of sep
	int lastpos;			// previous position of sep
	int	length;

	arrayP->RemoveAll();
	//	line.Replace( '\t', ' ' );
	while (pos != -1)
	{
		lastpos = pos;
		pos = line.Find(sep, pos);											// find next sep	
		if (pos != -1)													// there is a sep!
		{
			length = pos - lastpos;										// length of the item between two sep
			if (length || single)arrayP->Add(line.Mid(lastpos, length));	// there is an item between two sep, or empty column if allowed
			pos++;														// step to next position and continue
		}
		else
		{																// no more sep, save last item 
			if (lastpos < line.GetLength())								// if there is no sep after the last item
				arrayP->Add(line.Mid(lastpos));
		}
	}
	return int(arrayP->GetSize());
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
double Log2(double n)
{
	return log(n) / log(2);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TCHAR* thousend(unsigned _int64 val)
{
	static TCHAR result[128];
	TCHAR* pt;
	int n;
	swprintf_s(result, sizeof(result) / sizeof(TCHAR), L"%I64d", val);
	// Point pt at end of string
	for (pt = result; *pt; pt++) {}

	n = 8;		// 3 digit + terminating 0 character
	while (1)
	{
		pt -= 3; // shift 3 digits
		if (pt > result)
		{
			memmove(pt + 1, pt, n);
			*pt = TCHAR(',');	// thousand separator
			n += 8;		// 3 digits + separator
		}
		else
			break;
	}
	return result;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void splitFilespec(CString filespec, CString* drive, CString* path, CString* name, CString* ext)
{
	int length;
	int	p1;			// ':'
	int	p2;			// last '\'
	int	p3;			// last '.'

	length = filespec.GetLength();
	p1 = filespec.Find(':');
	p2 = filespec.ReverseFind('\\');
	p3 = filespec.ReverseFind('.');

	*drive = filespec.Left(p1);
	*path = filespec.Mid(p1 + 1, p2 - p1 - 1);
	*name = filespec.Mid(p2 + 1, p3 - p2 - 1);
	*ext = filespec.Right(length - p3 - 1);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString dropUntil(CString str, char ch)
{
	int pos;

	if ((pos = str.Find(ch)) == -1) return(L"");

	str = str.Right(str.GetLength() - pos - 1);
	return str;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString dropLastCharacter(CString cLine)
{
	CString str;
	int len = cLine.GetLength();
	str = cLine.Left(len - 1);
	return str;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getUntil(CString str, char ch)
{
	int pos;

	if ((pos = str.Find(ch)) == -1)
		return str;

	str = str.Left(pos);
	return str;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString dropLastWord(CString cLine)
{
	CString str(L"");
	int pos;
	cLine.Trim();
	if ((pos = cLine.ReverseFind(' ')) != -1)
		str = cLine.Left(pos);
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString dropFirstWord(CString cLine)
{
	CString str(L"");
	int pos;
	cLine.Trim();
	if ((pos = cLine.Find(' ')) != -1)
		str = cLine.Mid(pos + 1);
	return str;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// a str n. szavát visszadja, pos-ba teszi a szó indexét 
// ha nincs n szó a str-ben, akkor a stringet adja vissza
CString getWord(CString str, int n, int* pos)
{
	CString word;
	word.Empty();
	int pozi;
	int poziS;
	int length = str.GetLength();

	str.Trim();
	if ((pozi = str.Find(' ')) != -1)
	{
		poziS = pozi + 1;
		for (int i = 1; i < n; ++i)
		{
			str = str.Mid(pozi + 1);
			str.Trim();
			if ((pozi = str.Find(' ')) == -1)
			{
				*pos = length;		// nincs több szó a stringben!!
				return str;
			}
			poziS += pozi + 1;
		}
		word = str.Left(pozi);
		word.Trim();
	}
	else
	{
		poziS = -1;
		word = str;
	}
	*pos = poziS;
	return word;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString sepFirstName(CString str)
{
	str.Replace('_', ' ');
	str.Replace('-', ' ');
	return(getFirstWord(str));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getFirstWord(CString str)
{
	int pos;
	str.Trim();
	if ((pos = str.Find(' ')) == -1) return str;
	return(str.Left(pos));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getSecondWord(CString str)
{
	CString out;
	int pos1;
	int pos2;
	str.Trim();
	if ((pos1 = str.Find(' ')) == -1) return L"";			// csak legfeljebb 1 szó van benne;
	if ((pos2 = str.Find(' ', pos1 + 1)) == -1)
	{
		out = str.Mid(pos1 + 1);			// csak 2 szó van benne
	}
	out = str.Mid(pos1 + 1, pos2 - pos1 - 1);
	return(out);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getTwoWords(CString str)
{
	CString out;
	int pos1;
	int pos2;
	str.Trim();
	if ((pos1 = str.Find(' ')) == -1) return L"";			// csak legfeljebb 1 szó van benne;
	if ((pos2 = str.Find(' ', pos1 + 1)) == -1)
	{
		return(str);			// csak 2 szó van benne
	}
	return(str.Left(pos2 - 1));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL isNumeric(CString word)
{
	if (word.SpanIncluding(L"?") == word)  // csak '?' karakter van a szóban 
		return false;
	return(word.SpanIncluding(L"0123456789 ") == word);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL isRoman(CString word)
{
	LPCTSTR valid = L"IVXabcde";
	LPCTSTR valid2 = L"IVX";
	CString subs;
	CString subs2;

	subs = word.SpanIncluding(valid);
	if (subs.GetLength() == word.GetLength())
	{
		subs2 = subs.SpanIncluding(valid2);
		return(!subs2.IsEmpty());
	}
	return FALSE;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString cleanCline(CString cLine)
{

	CString line;
	CString tag;
	CString str;

	line = dropUntil(cLine, '>');						// leszedi	 <> jeleket
	if (line.Find('>') != -1)
		line = dropUntil(line, '>');
	tag = getUntil(line, ';');						// generáci után van;

	line = dropUntil(line, ';');
	line.Remove('%');
	if (line == L"</ol>") line.Empty();
	line.Trim();
	str.Empty();
	if (!line.IsEmpty())
		str.Format(L"%s %s", tag.Left(1), line);
	return str;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A soremlés és más kiszûrése
CString cleanHtmlLine(CString cLine)
{

	CString str;
	CString line;
	CString tag;
	int pos;

	if ((pos = cLine.Find('<')) != -1)
	{
		if (pos)
		{
			cLine = cLine.Left(pos);			// ha <  a sor végén van nem az elsõ helyen van
		}
		else
		{
			if ((pos = cLine.ReverseFind('>')) != -1)
			{
				cLine = cLine.Mid(pos + 1);
			}
		}
	}

	tag = getUntil(cLine, ';');
	if ((pos = cLine.Find(';')) != -1)
		if (pos != cLine.GetLength())
			cLine = dropUntil(cLine, ';');

	str.Empty();
	if (!cLine.IsEmpty())
		if (cLine.GetAt(0) != '%')
			str.Format(L"%s %s", tag.Left(1), cLine);
		else
			str = cLine;
	return str;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A soremlés és más kiszûrése
CString cleanHtmlTags(int lineNumber, CString cLine)
{
	CString str;
	CString str1;
	CString str2;
	CString tag;
	int	pos;
	int pos1 = 0;
	int pos2 = 0;

	while ((pos1 = cLine.Find('<', pos1)) != -1)
	{
		str1 = cLine.Left(pos1);
		if ((pos2 = cLine.Find('>', pos1)) == -1)
		{
			str.Format(L"A '<' karakter után nincs '>' karakter!!\nL%d %s", lineNumber, cLine);
			AfxMessageBox(str, MB_ICONERROR);
			return L"";
		}
		str2 = cLine.Mid(pos2 + 1);
		cLine = str1 + str2;
	}

	tag = getUntil(cLine, ';');
	if ((pos = cLine.Find(';')) != -1)
		if (pos != cLine.GetLength())
			cLine = dropUntil(cLine, ';');

	str.Empty();
	if (!cLine.IsEmpty())
		if (cLine.GetAt(0) != '%')
			str.Format(L"%s %s", tag.Left(1), cLine);
		else
			str = cLine;
	return str;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Az A arrajbõl az n-dik szótól m darab szót összerak szóközzel elválasztva, és azt visszadja
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString packWords(CStringArray* A, int n, int m)
{
	if (!m) return L"";

	CString str(L"");
	int cnt = 0;

	int db = (int)A->GetSize();
	if (db == 4)
		int g = 0;

	for (int j = n; cnt < m; ++j)
	{
		if (j >= db || j < 0)
			int z = 2;

		str += A->GetAt(j);
		str += L" ";
		++cnt;
	}
	if (db == 4)
		int g = 0;
	return str.Trim();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getLastWord(CString cLine)
{
	CString ret(L"");
	int pos;
	if ((pos = cLine.ReverseFind(' ')) != -1)
		ret = cLine.Mid(pos + 1);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setCreationTime(CString filespec)
{
	SYSTEMTIME thesystemtime;
	FILETIME thefiletime;

	GetSystemTime(&thesystemtime);
	SystemTimeToFileTime(&thesystemtime, &thefiletime);

	HANDLE fileHandle = CreateFile(filespec, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetFileTime(fileHandle, &thefiletime, (LPFILETIME)NULL, (LPFILETIME)NULL);
	CloseHandle(fileHandle);
}
bool isLastCharacter(CString str, TCHAR kar)
{
	if (str.IsEmpty()) return false;
	TCHAR kar1 = str.GetAt(str.GetLength() - 1);
	if (kar == kar1) return TRUE;
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// a dátum eõtt álló 'kb' ill. a dátum után álló 'után','körül','kb' a dátum része!
// visszadott ret értéke megadja, hány szóból áll a dátum
int isDate(CStringArray* A, int i, CString* datum)
{
	INT_PTR n = A->GetCount();
	int ret = 0;
	CStringArray M;
	CString date(L"");
	CString miez;
	M.Add(L"után");
	M.Add(L"körül");
	M.Add(L"elõtt");
	M.Add(L"kb");

	miez = A->GetAt(i);
	miez.Replace('?', ' ');
	miez.Replace('.', ' ');
	miez.Trim();
	if (isNumeric(miez))
	{
		++ret;
		if (i > 0 && !A->GetAt(i - 1).Compare(L"kb"))		// kb 1944
		{
			date = L"kb ";
			date += A->GetAt(i);
			++ret;
		}
		else
			date = A->GetAt(i);

		if (i + 1 < n - 1)
		{
			for (int j = 0; j < M.GetCount(); ++j)
			{
				if (!A->GetAt(i + 1).Compare(M[j]))
				{
					date += " ";
					date += M[j];
					++ret;
					break;
				}
			}
		}
	}
	*datum = date;
	return ret;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// a dátum eõtt álló 'kb' ill. a dátum után álló 'után','körül','kb' a dátum része!
BOOL isDateOK(CString datum)
{
	if (datum.IsEmpty()) return true;
	CString date;
	CString modifier;

	date = getFirstWord(datum);
	if (checkDate(date))
	{
		modifier = getSecondWord(datum);
		if (modifier.IsEmpty() || modifier == L"után" || modifier == L"körül" || modifier == L"kb")
			return TRUE;
	}
	return FALSE;
}
/*
//////////////////////////////////////////////////////////////////////////////////////////
BOOL isValidBrace( CString brace )
{
	if( brace.Find( '-' )		!= -1 )	return TRUE;
	if( brace.Find( L"f." )		!= -1 )	return TRUE;
	if( brace.Find( L"lánya" )	!= -1 )	return TRUE;
	if( brace.Find( L"leánya" )	!= -1 )	return TRUE;
	if( brace.Find( L"fia" )	!= -1 )	return TRUE;
	return FALSE;
}
*/
/*
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString date2date(CString date)
{
	CStringArray monthes;

	monthes.Add(L"Jan");
	monthes.Add(L"Feb");
	monthes.Add(L"Mar");
	monthes.Add(L"Apr");
	monthes.Add(L"May");
	monthes.Add(L"Jun");
	monthes.Add(L"Jul");
	monthes.Add(L"Aug");
	monthes.Add(L"Sep");
	monthes.Add(L"Oct");
	monthes.Add(L"Nov");
	monthes.Add(L"Dec");

	CStringArray A;
	int pos1 = -1;
	int pos2 = -1;
	int n;
	int	i;
	int year;
	int month = 0;
	int day;
	CString honap;
	CString modifier(L"");
	CString str;
	CString modi[] = { L"kb", L"elõtt", L"után", L"körül" };


	if ((pos1 = date.Find(L"BEF")) != -1 || (pos2 = date.Find(L"AFT")) != -1)
	{
		if (pos1 != -1) modifier = L" elõtt";
		else if (pos2 != -1) modifier = L" után";
		date = date.Mid(4);
	}

	for (i = 0; i < sizeof(TCHAR) / sizeof(modi); ++i)
	{
		if ((pos1 = date.Find(modi[i])) != -1)
		{
			modifier = modi[i];
			date = date.Left(pos1);
			break;
		}
	}

	n = wordList(&A, date, ' ', FALSE);

	if (n != 3)
	{
		date.Remove('(');
		date.Remove(')');
		date.Trim();

		int len = date.GetLength();
		if (len == 11)
		{
			if (date.GetAt(len - 1) == '.')
			{
				str = date.Left(len - 1);
				date = str;
			}
		}
		return(date);
	}
	else
	{										// 17 DEC 1944
		day = _wtoi(A.GetAt(0));
		year = _wtoi(A.GetAt(2));

		for (int i = 0; i < monthes.GetCount(); i++)
		{
			honap = A.GetAt(1);
			honap.Trim();
			if (!honap.CompareNoCase(monthes[i]))
			{
				month = i + 1;
				break;
			}
		}
		str.Format(L"%4d.%02d.%02d", year, month, day);
	}
	str += modifier;
	str.Trim();
	return str;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString firstUpper(CString name)
{
	CString out;
	CStringArray A;
	int n;
	TCHAR kar;

	n = wordList(&A, name, ' ', false);
	for (int i = 0; i < n; ++i)
	{
		if (A[i] != L"és")
		{
			kar = A[i].GetAt(0);
			kar = TCHAR(toupper(kar));
			A[i].SetAt(0, kar);
		}
	}
	out = packWords(&A, 0, n);
	return out;
}
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString upper( CString str )
{
	int length = str.GetLength();
	TCHAR karI;
	TCHAR karO;
	CString out(L"");
	for( int i = 0; i < length; ++i )
	{
		karI = str.GetAt( i );
		karO = TCHAR(toupper( karI ));
		out += karO;
	}
	return out;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL isOdd(_int64 num)
{
	return(num % 2);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL isEven(_int64 num)
{
	return(!(num % 2));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getString(int n)
{
	CString str;
	str.Format(L"%d", n);
	return str;
}
/*
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TCHAR * string2char( CString str )
{
	int tLen = str.GetLength()+1;
	LPTSTR sT = new TCHAR[tLen];
	_tcscpy_s( sT, tLen, str.GetBuffer());
	return sT;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Elfogadott formátum:
// az évnek 1000 és az aktuális év közé kell esni

// 1944
// 1944.12.
// 1944.12.17

// az üres dátumot is hibásnak értékeli

bool checkDate(CString datum)
{
	datum.Trim();
	int length = datum.GetLength();
	if (length < 4) return false;

	int	year = _wtoi(datum.Left(4));
	if (year < 1000) return false;

	// év ellenõrzése
	char	sTime[50];
	time_t	now = time(0);
	ctime_s(sTime, sizeof(sTime), &now);
	CString st;
	st = sTime;
	int	yearCurr = _wtoi(st.Right(6));		// "\n" miatt 6 karaktert vesz le, ami konverzinál ugyis lemarad
	if (year > yearCurr) return false;

	// év utáni '.' ellenõrzése
	if (length == 4) return true;	// csak 2001 a dátum

	if (datum.GetAt(4) != '.') return false;
	if (length <= 7) return false;

	// hónap ellenõrzése a 2019.11 
	if (!iswdigit(datum.GetAt(5)))
		return false;
	if (!iswdigit(datum.GetAt(6)))
		return false;
	int	month = _wtoi(datum.Mid(5, 2));
	if (month < 1 || month > 12) return false;

	if (length == 8) return true;	// csak 2019.11.  a dátum

	if (datum.GetAt(7) != '.') return false;
	if (length < 10) return false;

	// nap ellenõrzése 2019.11.12
	if (!iswdigit(datum.GetAt(8)))
		return false;
	if (!iswdigit(datum.GetAt(9)))
		return false;
	int	day = _wtoi(datum.Mid(8, 2));
	if (day < 1) return false;
	if (month == 2 && day > 29) return false;
	if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
	if (day > 31) return false;
	if (length > 10) return false;  // dátum után még van valami
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString roundDate( CString date )
{
	int length = date.GetLength();
	switch( length )
	{
	case 4:
		date += L".06.30";
		break;
	case 7:
		date += L".01";
		break;
	case 8:
		date += L"01";
	}
	return date;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Addig olvassa a fájlt, amíg a végére nem ér vagy talált benne UTF8BOM-ot vagy UTF8 kódot
// Default kódolás: ANSI
int GetInputCode(CString fileSpec)
{
	FILE* fl;
	int errno_t;
	CString str;

	if ((errno_t = _wfopen_s(&fl, fileSpec, L"rb")))
	{
		str.Format(L"%s megnyitási hiba!", fileSpec);
		AfxMessageBox(str);
		return NULL;
	}

	byte bom1 = 0XEF;
	byte bom2 = 0XBB;
	byte bom3 = 0XBF;

	int cnt = 1;

#define LEN  100
	UCHAR	buffer[LEN];
	int		length;
	while ((length = (int)fread(buffer, 1, LEN, fl)))
	{
		if (cnt)
		{
			if (buffer[0] == bom1 && buffer[1] == bom2 && buffer[2] == bom3)
			{
				fclose(fl);
				return UTF8BOM;
			}
		}
		cnt = 0;
		if (isUTF8(buffer, length))
		{
			fclose(fl);
			return UTF8;
		}
	}
	fclose(fl);
	return ANSI;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Layout of UTF-8 byte sequences
// Number of bytes	First code point	Last code point	Byte 1		Byte 2		Byte 3		Byte 4
// 1				U+0000				U+007F			0xxxxxxx	
// 2				U+0080				U+07FF			110xxxxx	10xxxxxx	
// 3				U+0800				U+FFFF			1110xxxx	10xxxxxx	10xxxxxx	
// 4				U+10000	[nb 2]		U+10FFFF		11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
bool isUTF8(UCHAR* bytes, int length)
{
	int i = 0;
	UCHAR* b = bytes;
	UCHAR byte;
	while (i < length)
	{
		byte = bytes[i];
		if ((bytes[i] >> 3) == 0X1E && i + 3 < length && (bytes[i + 1] >> 6) == 0X02 && (bytes[i + 2] >> 6) == 0X02 && (bytes[i + 3] >> 6) == 0X02)
			return UTF8;																			// 4 bytes 
		if ((bytes[i] >> 4) == 0X0E && i + 2 < length && (bytes[i + 1] >> 6) == 0X02 && (bytes[i + 2] >> 6) == 0X02)
			return UTF8;																			// 3 bytes
		if ((bytes[i] >> 5) == 0X06 && i + 1 < length && (bytes[i + 1] >> 6) == 0X02)
			return UTF8;																			// 2 bytes
		++i;
	}
	return ANSI;
}

/*
bool isUTF8( UCHAR * bytes, int length )
{
	int i = 0;
	UCHAR* b = bytes;
//	const unsigned char * bytes = (const unsigned char *)string;
//	while( i < length )
	while( *bytes )
	{
		// ASCII use bytes[0] <= 0x7F to allow ASCII control characters
		if( bytes[0] == 0x09 || bytes[0] == 0x0A ||	bytes[0] == 0x0D || (0x20 <= bytes[0] && bytes[0] <= 0x7E) )
		{
			++bytes;
//			++i;
			continue;
		}

		// non-overlong 2-byte
		if( (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&  (0x80 <= bytes[1] && bytes[1] <= 0xBF)  )
		{
			bytes += 2;
//			i += 2;
			continue;
		}

		// excluding overlongs  // straight 3-byte // excluding surrogates
		if( ( bytes[0] == 0xE0 && (0xA0 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) ||
			( ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) || bytes[0] == 0xEE || bytes[0] == 0xEF) && (0x80 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ) ||
			( bytes[0] == 0xED && (0x80 <= bytes[1] && bytes[1] <= 0x9F) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) ))
		{
			bytes += 3;
//			i += 3;
			continue;
		}

		// planes 1-3 // planes 4-15  // plane 16
		if( ( bytes[0] == 0xF0 &&  (0x90 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) && (0x80 <= bytes[3] && bytes[3] <= 0xBF) ) ||
			( (0xF1 <= bytes[0] && bytes[0] <= 0xF3) && (0x80 <= bytes[1] && bytes[1] <= 0xBF) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) && (0x80 <= bytes[3] && bytes[3] <= 0xBF) ) ||
			( bytes[0] == 0xF4 && (0x80 <= bytes[1] && bytes[1] <= 0x8F) && (0x80 <= bytes[2] && bytes[2] <= 0xBF) && (0x80 <= bytes[3] && bytes[3] <= 0xBF) ) )
		{
			bytes += 4;
//			i += 4;
			continue;
		}
		return 0;		// ANSI
	}
	return 1;			// UTF8
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStringA AnsiToUtf8(CString str)
{
	return(UnicodeToUtf8(str));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString Utf8ToAnsi(CString cLine)
{
	const char* pBuffer;
	int len = cLine.GetLength() + 1;
	pBuffer = new char[len];
	USES_CONVERSION;
	pBuffer = T2A(cLine);
	return(Utf8ToUnicode(pBuffer));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  1 : mindkettõ meg van adva és egyezik;
//  0 : csak az egyik vagy egy sincs van megadva, így nincs ellentmondás
//  -1 : mindkettõ meg van adva és nem egyezik
int same(CString ref, CString par1, CString par2)
{
	int len1 = par1.GetLength();
	int len2 = par2.GetLength();
	int len;
	if (len1 && len2)
	{
		len = (len1 < len2) ? (len1) : (len2);
		if (par1.Left(len) == par2.Left(len))
			return 1;
		else
			return -1;
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int getYearFromDate(CString date)
{
	date.Replace('?', ' ');
	date.Trim();
	if (date.Left(2) == L"kb")
		date = date.Mid(3);
	date = date.Left(4);
	date.Trim();
	int year = _wtoi(date);
	if (year < 1000) year = 0;
	return(year);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getTimeTag()
{
	return(CTime::GetCurrentTime().Format("%Y%m%d%H%M"));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool openFileSpec(FILE** ff, CString fileSpec, CString mode)
{
	int		errno_t;
	TCHAR buffer[100];
	CString str;

	//	const wchar_t *filename,

	if ((errno_t = _wfopen_s(ff, fileSpec, mode)) != 0)  // w+ = reading and writing, existing file destroyed
	{
		_wcserror_s(buffer, sizeof(buffer));
		str.Format(L"%s\nmegnyitási hiba!\n%s", fileSpec, buffer);
		AfxMessageBox(str);
		return false;
	}
	return true;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int dateDiff( CString date1, CString date2 )
{
	int diff = _wtoi( date1.Left(4) ) - _wtoi( date2.Left(4) );
	return diff;
}
*/
/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// return:
// 1000000 - nem értékelhetõ

//  0  date1 == date2
// -1  date1 < date2
//  1  date1 > date2
int compDate( CString date1, CString date2 )
{
	int invalid = 100000;
	if( !checkDate( date1 ) || !checkDate(date2 ) ) return invalid;

	int n1 = date1.GetLength();
	int n2 = date2.GetLength();
	if( n1 < n2 )		date2 = date2.Left( n1 );
	else if( n1> n2 )	date1 = date1.Left( n2 );
	return( date1.Compare( date2 ) );
}
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Két dátum YYYY[[.MM.]DD[ formátumú
// a hosszabb dátumot csonkítja a rövidebb hosszára és úgy hasonlítja össze
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int diffD(CString date1, CString date2, int* pYear)
{
	if (date1.IsEmpty() || date2.IsEmpty()) return INT_MAX;
	*pYear = _wtoi(date1.Left(4)) - _wtoi(date2.Left(4));

	int len1 = date1.GetLength();
	int len2 = date2.GetLength();
	int len = len1;

	if (len1 < len2)
		date2 = date2.Left(len1);
	else if (len1 > len2)
	{
		date1 = date1.Left(len2);
		len = len2;
	}
	return(date1.Compare(date2));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString  getNameBD(CString name, CString birth, CString death, CString wedding)
{
	CString nameBD = name;

	if (wedding.IsEmpty())
	{
		if (!birth.IsEmpty() && death.IsEmpty())
			nameBD.Format(L"%s (*%s)", name, birth);
		else if (birth.IsEmpty() && !death.IsEmpty())
			nameBD.Format(L"%s (+%s)", name, death);
		else if (!birth.IsEmpty() && !death.IsEmpty())
			nameBD.Format(L"%s (*%s +%s)", name, birth, death);
	}
	else
	{
		if (!birth.IsEmpty() && death.IsEmpty())
			nameBD.Format(L"%s (*%s =%s)", name, birth, wedding);
		else if (birth.IsEmpty() && !death.IsEmpty())
			nameBD.Format(L"%s (+%s =%s)", name, death, wedding);
		else if (!birth.IsEmpty() && !death.IsEmpty())
			nameBD.Format(L"%s (*%s +%s =%s)", name, birth, death, wedding);
		else if (birth.IsEmpty() && death.IsEmpty())
			nameBD.Format(L"%s (=%s)", name, wedding);
	}



	return nameBD;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString pack(CString tag, CString place, CString date)
{
	CString str;
	str.Format(L"%s %s", place, date);
	str.Trim();
	if (!str.IsEmpty())
		str.Format(L"%s%s", tag, (CString)str);
	return str;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int getMonth(CString month)
{
	CStringArray M;
	M.Add(L"Jan");
	M.Add(L"Feb");
	M.Add(L"Mar");
	M.Add(L"Apr");
	M.Add(L"May");
	M.Add(L"Jun");
	M.Add(L"Jul");
	M.Add(L"Aug");
	M.Add(L"Sep");
	M.Add(L"Oct");
	M.Add(L"Nov");
	M.Add(L"Dec");
	int i;
	for (i = 0; i < M.GetSize(); ++i)
		if (M.GetAt(i) == month) break;
	return i + 1;
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// C O N V E R S I O N S //////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStringW Utf8ToUnicode(LPCSTR utf8)
{
	CStringW utf16(_T(""));
	if (!utf8)
		return utf16;
	DWORD dwNum = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);//Get the size of the string by setting the 4th parameter to -1
	WCHAR* unistring = new WCHAR[dwNum];//ebben taroljuk az unicode formatumot
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unistring, dwNum); // ANSI to UNICODE

	utf16 = CStringW(unistring);
	delete[] unistring;

	return utf16; //utf16 encoded string
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStringA UnicodeToUtf8(LPCWSTR unicode)
{
	CStringA utf8(_T(""));
	if (!unicode)
		return utf8;
	DWORD dwNum = WideCharToMultiByte(CP_UTF8, 0, unicode, -1, NULL, 0, NULL, NULL);// az utf8 string ennyi karakterbol fog allni.
	char* utf8string = new char[dwNum]; //ebben taroljuk az ansi stringet
	WideCharToMultiByte(CP_UTF8, 0, unicode, -1, utf8string, dwNum, 0, 0); // UNICODE to UTF-8


	utf8 = CStringA(utf8string);

	delete[] utf8string;
	return utf8; //utf8 encoded string
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStringW MultiToUnicode(LPCSTR multi)
{
	CStringW utf16(_T(""));
	if (!multi)
		return utf16;
	DWORD dwNum = MultiByteToWideChar(GetACP(), 0, multi, -1, NULL, 0);//Get the size of the string by setting the 4th parameter to -1
	WCHAR* unistring = new WCHAR[dwNum];//ebben taroljuk az unicode formatumot
	MultiByteToWideChar(GetACP(), 0, multi, -1, unistring, dwNum); // ANSI to UNICODE


	utf16 = CStringW(unistring);
	delete[] unistring;

	return utf16; //utf16 encoded string
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CStringA UnicodeToMulti(LPCWSTR unicode)
{
	CStringA utf8(_T(""));
	if (!unicode)
		return utf8;
	DWORD dwNum = WideCharToMultiByte(GetACP(), 0, unicode, -1, NULL, 0, NULL, NULL);// az utf8 string ennyi karakterbol fog allni.
	char* multistring = new char[dwNum]; //ebben taroljuk az ansi stringet
	WideCharToMultiByte(GetACP(), 0, unicode, -1, multistring, dwNum, 0, 0); // UNICODE to UTF-8

	CStringA multi(_T(""));
	multi = CStringA(multistring);

	delete[] multistring;
	return multi; //utf8 encoded string
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A megadott mappáben lévõ ext kiterjesztésû fájlok neveit a vFiles vektorba gyûjti.
void collectFiles(CString folder, CString ext, std::vector<CString>* vFiles)
{
	vFiles->clear();
	CString file;
	CString fileName;
	file.Format(L"%s\\*.%s", folder, ext);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(file);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		fileName = finder.GetFileName();
		vFiles->push_back(fileName);
	}
	finder.Close();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString lastWord(CString str)
{
	int pos;
	CString word(L"");
	if ((pos = str.ReverseFind(' ')) == -1) return word;
	word = str.Mid(pos + 1);
	return word;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString lastButOne(CString str)
{
	int pos;
	CString word(L"");

	if ((pos = str.ReverseFind(' ')) == -1) return word;
	str = str.Left(pos);
	if ((pos = str.ReverseFind(' ')) == -1) return word;
	word = str.Mid(pos + 1);
	return word;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USHORT rotate16(USHORT word)
{
	return(word >> 8 | word << 8);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT rotate32(UINT word)
{
	// 1234
	//         4000                0300                            0020                       0001  
	return(word << 24 | ((word << 16) >> 24) << 16 | ((word >> 16) << 24) >> 16 | word >> 24);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCHAR ascii_ebcdic(UCHAR ascii)
{
	unsigned char a2e[256] =
	{
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//         	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//         	
		0x40, 0x4F, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,	//  !"#$%&'	
		0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,	// ()*+,-./	
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,	// 01234567	
		0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,	// 89:;<=>?	
		0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,	// @ABCDEFG	
		0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,	// HIJKLMNO	
		0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,	// PQRSTUVW	
		0xE7, 0xE8, 0xE9, 0x4A, 0xE0, 0x5A, 0x5F, 0x6D,	// XYZ[\]^_	
		0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,	// `abcdefg	
		0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,	// hijklmno	
		0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,	// pqrstuvw	
		0xA7, 0xA8, 0xA9, 0xC0, 0x6A, 0xD0, 0xA1, 0x40,	// xyz{|}~ 	
		0xB9, 0xBA, 0xED, 0xBF, 0xBC, 0xBD, 0xEC, 0xFA,	//          
		0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xDA, 0xDB, 0xDC,	//        	
		0xDE, 0xDF, 0xEA, 0xEB, 0xBE, 0xCA, 0xBB, 0xFE,	//			
		0xFB, 0xFD, 0x7d, 0xEF, 0xEE, 0xFC, 0xB8, 0xDD,	//			
		0x77, 0x78, 0xAF, 0x8D, 0x8A, 0x8B, 0xAE, 0xB2,	//			
		0x8F, 0x90, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,	//			
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//	       	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//	       	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//	       	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//	       	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//	       	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//	       	
		0xAA, 0xAB, 0xAC, 0xAD, 0x8C, 0x8E, 0x80, 0xB6,	// ðñòóôõö÷	
		0xB3, 0xB5, 0xB7, 0xB1, 0xB0, 0xB4, 0x76, 0xA0,	// øùúûüýþÿ	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,	//        	
		0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40 	//        	
	};
	return a2e[ascii];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UCHAR ebcdic_ascii(UCHAR eb)
{
	unsigned char e2a[256] =
	{
		0x00, 0x01, 0x02, 0x03, 0x9c, 0x09, 0x86, 0x7f, // 00 
		0x97, 0x8d, 0x8e, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, // 08 
		0x10, 0x11, 0x12, 0x13, 0x9d, 0x85, 0x08, 0x87, // 10 
		0x18, 0x19, 0x92, 0x8f, 0x1c, 0x1d, 0x1e, 0x1f, // 18 
		0x80, 0x81, 0x82, 0x83, 0x84, 0x0a, 0x17, 0x1b, // 20 
		0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x05, 0x06, 0x07, // 28 
		0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04, // 30 
		0x98, 0x99, 0x9a, 0x9b, 0x14, 0x15, 0x9e, 0x1a, // 38 
		0x20, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, // 40 
		0xa7, 0xa8, 0x5b, 0x2e, 0x3c, 0x28, 0x2b, 0x21, // 48 
		0x26, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, // 50 
		0xb0, 0xb1, 0x5d, 0x24, 0x2a, 0x29, 0x3b, 0x5e, // 58 
		0x2d, 0x2f, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, // 60 
		0xb8, 0xb9, 0x7c, 0x2c, 0x25, 0x5f, 0x3e, 0x3f, // 68 
		0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1, // 70 
		0xc2, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22, // 78 
		0xc3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, // 80 
		0x68, 0x69, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, // 88 
		0xca, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, // 90 
		0x71, 0x72, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, // 98 
		0xd1, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, // a0 
		0x79, 0x7a, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, // a8 
		0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, // b0 
		0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, // b8 
		0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, // c0 
		0x48, 0x49, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, // c8 
		0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, // d0 
		0x51, 0x52, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3, // d8 
		0x5c, 0x9f, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, // e0 
		0x59, 0x5a, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, // e8 
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, // f0 
		0x38, 0x39, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  // f8 
	};
	return e2a[eb];
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                Convert EBCDIC to ASCII Character String                    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Call:                                                                      
//                                                                            
// ebc2asc(strout,strin,length,dis)                                      
//                                                                            
// strout     result string (EBCDIC)                                          
// strin      input string (ASCII)                                            
// length     length of string                                                
// dis        display option:                                                 
//            <true>   replace all non-printable characters by '.'            
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ebcdic2ascii(UCHAR* out, UCHAR* in, int length, int dis)
{
	int  i;
	for (i = 0; i < length; i++)
	{
		out[i] = ebcdic_ascii(in[i]);
		if (dis && (out[i] < 32 || out[i] >= 127))out[i] = '.';
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                Convert  ASCII to EBCDIC Character String                   
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ascii2ebcdic(UCHAR* out, UCHAR* in, int length, int dis)
{
	int  i;
	for (i = 0; i < length; i++)
	{
		out[i] = ascii_ebcdic(in[i]);
		if (dis && (in[i] < 32 || in[i] >= 127))out[i] = '.';
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Notes:
//Up to 3 bits lost on IEEE -> IBM
// endian = 0 for little endian machine, =1 for big endian machines
void ieee_to_ibm(int* from, int* to, int n, int endian)
{
	register int fconv, fmant, i, t;

	for (i = 0; i < n; ++i) {
		fconv = from[i];
		if (fconv) {
			fmant = (0x007fffff & fconv) | 0x00800000;
			t = (int)((0x7f800000 & fconv) >> 23) - 126;
			while (t & 0x3) { ++t; fmant >>= 1; }
			fconv = (0x80000000 & fconv) | (((t >> 2) + 64) << 24) | fmant;
		}
		if (endian == 0)
			fconv = (fconv << 24) | ((fconv >> 24) & 0xff) |
			((fconv & 0xff00) << 8) | ((fconv & 0xff0000) >> 8);

		to[i] = fconv;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ieee2ibm(UINT* ieee, int n)
{
	int i;
	int	tolas;
	int	iedik;
	unsigned int	sign;
	unsigned int	exponent;
	unsigned int	mantissza;
	unsigned int	ibmMantissza;
	unsigned int	ibmExponent;
	unsigned int	bit24 = 0X00800000;

	for (i = 0; i < n; ++i)
	{
		sign = ieee[i] & 0X80000000;
		exponent = (ieee[i] << 1) >> 24;
		mantissza = (ieee[i] << 9) >> 9;
		if (ieee[i])
		{
			iedik = 1 + exponent - 127 + 4 * 64;
			tolas = (4 - (iedik % 4)) % 4;
			ibmExponent = iedik / 4;
			ibmMantissza = bit24 + mantissza;
			if (tolas)
			{
				++ibmExponent;
				ibmMantissza >>= tolas;
			}
			ibmExponent <<= 24;
			ieee[i] = sign | ibmExponent | ibmMantissza;
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ibm2ieee(UINT* ibm, int n)
{
	int i;
	unsigned int fconv;  //floating number in ibm float format
	unsigned int fmant;  //mantissa in ibm then ieee floating format
	unsigned int exp;    //power of 2 in ieee floating format

	for (i = 0; i < n; ++i)
	{
		fconv = ibm[i];
		if (fconv)
		{
			fmant = fconv & 0x00FFFFFF;
			if (fmant == 0)
			{
				fconv = 0;
			}
			else
			{
				exp = ((fconv & 0x7F000000) >> 22) - 130;					//create power of 2 from power of 16
				while (!(fmant & 0x00800000)) { --exp; fmant <<= 1; }			//normalize mantissa from hexa to binary, adjust exponent
				if (exp > 0XFE)    fconv = (0x80000000 & fconv) | 0x7F7FFFFF;	//overflow! substitue by signed max value
				else if (exp <= 0) fconv = 0;									//underflow! substitute by 0
				else            fconv = (0x80000000 & fconv) | (exp << 23) | (0x007FFFFF & fmant);
			}
		}

		fconv = rotate32(fconv);
		ibm[i] = fconv;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString getWord(CString line, int n)
{
	for (int i = 1; i < n; ++i)
	{
		line = line.Right(line.GetLength() - line.Find(' ') - 1);
	}
	int at = line.Find(' ');
	if (at != -1)
		return(line.Left(at));
	else
		return(line);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void rotateBuffer32(UINT* out, UINT* in, int n)
{
	for (int i = 0; i < n; i++)
	{
		out[i] = rotate32(in[i]);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// buf-ban lévõ 'count' bájtból stringet képez 
char* hexs(char* buf, char* str)
{
	int i = 0;
	while (buf[i] != 0)
	{
		str[i] = buf[i];
	}
	str[i] = 0;
	return str;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CString current_time()
{
	time_t now = time(NULL);
	struct tm tstruct;
	char buf[40];
	localtime_s(&tstruct, &now);
	//format: HH:mm:ss
	strftime(buf, sizeof(buf), "%X", &tstruct);
	CString time;
	time = buf;
	time.Replace(':', '_');
	return time;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
CString copyN(byte* buff, int db)
{
	CString str;
	char* string = (char*)new char[db + 1];

	memcpy_s(string, db, buff, db);
	string[db] = 0;

	str = string;
	return str;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UINT get_uint(byte* buf, bool rotate)
{
	union
	{
		byte h[4];
		UINT x;
	}u;
	memcpy(&u.h, buf, 4);
	if (rotate)
		u.x = rotate32(u.x);
	return(u.x);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
USHORT get_ushort(byte* buf, bool rotate)
{
	union
	{
		byte h[2];
		USHORT x;
	} u;
	memcpy(&u.h, buf, 2);
	if (rotate) u.x = rotate16(u.x);
	return u.x;
}
