#pragma once
#include "assert.h"
#include "sqlitedb.h"
#include <Ws2tcpip.h>//socket functions hoz

//////////////////////////////////////////////////////////////////////
// Socket Stack Class (buffer a halozatos olvasas/iras adatainak)
//////////////////////////////////////////////////////////////////////

// a socket hibakodok meghatarozasa
#define SOCKET_STACK_OK							0
#define SOCKET_STACK_SEND_ERROR					1
#define SOCKET_STACK_RECV_HEADER_ERROR			2
#define SOCKET_STACK_RECV_BODY_ERROR			3
#define SOCKET_STACK_ACCEPT_ERROR				4
#define SOCKET_STACK_CONNECT_ERROR				5
#define SOCKET_STACK_TOO_BIG_DATA				6
#define SOCKET_STACK_SERVER_STUB_ERROR			7
#define SOCKET_STACK_NOT_OPEN					8
#define SOCKET_STACK_NO_RIGHTS					9
#define SOCKET_STACK_SOCKET_ERROR				100
#define SOCKET_STACK_NO_MORE_DATA				101
#define SOCKET_STACK_TOO_FEW_DATA				102
#define SOCKET_STACK_USER_ERROR_BASE			1000

#define SOCKET_STACK_BUFFER_SIZE 0x00100000//1MB network buffer. Ez a max adat, amit egyszerre kuldunk
#define SOCKET_STACK_HEADER_SIZE 5//integer+byte

class CSocketStack
{
private:
	char* m_buffBase;//a buffer adatainak kezdete (header utani cim)
	char* m_pPos;//buffer pozicio
	SOCKET nSock;//kapcsolt socket
	unsigned char contSign;//adat folytatodik jel (0 ha vege, 1, ha folytatodik)
	char* m_buffStart;//a buffer eleje
	int m_iToGet;//a hasznos buffer meret (olvasashoz)
	int m_err;

	void InitBuffer();//buffer letrehozas
	bool CheckSize(int iCheckLen);
	void SetHeader();
	void ReadHeader();
	int	SendBuffer();
	int	RecvBuffer(int iMaxLen);
	bool ContinueRecvMessage(int minSize);
	bool CheckSocket();

public:
	CSocketStack(SOCKET sock);
	virtual ~CSocketStack();

	void SetSocket(SOCKET cSock);
	void Reset(bool trueReset=0);
	bool WriteByte(unsigned char b);
	bool WriteInt(int i);
	bool WriteDouble(double i);
	bool WriteStr(const TCHAR* str );
//	bool WriteStr(TCHAR* str);
	bool WriteStrArray(TCHAR** strArray, int iLen);

	unsigned char ReadByte();
	int	ReadInt();
	double ReadDouble();
	TCHAR* ReadStr();
	TCHAR** ReadStrArray(int * iLen);

	int Invoke();
	int SendMessageBuffer();
	int RecvMessage();
	void FlushSocket();

	int GetErrorCode();
	static const TCHAR* GetErrorMessageStr(int iErr);
};

////////////////////////////////////////
///   CThread CLASS
////////////////////////////////////////

class CThread
{
private:
	unsigned long tid;//thread id. a thread inditashoz kell. masra nem hasznaljuk
	bool suspended;

public:
	HANDLE handle;//a thread handle-je
	bool autoDelete;

public:
	CThread();
	virtual ~CThread();
	
	void virtual Execute()=0;//ezt kell feluldeffinialni a thread valodi funkciojaval
	bool Create(bool susp=false);
	void SetAutoDelete();
	void Resume();
	void Suspend();
	void Kill();
	void Close();
	bool IsThreadAlive();
};

//////////////////////////////////////////////////////////////////////
// CSqliteServer Class
//////////////////////////////////////////////////////////////////////

typedef void (*SQLITE_SERVER_CALLBACK)(void*,int,SOCKET,LPCTSTR);//az sqliteserver socket es log uzeneteinek callback fuggveny tipusa
#define SQLITE_SERVER_DEFAULT_PORT 1278

class CSqliteServer : public CThread
{
private:
	UINT m_sPort;//server port
	SOCKET m_sockListen;
	bool m_bRunning;
	CUIntArray m_lstClientSockets;
	bool authMode;
	bool logging;
	CStdioFile logFile;
	bool CreateUserRightsDatabase();
	CString errorMsg;
	SQLITE_SERVER_CALLBACK callBackFunct;
	void* callerFunctPtr;

public:
	CSqliteServer();
	~CSqliteServer();

	bool Start(bool bSuspended=false);
	void Stop();
	bool SetPort(UINT sPort);
	UINT GetPort();
	bool IsListening() const { return m_bRunning; };
	void KillConnection(SOCKET sock);
	void KillConnections();
	void SetServerCallback(SQLITE_SERVER_CALLBACK cbFunct, void* pObj);
	void RemoveClientSocket(SOCKET sock);
	LPCTSTR GetLastError();// a legutolso hibajelzes
	bool IsLoggingEnable();
	void WriteToLog(LPCTSTR strLog);
	void SetLogging(bool mode);
	bool SetAuthMode(bool mode);
	bool GetAuthMode();
	int GetUserRights(CString user,CString pass, CString database, BYTE* rightsArray);
	int ProcessGrantExec(LPCTSTR grantCommand,LPCTSTR userName,BYTE* userRights);
	int ProcessGrantQuery(LPCTSTR userName, LPCTSTR grantCommand,CUIntArray* colTypes,CStringArray* colList,std::vector<TCHAR*> *resTable);

private:
	void Execute();
	void AddClientSocket(SOCKET sock);
	int ExRightNumByName(LPCTSTR permStr);
	int RightNumByName(LPCTSTR permNum);
	void RightNameByNum(int num, CStringArray* rightStrArray);
};

//////////////////////////////////////////////////////////////////////
// CRequestHandler Class A szerveroldali valaszok megadasa
//////////////////////////////////////////////////////////////////////

class CRequestHandler
{
public:
	CRequestHandler(CSqliteServer * pParentServer);
	~CRequestHandler();

	// Reads request data from stack and calls the individual method
	CString clientHostname, clientIp;

	int HandleRequest(SOCKET sock, CSocketStack * pStack);
	static int SqlAuthorizerCallback(void* callerObject,int sql_action_code,const char* c,const char* d,const char* e,const char* f);

private:
	BYTE m_Rights[33];
	CString connUser;
    
	CSqliteServer * m_pParentServer;
	CSqliteDB m_DB;//szerver adatbazis
	CSqliteDBRecordSet m_DBrec;//szerver recordset
};

//////////////////////////////////////////////////////////////////////
// CClientConection Class
//////////////////////////////////////////////////////////////////////

class CClientConnection
{
private:
	SOCKET m_sockfd;
	int m_iSize;

public:
	SOCKET Accept();
	void Close();

	CClientConnection(SOCKET sockfd);
	~CClientConnection();

public:
	SOCKET m_sockNew;
	CSocketStack* m_pStack;//ebben tarolodnak a bejovo uzenetek
	CSqliteServer * m_pParentServer;//megadasa kivulrol tortenik
};

//////////////////////////////////////////////////////////////////////
// CHandlerThread Class
//////////////////////////////////////////////////////////////////////

class CHandlerThread : public CThread
{
private:
	CClientConnection * m_pConnection;
	CSqliteServer * m_pParentServer;

public:
//	CHandlerThread();
	CHandlerThread(CClientConnection * pConnection);
	//CHandlerThread(CClientConnection * pConnection, CSqliteServer * pParentServer);
	void Execute();
};

////////////////////////////////////////
///   CSqliteDBEx CLASS
////////////////////////////////////////

class CSqliteDBEx :	public CSqliteDB
{
public:
	CSqliteDBEx(void);
	virtual ~CSqliteDBEx(void);
	bool Connect(LPCTSTR srvHostName, int srvPort, LPCTSTR dbUser, LPCTSTR dbPass, LPCTSTR strDBPathName = NULL, LPCTSTR strDBFilePass = NULL, int Flags=SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
	int ChangePass(LPCTSTR strNewPass);
	int Close();
	int Execute(LPCTSTR strQuery);
	bool IsNetworkConnected();
	CString GetLibversion();

private:
	bool BuildNetworkConnection(LPCTSTR hostName, int port);
	void AbortNetworkConnection(void);
	int SendOpenDatabase(LPCTSTR dbNamePath, LPCTSTR dbFilePass, int Flags);
	int SendDBAuthenticationData(LPCTSTR user, LPCTSTR pass, LPCTSTR database);
	int SendDBFileNewPass(LPCTSTR pass);

public:
	SOCKET mainSocket;

};

////////////////////////////////////////
///   CSqliteDBRecordSetEx CLASS
////////////////////////////////////////

class CSqliteDBRecordSetEx : public CSqliteDBRecordSet
{
public:
	CSqliteDBRecordSetEx(void);
	virtual ~CSqliteDBRecordSetEx(void);
	UINT Query(LPCTSTR strQueryString, void *ptrDB, int firstElement=0, int maxElement=INT_MAX);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// Global Functions //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// socket functions
//////////////////////////////////////////////////////////////////////

int socket_init();
int socket_uninit();
SOCKET socket_accept(SOCKET sockfd,struct sockaddr * sock_addr, int * sock_addr_length);
SOCKET socket_server(unsigned short port);
SOCKET socket_client(char* port, char* host, struct addrinfo **aiList);
SOCKET socket_client(WCHAR* port, WCHAR* host, struct addrinfo **aiList);
//void socket_free_addrinfo(struct addrinfo *aiList);
void socket_close(SOCKET sock);
wchar_t * chartowchar(char * instr);
char * wchartochar(wchar_t * instr);

//////////////////////////////////////////////////////////////////////
// Getting name and ip by socket
//////////////////////////////////////////////////////////////////////

CString getClientIpAddress(SOCKET sock);
CString getClientHostName(SOCKET sock);

//////////////////////////////////////////////////////////////////////
// String cryption functions
//////////////////////////////////////////////////////////////////////

void cryptString(CString *strText, bool cr);
void HashString(CString *strText);

//////////////////////////////////////////////////////////////////////
// Network communication constants
//////////////////////////////////////////////////////////////////////

typedef enum : unsigned char {
	METHOD_None = 0,//
	METHOD_auth,//
	METHOD_sqlite_open,//
	METHOD_sqlite_close,//
	METHOD_sqlite_changes,
	METHOD_sqlite_last_insert_rowid,
	METHOD_sqlite_interrupt,
	METHOD_sqlite_libversion,//
	METHOD_sqlite_exec,//
	METHOD_sqlite_get_table,
	METHOD_newpass_dbfile,//
	METHOD_sqlite_query//
} SQLiteMethodName;

