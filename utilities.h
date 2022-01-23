#pragma once
#include <string>
#include <sstream>
#include <vector>

#include <afxcoll.h>
#include <afxwin.h>


#define	ONE	TRUE			// for wordList
#define	MORE FALSE

#define OPEN TRUE
#define CLOSED FALSE

#define ANSI	0
#define UTF8	1
#define UTF8BOM 2


int		wordList(CStringArray* arrayP, CString line, char sep, BOOL single);
void	splitLine(CString line, wchar_t delim, bool emptyItem, std::vector<CString>* elems);
int		splitCString(CString cLine, TCHAR sep, bool emptyItem, CStringArray* A);
void	splitstring(const std::wstring& s, wchar_t delim, bool emptyItem, std::vector<std::wstring>* elems);
CString  getNRBD(CString rowid, CString name, CString birth, CString death);
CString upperToLower(CString str);

double Log2(double n);
TCHAR* thousend(unsigned _int64 val);
void splitFilespec(CString filespec, CString* drive, CString* path, CString* name, CString* ext);


CStringW Utf8ToUnicode(LPCSTR utf8);
CStringA UnicodeToUtf8(LPCWSTR unicode);
CStringW MultiToUnicode(LPCSTR multi);
CStringA UnicodeToMulti(LPCWSTR unicode);

CString		getString(int n);
double		Log2(double n);
TCHAR*		thousend(unsigned _int64 val);
void		splitFilespec(CString filespec, CString* drive, CString* path, CString* name, CString* ext);

CString		dropUntil(CString str, char ch);
CString		getUntil(CString str, char ch);
CString		dropLastWord(CString cLine);
CString		dropLastCharacter(CString cLine);
CString		dropFirstWord(CString cLine);
CString		getWord(CString str, int n, int* pos);
CString		getFirstWord(CString str);
CString		getSecondWord(CString str);
CString		getLastWord(CString str);
TCHAR		getLastCharacter(CString word);
CString		getTwoWords(CString str);
CString		sepFirstName(CString str);


//CString		date2date(CString date);
int			isDate(CStringArray* A, int i, CString* datum);
BOOL		isDateOK(CString datum);
BOOL		isNumeric(CString word);
BOOL		isRoman(CString word);
BOOL		isRoman(TCHAR kar);
BOOL		isNumber(CString word);
//BOOL		isValidBrace(CString brace);


CString		cleanCline(CString cLine);
CString		cleanHtmlLine(CString cLine);
CString		cleanHtmlTags(int lineNumber, CString cLine);
CString		packWords(CStringArray* A, int from, int db);
void		setCreationTime(CString filespec);



//bool operator<(const SAMEMARRIAGE &s1, const SAMEMARRIAGE &s2);
bool		isLastCharacter(CString str, TCHAR kar);


CString		firstUpper(CString name);
bool		checkDate(CString datum);

BOOL		isOdd(_int64 num);
BOOL		isEven(_int64 num);
int			GetInputCode(CString fileSpec);
bool		isUTF8(UCHAR* string, int length);
CStringA	AnsiToUtf8(CString str);
CString		Utf8ToAnsi(CString cLine);
int			getYearFromDate(CString date);
CString		getTimeTag();
bool		openFileSpec(FILE** ff, CString fileSpec, CString mode);
CString		roundDate(CString date);
int			diffD(CString date1, CString date2, int* pYear);
CString		getNameBD(CString name, CString birth, CString death, CString wedding);
CString		pack(CString tag, CString place, CString date);
int			getMonth(CString month);

void collectFiles(CString folder, CString ext, std::vector<CString>* vFiles);
CString lastWord(CString str);
CString lastButOne(CString str);
USHORT rotate16(USHORT word);
UINT rotate32(UINT word);
UCHAR ascii_ebcdic(UCHAR ascii);
UCHAR ebcdic_ascii(UCHAR eb);
void ebcdic2ascii(UCHAR* out, UCHAR* in, int length, int dis);
void ascii2ebcdic(UCHAR* out, UCHAR* in, int length, int dis);
void ieee_to_ibm(int* from, int* to, int n, int endian);
void ieee2ibm(UINT* ieee, int n);
void ibm2ieee(UINT* ibm, int n);
CString getWord(CString line, int n);
void rotateBuffer32(UINT* out, UINT* in, int n);
char* hexs(char* buf, char* str);
CString current_time();
CString copyN(byte* buff, int db);
UINT get_uint(byte* buf, bool rotate);
USHORT get_ushort(byte* buf, bool rotate);
CString getWordFrom(CString line, int pos);
CString getPresentDateTime();
int convertRomanToInt(CString roman);
int digit(TCHAR c);
bool isStringUpper(CString str);
int same(CString par1, CString par2);
