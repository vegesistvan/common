#include "StdAfx.h"
#include "SqliteDBEx.h"
#pragma warning( disable : 4996)

CSqliteDBEx::CSqliteDBEx(void)
{
	srand ((unsigned)time(NULL));//inicializaljuk a veletlenszam generatort
}

CSqliteDBEx::~CSqliteDBEx(void)
{
}

bool CSqliteDBEx::Connect(LPCTSTR srvHostName, int srvPort, LPCTSTR dbUser, LPCTSTR dbPass, LPCTSTR strDBPathName, LPCTSTR strDBFilePass, int Flags)//csatlakozas a szerverhez es az adatbazishoz
{
	AbortNetworkConnection();//ha esetleg letezik mar kapcsolat, bontjuk

	if(BuildNetworkConnection(srvHostName, srvPort))//letrehozzuk a kapcsolatot a szerverrel az adott porton
		return true;//a hibauzenetet az ErrMsg mar tartalmazza
	
	m_bConnected|=2;//network connected

	if(SendDBAuthenticationData(dbUser, dbPass, strDBPathName))//kuldjuk az adatbazis autentikacios adatokat
		return true;//a hibauzenetet az ErrMsg mar tartalmazza
	
	if(SendOpenDatabase(strDBPathName, strDBFilePass, Flags))//megnyitjuk a tavoli adatbazist
		return true;//a hibauzenetet az ErrMsg mar tartalmazza

	CString str=strDBPathName;
	if(str.Compare(_T(""))!=0)
		m_bConnected|= 1;//sikerult csatlakozni az adatbazishoz

	return false;
}

int CSqliteDBEx::ChangePass(LPCTSTR strNewPass)// adatbazisfajl jelszavanak megvaltoztatasa
{
	IsNetworkConnected();//ellenorizzuk a halozati kapcsolatot
	if (m_bConnected==3)//ha meg van nyitva az adatbazis
	{
		if(m_encryption)
		{
			int iRes=SendDBFileNewPass(strNewPass);//elkuldjuk az uj jelszot
			return iRes;//a hibauzenetet az ErrMsg mar tartalmazza
		}
		else
		{
			ErrMsg.Format(_T("Database Encryption Not Supported !"));
			return true;
		}
	}

	ErrMsg.Format(_T("Database not connected"));
	return true;
}

CString CSqliteDBEx::GetLibversion()
{
	IsNetworkConnected();// ellenorizzuk a halozati kapcsolatot
	if ((m_bConnected&2)!=2)//ha nincs kapcsolat a szserverrel
		return _T("Connection error");

	CSocketStack cStack(mainSocket);
	cStack.Reset();
	cStack.WriteByte(METHOD_sqlite_libversion);
	int iRes = (cStack.Invoke());//kuldjuk a parancsot

	if (iRes == SOCKET_STACK_OK)//ha megerkezett a valasz
	{
		iRes = cStack.ReadInt();//megnezzuk hogy van e hiba
		return cStack.ReadStr();
	}
	CString str;
	str.Format(_T("Socket error: %d"),iRes);// ha hiba tortenik
	return str;
}

int CSqliteDBEx::Close()// megnyitott adatbazis bezarasa
{
	IsNetworkConnected();// ellenorizzuk a halozati kapcsolatot
	if (!m_bConnected)//ha nem vagyunk csatlakozva az adatbazishoz
	{
		ErrMsg.Format(_T("Connection error"));
		return SOCKET_STACK_CONNECT_ERROR;
	}

	int iRes=0;
	if(m_bConnected==3)
	{
		CSocketStack cStack(mainSocket);
		cStack.Reset();
		cStack.WriteByte(METHOD_sqlite_close);
		iRes = (cStack.Invoke());//kuldjuk az adatbazis bezaro jelet

		if (iRes == SOCKET_STACK_OK)//ha megerkezett a valasz
		{
			iRes = cStack.ReadInt();//megnezzuk hogy van e hiba
			if(iRes)
				ErrMsg.Format(_T("%s"),cStack.ReadStr());//hiba eseten hibauzi
		}
		else
			ErrMsg.Format(_T("Socket error: %d"),iRes);
	}

	AbortNetworkConnection();//bontjuk a halozati kapcsolatot
	m_bConnected = 0;//nem vagyunk csatlakozva az adatbazishoz
	return iRes;
}

int CSqliteDBEx::Execute(LPCTSTR strQuery)  // sql parancs kiadasa adatbazis modositas celjabol. lekerdezes nem lehetseges
{
	IsNetworkConnected();//halozati kapcs ellenorzese
	if((m_bConnected&2)!=2)//ha nincs halozati kapcsolat
	{
		ErrMsg=_T("No connection to server");
		return 1;
	}
	
	CSocketStack cStack(mainSocket);
	cStack.Reset();
	cStack.WriteByte(METHOD_sqlite_exec);
	cStack.WriteStr((TCHAR*)strQuery);
	int iRes = (cStack.Invoke());//kuldjuk a kodot es a parancsot

	if (iRes == SOCKET_STACK_OK)// ha kaptunk valaszt
	{
		iRes = cStack.ReadInt();//megnezzuk, hogy van e hiba
		if (iRes)
			ErrMsg.Format(_T("%s"),cStack.ReadStr());//taroljuk a hibat
	}
	else
		ErrMsg.Format(_T("Socket error: %d"),iRes);

	return iRes;
}

bool CSqliteDBEx::IsNetworkConnected()//halozati kapcsolat ellenorzese
{
	int error=0, error_len=sizeof(error);	
	if(getsockopt(mainSocket, SOL_SOCKET, SO_ERROR, (char*) &error, &error_len )<0)
	{
		m_bConnected=0;//ha nem el a halozat, akkor az adatbazis sem elerheto
		return false;
	}
	return true;
}

bool CSqliteDBEx::BuildNetworkConnection(LPCTSTR hostName, int port)//halozati kapcs. letrehozasa
{
	CString strPort;
	struct addrinfo * m_pAddrInfo;
	strPort.Format(_T("%d"),port);

	mainSocket = socket_client(strPort.GetBuffer(),(LPTSTR)hostName,&m_pAddrInfo);//beallitjuk a client socket(kapcsolat azonosito)

	if (mainSocket == INVALID_SOCKET)//ha nem sikerult letrehozni
	{
		ErrMsg.Format(_T("Socket error. Can't access server"));
		return true;
	}

	while (m_pAddrInfo && m_pAddrInfo->ai_addr)
	{
		if (connect(mainSocket, m_pAddrInfo->ai_addr, (int)m_pAddrInfo->ai_addrlen)==0)//letrejon a kapcsolat
			break;
		else
			m_pAddrInfo=m_pAddrInfo->ai_next;//ezt nem tudom mi.
	}

	if (m_pAddrInfo==0)//ha nem sikerul kapcsolodni
	{
		AbortNetworkConnection();
		ErrMsg.Format(_T("Network error. Can't access server"));
		return true;
	}

	return false;
}

void CSqliteDBEx::AbortNetworkConnection(void)//halozati kapcsolat bontasa
{
	if (mainSocket!=INVALID_SOCKET)
	{
		socket_close(mainSocket);
		mainSocket=INVALID_SOCKET;
	}
	m_bConnected=0;
}

int CSqliteDBEx::SendOpenDatabase(LPCTSTR dbNamePath, LPCTSTR dbFilePass, int Flags)//tavoli adatbazis megnyitas
{
	if (!IsNetworkConnected())
		return SOCKET_STACK_CONNECT_ERROR;

	CString strPass=dbFilePass;
	cryptString(&strPass,true);

	CSocketStack cStack(mainSocket);
	cStack.Reset();
	cStack.WriteByte(METHOD_sqlite_open);
	cStack.WriteStr((TCHAR*)dbNamePath);
	cStack.WriteStr(strPass.GetBuffer());
	cStack.WriteInt(Flags);
	int iRes = (cStack.Invoke());

	if (iRes == SOCKET_STACK_OK)
	{
		iRes = cStack.ReadInt();
		if(iRes)
		{
			ErrMsg.Format(_T("%s"),cStack.ReadStr());
			AbortNetworkConnection();
		}
	}
	else
		ErrMsg.Format(_T("Socket error: %d"),iRes);

	return iRes;
}

int CSqliteDBEx::SendDBAuthenticationData(LPCTSTR user, LPCTSTR pass, LPCTSTR database)//adatbazis authentikacio kuldese
{
	if (!IsNetworkConnected())
		return SOCKET_STACK_CONNECT_ERROR;
	CString str=pass;
	cryptString(&str,true);//jelszo titkositasa
	pass=str;
	CSocketStack cStack(mainSocket);
	cStack.Reset();
	cStack.WriteByte(METHOD_auth);
	cStack.WriteStr((TCHAR*)user);
	cStack.WriteStr((TCHAR*)pass);
	cStack.WriteStr((TCHAR*)database);
	int iRes = (cStack.Invoke());

	if (iRes == SOCKET_STACK_OK)
	{
		iRes = cStack.ReadInt();
		if(iRes)
		{
			ErrMsg.Format(_T("%s"),cStack.ReadStr());
			AbortNetworkConnection();
		}
	}
	else
		ErrMsg.Format(_T("Socket error: %d"),iRes);

	return iRes;
}

int CSqliteDBEx::SendDBFileNewPass(LPCTSTR pass)//adatbazisfajl uj jelszo kuldese
{
	if (!IsNetworkConnected())
		return SOCKET_STACK_CONNECT_ERROR;

	CString str=pass;
	cryptString(&str,true);//jelszo titkositasa
	pass=str;
	CSocketStack cStack(mainSocket);
	cStack.Reset();
	cStack.WriteByte(METHOD_newpass_dbfile);
	cStack.WriteStr((TCHAR*)pass);
	int iRes = (cStack.Invoke());

	if (iRes == SOCKET_STACK_OK)
	{
		iRes = cStack.ReadInt();
		if(iRes)
			ErrMsg.Format(_T("%s"),cStack.ReadStr());
	}
	else
		ErrMsg.Format(_T("Socket error: %d"),iRes);

	return iRes;
}

////////////////////////////////////////
///   CSqliteDBRecordSetEx CLASS
////////////////////////////////////////

CSqliteDBRecordSetEx::CSqliteDBRecordSetEx(void)
{
}

CSqliteDBRecordSetEx::~CSqliteDBRecordSetEx(void)
{
}

UINT CSqliteDBRecordSetEx::Query(LPCTSTR strQueryString, void *ptrDB, int firstElement, int maxElement)// a rekordszet feltoltese sql paranccsal
{
	((CSqliteDBEx*)ptrDB)->IsNetworkConnected();//ellenorizzuk, hogy el e a halozati kapcsolat
	if (!((CSqliteDBEx*)ptrDB)->IsConnected())//ha nem vagyunk csatlakozva az adatbazishoz
	{
		AfxMessageBox(_T("No connection to database"));
		return RSQUERY_NOCONNECT;
	}

	if (!m_bIsEmpty)//ha a recordset nem ures
		Clear();//kiuritjuk

	CSocketStack cStack(((CSqliteDBEx*)ptrDB)->mainSocket);
	cStack.Reset();
	cStack.WriteByte(METHOD_sqlite_query);
	cStack.WriteStr((TCHAR*)strQueryString);
	cStack.WriteInt(firstElement);
	cStack.WriteInt(maxElement);
	int iRes = (cStack.Invoke());//kuldjuk az sql parancsot

	if (iRes == SOCKET_STACK_OK)
		iRes = cStack.ReadInt();//header
	else
	{
		ErrMsg.Format(_T("Socket error: %d"),iRes);
		return iRes;
	}

	if(iRes)//hiba eseten
	{
		ErrMsg=cStack.ReadStr();//taroljuk a hibauzenetet
		return iRes;
	}

	m_nRecordsCount=cStack.ReadInt();// a sorok szama
	m_nColumnsCount=cStack.ReadInt();//az oszlopok szama

	if(m_nColumnsCount)//ha van oszlop
	{
		m_bIsEmpty=false;//akkor nem ures a tabla
		for(int i=0;i<m_nColumnsCount;i++)
			strColumnsList.Add(cStack.ReadStr());
	}

/*	if(m_nColumnsCount)//ha van oszlop
	{
		m_bIsEmpty=false;//akkor nem ures a tabla
		cStack.ReadCStringArray(&strColumnsList);//oszlopnevek
	}*/

	if(m_nRecordsCount)
	{
		for(UINT i=0;i<ColumnsCount();i++)
			nColumnsType.Add(cStack.ReadInt());//oszlop tipusok

		for(int i=0;i<m_nRecordsCount*m_nColumnsCount;i++)//feltoltjuk a recordset adatait az orsTablaba
		{

//			CStringArray* tmpStrArray=new CStringArray;
//			cStack.ReadCStringArray(tmpStrArray);
//			orsTable.Add((CObject *)tmpStrArray);
			CString tmpStr=cStack.ReadStr();
//			double tmpDbl=cStack.ReadDouble();
			int tLen=tmpStr.GetLength()+1;
			LPTSTR sT=new TCHAR[tLen];
			_tcscpy_s(sT,tLen,tmpStr.GetBuffer());
			orsTable.push_back(sT);
		}
	}

	return iRes;
}

//////////////////////////////////////////////////////////////////////
// CThread Class 
// az osztaly orokitesevel es az execute fuggveny ujradeffinialasaval lehet uj threadet inditani
// a thread leirasa az execute fuggvenyben tortenik
// az uj threadet a create(false) inditja (true eseten suspended marad) ha nem adunk meg semmit, akkor false
// autoDelete eseten torlodik a cthread objektum mikor befejezodik (SetAutoDelete)
//////////////////////////////////////////////////////////////////////

DWORD WINAPI EP(LPVOID param)//ez egy threaden kivuli fuggveny, melyet a thread hiv meg. Ez inditja a threadet
//unsigned long _stdcall EP(CThread * param)
{
	CThread * thread = (CThread *) param;
	thread->Execute();//itt indul a thread fo fuggvenye
	CloseHandle(thread->handle);//itt leallitjuk a threadet
	thread->handle=0;//a kezelojet 0-ra allitjuk
	if(thread->autoDelete)//a client threadeket nem lehet becsukni, mert nincs veluk kapcsolat a letrehozas utan
		delete thread;//torli a thread objektumot
	return 0;
}

CThread::CThread()
{
	handle=0;
	suspended=false;
	autoDelete=false;
}

CThread::~CThread()
{
	Kill();
}

void CThread::SetAutoDelete()//ezzel allitjuk be az automatikus objektum torlest
{
	autoDelete=true;
}

bool CThread::Create(bool susp)//ez inditja a threadet
{
	suspended=susp;
	handle=CreateThread(NULL,0,EP,this,CREATE_SUSPENDED,&tid);
	if (suspended==false)//ha nem suspended modba inditjuk
		Resume();//akkor felelesztjuk
	return IsThreadAlive();
}

void CThread::Kill()//leallitja a threadet
{
	if (handle!=0)
	{
		if (WaitForSingleObject(handle, 3000)!=WAIT_OBJECT_0)//kicsit varakozunk, hogy a thread befejezze amit csinal
		{
			DWORD dwExit=0;
			GetExitCodeThread(handle, &dwExit);
			TerminateThread(handle,dwExit);
			Close();
		}
	}
}

void CThread::Resume()//ujrainditja a suspended threadet
{
	if (handle!=0)
		ResumeThread(handle);
}

void CThread::Suspend()//a futo threadet felfuggeszti
{
	if (handle!=0)
		SuspendThread(handle);
}

void CThread::Close()//a handle lezarasa
{
	if (handle!=0)
	{
		CloseHandle(handle);
		handle=0;
	}
}

bool CThread::IsThreadAlive()//aktiv -e a thread
{
	return (handle!=0);
}

//////////////////////////////////////////////////////////////////////////
// CRequestHandlerThread Class  Itt adjuk meg a szerver valaszreakcioit
// kapcsolatonkent egy jon letre belole
//////////////////////////////////////////////////////////////////////////

CRequestHandler::CRequestHandler(CSqliteServer * pParentServer)
{
	m_pParentServer=pParentServer;//a serverre mutat
	memset( m_Rights, 0, sizeof(m_Rights));
}

CRequestHandler::~CRequestHandler()
{
	if ((m_DB.IsConnected()&1)==1)
		m_DB.Close();
}

int CRequestHandler::SqlAuthorizerCallback(void* callerObject,int sql_action_code,const char* c,const char* d,const char* e,const char* f)
{	//ezt hivja meg az sqlite amikor az autorizaciot ellenorzi
	CRequestHandler* sourceObjRequestHandler=(CRequestHandler*) callerObject;
	if (sourceObjRequestHandler->m_Rights[sql_action_code+1]!=0)//ha van jogunk az adott feladathoz
		return SQLITE_OK;
	return SQLITE_DENY;//ha nincs
}

int CRequestHandler::HandleRequest(SOCKET sock, CSocketStack * pStack)//ez a lenyeg. Itt kezeljuk a kliens igenyeit
{
	CString logStr;
	logStr.Format(_T("%s %s user:"),clientHostname,clientIp);
	pStack->SetSocket(sock);
	int iRes = pStack->RecvMessage();//itt fogadjuk az adatokat, es feltoltjuk a buffert
	if (iRes == SOCKET_STACK_OK)//ha sikeresen megjott az info a halozaton
	{
		unsigned char Method = pStack->ReadByte();//beolvassuk a teendo kodjat
		try
		{
			switch (SQLiteMethodName(Method))
			{
			case METHOD_None://ha semmit nem csinalunk
				{
					pStack->Reset();
					pStack->WriteInt(SOCKET_STACK_OK);//kuldunk egy ok. valaszt
				}
				break;
			case METHOD_auth://adatbazis authentikacio erkezese eseten
				{
					CString in_user = pStack->ReadStr();//kiolvassuk az usert
					connUser=in_user;//a grant dolgoknel szukseg lesz ra ezert eltaroljuk
					CString in_pass = pStack->ReadStr();//a passwordot
					CString in_database = pStack->ReadStr();//az adatbazis nevet
					cryptString(&in_pass,false);//kikodoljuk a jelszot

					logStr.Append(connUser);
					logStr.Append(_T(" authentication "));

					int iRes=m_pParentServer->GetUserRights(in_user,in_pass,in_database,m_Rights);
					
					pStack->Reset();// Reset stack
					pStack->WriteInt(iRes);//visszakuldjuk az eredmenyt
					if (iRes)
					{
						pStack->WriteStr((TCHAR*)m_pParentServer->GetLastError());//hiba eseten a hiba szoveget is visszakuldjuk
						logStr.Append(_T("failed."));
					}
					else
						logStr.Append(_T("successful."));

					m_pParentServer->WriteToLog(logStr);//log kuldese
				} // end of case METHOD_auth:
				break;
			case METHOD_newpass_dbfile://adatbazis jelszo modositas eseten
				{
					logStr.Append(connUser);
					logStr.Append(_T(" modify password of database:"));
					logStr.Append(m_DB.GetDbName());
					TCHAR alma[] = L"Permission denied (chFilePass)";

					if ((m_Rights[0]&2)!=2 && (m_Rights[0]&8)!=8)//ha nincs irasi jogunk az adatbazishoz vagy nem vagyunk superuserek
					{
						pStack->Reset();
						pStack->WriteInt(SOCKET_STACK_NO_RIGHTS);
						pStack->WriteStr(_T("Permission denied (chFilePass)"));//hiba eseten a hiba szoveget is visszakuldjuk
					//	pStack->WriteStr( alma );//hiba eseten a hiba szoveget is visszakuldjuk
						logStr.Append(_T(" failed. (permission denied)"));
					}
					else
					{
						CString in_pass = pStack->ReadStr();//kiolvassuk a passwordot
						cryptString(&in_pass,false);//kikodoljuk a jelszot
						int iRes=m_DB.ChangePass(in_pass);

						pStack->Reset();
						pStack->WriteInt(iRes);//visszakuldjuk az eredmenyt
						if (iRes)
						{
							pStack->WriteStr((TCHAR*)m_pParentServer->GetLastError());
							logStr.Append(_T(" failed."));
						}
						else
							logStr.Append(_T(" succeed."));
					}
					m_pParentServer->WriteToLog(logStr);//log kuldese
				} // end of case METHOD_newpass_dbfile:
				break;
			case METHOD_sqlite_open://adatbazis megnyitasa. A valosagos megnyitast kesobb korlatozzuk
				{
					logStr.Append(connUser);

					CString in_filename = pStack->ReadStr();
					CString in_filepass = pStack->ReadStr();
					int in_mode = pStack->ReadInt();
					if(in_filepass.Compare(_T(""))!=0)//ha van jelszo
						cryptString(&in_filepass,false);//kikodoljuk a jelszot

					logStr.Append(_T(" open database:"));
					logStr.Append(in_filename);

					if(in_filename.Compare(_T(""))==0)//ha nem adtunk meg adatbazisnevet
					{
						if(m_pParentServer->GetAuthMode())//akkor ha aktiv az auth. mode
						{//akkor fontos, hogy adatbazis nev nelkul is hozzaferjunk a szerverhez, ezert ok.-t kuldunk vissza, de nem nyitunk meg adatbazist
							pStack->Reset();
							pStack->WriteInt(0);
							break;
						}
						else
						{
							TCHAR alma[] = L"Name of database is empty!";
							pStack->Reset();
							pStack->WriteInt(1);
							pStack->WriteStr(_T("Name of database is empty!"));
							//pStack->WriteStr( alma );

							logStr.Append(_T(" failed. Name of database is empty!"));
							m_pParentServer->WriteToLog(logStr);//log kuldese
							break;
						}
					}

					if(m_pParentServer->IsLoggingEnable() && in_filename.CompareNoCase(_T("server.log"))==0)
					{
						int iRes=1;
						pStack->Reset();
						pStack->WriteInt(iRes);
						TCHAR alma[] = L"You can't user server.log file as database";
						pStack->WriteStr(_T("You can't user server.log file as database"));
						//pStack->WriteStr( alma );
						
						logStr.Append(_T(" failed. Try to open server.log as database."));
						m_pParentServer->WriteToLog(logStr);//log kuldese
						break;
					}
					// Reduce rights according to user authorization
					if ((m_Rights[0]&2)==0 && (m_Rights[0]&8)!=8)//ha nincs irasi jogunk
					{
						if ((in_mode&SQLITE_OPEN_READWRITE)!=0)//es iras/olvasas modban akarjuk megnyitni
							in_mode-=SQLITE_OPEN_READWRITE;//akkor elvesszuk az iras/olvasas jogot
						if ((m_Rights[0]&1)!=0)//ha van jogunk olvasashoz
							in_mode|=SQLITE_OPEN_READONLY;//akkor ezt a jogot engedelyezzuk
					}
					if (m_pParentServer->GetAuthMode())//ha aktiv az authentikacio
						in_mode-=in_mode&SQLITE_OPEN_CREATE;//akkor itt nem crealhatunk, csak a grant parancsokkal

					if(in_mode!=6&&in_mode!=1&&in_mode!=2)//ha nincs jogunk vagy nem letezik az adatbazis
					{
						int iRes=1;
						pStack->Reset();
						pStack->WriteInt(iRes);
						TCHAR alma[] = L"Permission denied or database not exists (open)";
						pStack->WriteStr(_T("Permission denied or database not exists (open)"));
						//pStack->WriteStr( alma );
						
						logStr.Append(_T(" failed. (not exists or access denied)"));
						m_pParentServer->WriteToLog(logStr);//log kuldese
						break;
					}

					int iRes = m_DB.Connect(in_filename, in_filepass , in_mode);//megnyitjuk az adatbazist

					if (iRes==0 && m_pParentServer->GetAuthMode())//ha sikerult megnyitni, es az authorizacio aktiv
						m_DB.SetAuthorizer(&CRequestHandler::SqlAuthorizerCallback,this);

					pStack->Reset();
					pStack->WriteInt(iRes);//visszakuldjuk az eredmenyt
					if (iRes)
					{
						pStack->WriteStr((TCHAR*)m_DB.GetLastError());//hiba eseten a hiba szoveget is visszakuldjuk
						logStr.Append(_T(" failed."));
					}
					else
						logStr.Append(_T(" succeed."));
					m_pParentServer->WriteToLog(logStr);//log kuldese
				} // end of case METHOD_sqlite_open:
				break;
			case METHOD_sqlite_close://adatbazis bezarasa
				{
					logStr.Append(connUser);
					logStr.Append(_T(" close database:"));
					logStr.Append(m_DB.GetDbName());

					int iRes=0;
					if((m_DB.IsConnected()&1)==1)//ha meg van nyitva az adatbazis
						iRes = m_DB.Close();//bezarjuk az adatbazist

					pStack->Reset();
					pStack->WriteInt(iRes);//az eredmenyt visszakuldjuk
					if(iRes)
					{
						TCHAR alma[] = L"Error closing database";
						pStack->WriteStr(_T("Error closing database"));
						//pStack->WriteStr( alma );
						logStr.Append(_T(" failed."));
					}
					else
						logStr.Append(_T(" succeed."));

					m_pParentServer->WriteToLog(logStr);//log kuldese
				} // end of case METHOD_sqlite_close:
				break;
			case METHOD_sqlite_libversion://az sqlite lib verziojanak lekerdezese
				{
					CString lib_version=m_DB.GetLibversion();

					pStack->Reset();
					pStack->WriteInt(SOCKET_STACK_OK);//kuldunk egy ok. valaszt
					pStack->WriteStr(lib_version.GetBuffer());//kuldjuk a verziot

					logStr.Append(connUser);	
					logStr.Append(_T(" check version of sqlite library"));
					m_pParentServer->WriteToLog(logStr);//log kuldese
				} // end of case METHOD_sqlite_libversion:
				break;
			case METHOD_sqlite_exec://sql parancs futtatasa. Ezt akkor is engedelyezzuk, ha nincs kapcs. az adatbazissal, a grant parancsok miatt
				{
					logStr.Append(connUser);

					CString in_sql = pStack->ReadStr();//beolvassuk a parancsot

					int tmpTokenPos=0;
					if(in_sql.Tokenize(_T(" "),tmpTokenPos).CompareNoCase(_T("grant"))==0)//ha grant-el kezdodik
					{
						int iRes = m_pParentServer->ProcessGrantExec(in_sql,connUser,m_Rights);//megprobaljuk vegrehajtani a grant parancsot
						
						pStack->Reset();
						pStack->WriteInt(iRes);
						if (iRes)//hiba eseten visszakuldjuk a hibauzenetet
						{
							pStack->WriteStr((TCHAR*)m_pParentServer->GetLastError());
							logStr.Append(_T(" GRANT command failed."));
						}
						else
							logStr.Append(_T(" GRANT command succeed."));
						m_pParentServer->WriteToLog(logStr);//log kuldese
						break;
					}
						
					logStr.Append(_T(" Database:"));
					logStr.Append(m_DB.GetDbName());

					int iRes = m_DB.Execute(in_sql);// egyeb parancs eseten vegrehajtjuk az sql parancsot

					pStack->Reset();
					pStack->WriteInt(iRes);
					if (iRes)//hiba eseten visszakuldjuk a hibauzenetet
					{
						pStack->WriteStr((TCHAR*)m_DB.GetLastError());
						logStr.Append(_T(" SQL EXECUTE command failed."));
					}
					else
						logStr.Append(_T(" SQL EXECUTE command succeed."));
					m_pParentServer->WriteToLog(logStr);//log kuldese
				} // end of case METHOD_sqlite_exec:
				break;
			case METHOD_sqlite_query://lekerdezes. Ezt akkor is engedelyezzuk, ha nincs kapcs az adatbazissal a grant -ek miatt
				{
					logStr.Append(connUser);

					CString in_sql = pStack->ReadStr();//sql parancs
					int firstElement = pStack->ReadInt();//first element of query table
					int maxElement = pStack->ReadInt();//max num of elements of query table

					int tmpTokenPos=0;
					if(in_sql.Tokenize(_T(" "),tmpTokenPos).CompareNoCase(_T("grant"))==0)//grant parancs eseten
					{
						CUIntArray colTypes;// oszlop tipusok
						CStringArray colList; // oszlopnevek
						std::vector<TCHAR*> resTable;// reuslt table

						int iRes = m_pParentServer->ProcessGrantQuery(connUser,in_sql,&colTypes,&colList,&resTable);

						pStack->Reset();
						pStack->WriteInt(iRes);
						if (iRes)//hiba eseten visszakuldjuk a hibauzenetet
						{
							pStack->WriteStr((TCHAR*)m_pParentServer->GetLastError());
							logStr.Append(_T(" GRANT QUERY command failed."));
							m_pParentServer->WriteToLog(logStr);//log kuldese
						}
						else
						{
							logStr.Append(_T(" GRANT QUERY command succeed."));
							m_pParentServer->WriteToLog(logStr);//log kuldese

							pStack->WriteInt(resTable.size()/colList.GetSize());// a elemek szama
							pStack->WriteInt(colList.GetSize());//az oszlopok szama
							if(!colList.GetSize())//ha nincsenek oszlopok
								break;
							//pStack->WriteCStringArray(&colList);// az oszlopok nevei
							for(int i=0;i<colList.GetSize();i++)
							{
								CString sT=colList.GetAt(i);
								pStack->WriteStr(sT.GetBuffer());
							}

							if(resTable.size())//ha vannak sorok
								for(int i=0;i<colList.GetSize();i++)//oszlopok tipusai
									pStack->WriteInt(colTypes.GetAt(i));
							for(UINT i=0;i<resTable.size();i++)//tabla adatai
							{
								pStack->WriteStr(resTable[i]);
								delete resTable[i];
							}
						}
						break;
					}

					logStr.Append(_T(" Database:"));
					logStr.Append(m_DB.GetDbName());

					int iRes = m_DBrec.Query(in_sql,&m_DB,firstElement,maxElement);//sql parancs kuldese

					pStack->Reset();
					pStack->WriteInt(iRes);
					if (iRes)//hiba eseten hibauzi vissza
					{
						pStack->WriteStr((TCHAR*)m_DBrec.GetLastError());

						logStr.Append(_T(" SQL QUERY failed."));
						m_pParentServer->WriteToLog(logStr);//log k
						break;
					} 	 
					else//ha nincs hiba
					{
						logStr.Append(_T(" SQL QUERY succeed."));
						m_pParentServer->WriteToLog(logStr);//log k

						int recCount=m_DBrec.RecordsCount();
						int colCount=m_DBrec.ColumnsCount();
						CUIntArray* colTypes=m_DBrec._GetColTypeArray();
						std::vector<TCHAR*> *resTable=m_DBrec._GetResTableArray();

						pStack->WriteInt(recCount);// a sorok szama
						pStack->WriteInt(colCount);//az oszlopok szama

						if(!colCount)
							break;

						//pStack->WriteCStringArray(m_DBrec._GetColListArray());// az oszlopok nevei
						CStringArray* stA=m_DBrec._GetColListArray();
						for(int i=0;i<stA->GetSize();i++)
						{
							CString sT=stA->GetAt(i);
							pStack->WriteStr(sT.GetBuffer());
						}
						
						
						if(recCount)
							for(int i=0;i<colCount;i++)//oszlopok tipusai
								pStack->WriteInt(colTypes->GetAt(i));
						for(int i=0;i<recCount*colCount;i++)//tabla adatai
						{
							pStack->WriteStr(resTable->operator [](i));
							//pStack->WriteDouble(resTable->operator [](i).fValue);
						}
					}
				} // end of case METHOD_sqlite_get_table:
				break;
			default:
				return SOCKET_STACK_TOO_BIG_DATA;//rossz adat eseten
			} // end of case
		} // end of try
		catch(...)//ha valami hiba tortent
		{
			// Something went wrong
			pStack->Reset();
			pStack->WriteInt(SOCKET_STACK_SERVER_STUB_ERROR);
			TCHAR alma[] = L"Unknown error";
			pStack->WriteStr(_T("Unknown error"));
			//pStack->WriteStr( alma );
		}
	}
	else
	{
		// Message broken. Try to send error back.
		return iRes;
	}
	return pStack->SendMessageBuffer();//itt kuldjuk vissza az adatokat
}

//////////////////////////////////////////////////////////////////////
// CHandlerThread Class  Ez rendeli ossze a kezelot es a klienseket. thread osztalybol szarmaztattuk. Minden kapcsolathoz kell egy ilyen.
//////////////////////////////////////////////////////////////////////

//CHandlerThread::CHandlerThread(CClientConnection * pConnection, CSqliteServer * pParentServer)
CHandlerThread::CHandlerThread(CClientConnection * pConnection)// a parentServer a client connectionbol meghatarozhato
{
	//m_pParentServer=pParentServer;
	m_pConnection=pConnection;
	m_pParentServer=pConnection->m_pParentServer;
}

void CHandlerThread::Execute()//ezt inditja a thread
{
	if (m_pConnection==0)
		return;
	else
	{
		CRequestHandler* pHandler = new CRequestHandler(m_pParentServer);//itt hozzuk letre a szerver valasz kezelo osztalyt
		int iRes = 0;
		CString cIp=getClientIpAddress(m_pConnection->m_sockNew);//meghatarozzuk a kliens ip cimet
		CString cHn=getClientHostName(m_pConnection->m_sockNew);//meghatarozzuk a kliens hostnevet
		pHandler->clientIp=cIp;
		pHandler->clientHostname=cHn;

		CString logStr;
		logStr.Format(_T("%s %s client connected."),cHn,cIp);
		m_pParentServer->WriteToLog(logStr);

		while(m_pConnection->m_sockNew) // While connected
		{
			iRes = pHandler->HandleRequest(m_pConnection->m_sockNew,m_pConnection->m_pStack);// itt inditjuk a valasz kezelo fuggvenyt
			if (iRes != SOCKET_STACK_OK)//hiba eseten
				m_pConnection->Close();//bontjuk a kapcsolatot

		}//ha vege a kapcsolatnak
		delete m_pConnection;//toroljuk a kliens objektumot
		delete pHandler;//toroljuk a valaszado objektumot

		logStr.Format(_T("%s %s client disconnected."),cHn,cIp);
		m_pParentServer->WriteToLog(logStr);
	}
}

//////////////////////////////////////////////////////////////////////
// CClientConnection Class //ebbe az osztalyba taroljuk a bejovo kapcsolatot. 1 kapcsolat, 1 objektum
//////////////////////////////////////////////////////////////////////

CClientConnection::CClientConnection(SOCKET sockfd)//sockfd a server socket
{
	m_sockfd=sockfd;
	m_sockNew = 0;
	m_iSize = sizeof(struct sockaddr_in);
	m_pStack = new CSocketStack(INVALID_SOCKET);
	m_pParentServer=0;
}

CClientConnection::~CClientConnection()
{
	Close();
	delete m_pStack;
}

SOCKET CClientConnection::Accept()//m_sockNew tarolja az uj bejovo kapcsolatot
{
	struct sockaddr_in m_addrRemote;
	m_sockNew = socket_accept(m_sockfd,(sockaddr*)&m_addrRemote,&m_iSize);
	return m_sockNew;
}

void CClientConnection::Close()//kapcsolat bontasa. az m_pParentServer megadasa kivulrol tortenik.
{
	if (m_sockNew)
	{
		socket_close(m_sockNew);
		if (m_pParentServer)
			m_pParentServer->RemoveClientSocket(m_sockNew);
	}
	m_sockNew=0;
	m_pStack->Reset();
}

//////////////////////////////////////////////////////////////////////
// CSqliteServer Class  Ez a szerver osztaly.
//////////////////////////////////////////////////////////////////////

CSqliteServer::CSqliteServer()
{
	m_sPort=SQLITE_SERVER_DEFAULT_PORT;
	m_sockListen=0;
	m_bRunning=false;
	socket_init();//ez kell, hogy menjen a halozat
	authMode=false;
	logging=false;
	callBackFunct=0;
}

CSqliteServer::~CSqliteServer()
{
	if (m_bRunning)
		Kill();
	socket_uninit();
}

bool CSqliteServer::Start(bool bSuspended/*=false*/)//ez inditja a szervert
{
	if (m_bRunning)//ha mar fut
		return false;
	if(Create(bSuspended))//ezz hozza letre a threadet es inditja az execute fuggvenyt
		return true;//ha sikerul elinditani
	else
	{
		m_bRunning=false;
		return false;
	}
}

void CSqliteServer::Execute()//ez a server fo fuggvenye
{
	m_sockListen = socket_server(m_sPort);//server socket letrehozasa
	if (m_sockListen == INVALID_SOCKET)//ha nem sikerult
	{
		errorMsg=_T("Invalid socket");
		return;
	}
	
 	CClientConnection* pConnection;
 	CHandlerThread*    pHandler;

	m_bRunning = true;//beallitjuk a futas jelet

	CString logStr;
	logStr.Format(_T("Server started. Listening on port:%d"),m_sPort);
	WriteToLog(logStr);

	while (m_bRunning)// amig fut a server
	{
 		pConnection = new CClientConnection(m_sockListen);//letrehozunk egy uj cliens objektumot
 		if (pConnection->Accept() != INVALID_SOCKET)//amint kapcsolodik valaki a szerverhez
 		{
			pConnection->m_pParentServer=this;//megadjuk a cliens objektumnak ezt a szervert parentservernek
			AddClientSocket(pConnection->m_sockNew);//taroljuk a kapcsolatot
			pHandler = new CHandlerThread(pConnection);//letrehozunk egy kliens-kezelo osszerendelo objektumot
			pHandler->SetAutoDelete();//autodeletere allitjuk, mert ha egyszer elindult nem lesz vele kapcsolatunk
 			if (!pHandler->Create())//itt inditjuk az osszerendelo objektumot. Ha nem jon letre
			{
				delete pConnection;//toroljuk a kliens kapcsolatot
				pConnection=0;
			}
 		}
 		else//ha nem jon letre a halozati kapcsolat
 		{
 			delete pConnection;//toroljuk a kliens objektumot
			if (m_sockListen==0)
				m_bRunning=false;
 		}
		// The thread deletes the connection, no need to worry
	}

	if (m_sockListen)//ha a szserver figyel
		socket_close(m_sockListen);//leallitjuk a figyelest
	m_sockListen=0;
	m_bRunning=false;
}

void CSqliteServer::Stop()//itt allitjuk le a szervert
{
	CString logStr;
	if (m_bRunning) 
	{
		logStr.Format(_T("Server stopped."));
		WriteToLog(logStr);
		socket_close(m_sockListen);
		m_sockListen=0;
		m_bRunning=false;
		CThread::Kill();//leallitjuk a threadet is		
	}
}

bool CSqliteServer::SetPort(UINT sPort)//port beallitasa
{
	if (m_bRunning)
		return false;
	else
	{
		m_sPort=sPort;
		return true;
	}
}

UINT CSqliteServer::GetPort()//port lekerese
{
	return m_sPort;
}

void CSqliteServer::KillConnection(SOCKET sock)//egy kapcsolat kilovese
{
//	CString logStr;
//	logStr.Format(_T("%s %s client connection dropped."),getClientHostName(sock),getClientIpAddress(sock));
	socket_close(sock);
//	WriteToLog(logStr);
}

void CSqliteServer::KillConnections()//osszes kapcsolat kilovese
{
	for(int i=m_lstClientSockets.GetCount();i>0;i--)
		KillConnection(m_lstClientSockets.GetAt(i-1));
/*	while(m_lstClientSockets.GetCount())
	{
		SOCKET sock=m_lstClientSockets.GetAt(0);
		if(callBackFunct)
			callBackFunct(callerFunctPtr,0,sock,_T(""));
		KillConnection(sock);
		m_lstClientSockets.RemoveAt(0);
	}*/
}

void CSqliteServer::SetServerCallback(SQLITE_SERVER_CALLBACK cbFunct, void* pObj)
{
	callBackFunct=cbFunct;
	callerFunctPtr=pObj;
}

void CSqliteServer::AddClientSocket(SOCKET sock)//kapcsolat tarolasa
{
	m_lstClientSockets.Add(sock);
	if(callBackFunct)
		callBackFunct(callerFunctPtr,1,sock,_T(""));
}

void CSqliteServer::RemoveClientSocket(SOCKET sock)//kapcsolat eltavolitasa a listabol
{
	for (int i=0;i<m_lstClientSockets.GetCount();i++)
	{
		if(sock==m_lstClientSockets.GetAt(i))
		{
			m_lstClientSockets.RemoveAt(i);
			if(callBackFunct)
				callBackFunct(callerFunctPtr,0,sock,_T(""));
			break;
		}
	}
}

void CSqliteServer::WriteToLog(LPCTSTR strLog)
{
	if(!logging)
		return;
	CString logTxt;
	SYSTEMTIME st;
	GetSystemTime(&st);
	logTxt.Format(_T("%d.%.2d.%.2d %.2d:%.2d:%.2d  %s\n"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,strLog);
	logFile.WriteString(logTxt);

	if(callBackFunct)
		callBackFunct(callerFunctPtr,2,0,logTxt);
}

void CSqliteServer::SetLogging(bool mode)
{
	logging=mode;
	if(logging && (logFile.m_hFile == CFile::hFileNull))
	{
		CFileException e;
		if(!logFile.Open(_T("server.log"), CFile::modeCreate | CFile::modeReadWrite | CFile::modeNoTruncate, &e))
		{
			AfxMessageBox(_T("Error opening logfile. Logging disabled."));
			logging=false;
			return;	
		}
		logFile.SeekToEnd();
	}
}

bool CSqliteServer::IsLoggingEnable()
{
	return logging;
}

bool CSqliteServer::SetAuthMode(bool mode)
{
	if (m_bRunning)
		return false;
	else
	{
		authMode=mode;
		return true;
	}
}

bool CSqliteServer::GetAuthMode()
{
	return authMode;
}

LPCTSTR CSqliteServer::GetLastError()// a legutolso hibajelzes
{
	return errorMsg;
}

bool CSqliteServer::CreateUserRightsDatabase()
{
	CSqliteDB db;
	CString str,strPass=_T("password");//default admin pass
	HashString(&strPass);

	if(db.Connect(_T("security.sqlite"),_T("sqliteServerAuthDatabasePassword")))//ha nem lehet letrehozni az adatbazist
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		return true;
	}
	if (db.Execute (_T("CREATE TABLE users (Name TEXT, Pass TEXT)")) != 0)
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		db.Close();
		return true;
	}
	if (db.Execute (_T("CREATE TABLE rights (UserId INT, Database TEXT, Permissions TEXT)")) != 0)
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		db.Close();
		return true;
	}
	if (db.Execute (_T("CREATE TABLE extrights (UserId INT, Permissions TEXT)")) != 0)
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		db.Close();
		return true;
	}
	str.Format(_T("INSERT INTO users VALUES ('admin','%s')"),strPass);
	if (db.Execute (str) != 0)
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		db.Close();
		return true;
	}
	if (db.Execute (_T("INSERT INTO extrights VALUES ('1','12')")) != 0)
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		db.Close();
		return true;
	}
	if (db.Execute (_T("INSERT INTO rights VALUES ('1','security.sqlite','3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1')")) != 0)
	{
		errorMsg.Format(_T("%s"),db.GetLastError());
		db.Close();
		return true;
	}
	db.Close();	
	return false;
}

int CSqliteServer::GetUserRights(CString user,CString pass,CString database, BYTE* rightsArray)
{
	if(!authMode)//ha auth mode kikapcsolva
	{
		rightsArray[0]=8;//akkor superuserek vagyunk
		return 0;
	}

	CSqliteDB db;
	if(db.Connect(_T("security.sqlite"),_T("sqliteServerAuthDatabasePassword"),SQLITE_OPEN_READWRITE))//ha nem lehet megnyitni az adatbazist
	{
		if(CreateUserRightsDatabase())//ha nem jon letre az adatbazis
			return 1;

		if(db.Connect(_T("security.sqlite"),_T("sqliteServerAuthDatabasePassword")))
		{
			errorMsg.Format(_T("%s"),db.GetLastError());
			return 1;
		}
	}
		
	CSqliteDBRecordSet recordSet;
	CString str;
	HashString(&pass);
	str.Format(_T("SELECT rowid FROM users WHERE Name='%s' AND Pass='%s'"),user,pass);
	recordSet.Query(str,&db);
	if(recordSet.RecordsCount()==0)
	{
		errorMsg.Format(_T("Bad username of password"));
		db.Close();
		return 1;
	}

	int userId=_tstoi(recordSet.GetFieldString(0));
	BYTE extRight=0;//alap, nincs extra jogunk (create, manager)
	
	str.Format(_T("SELECT Permissions FROM extrights WHERE UserId='%d'"),userId);
	recordSet.Query(str,&db);
	if(recordSet.RecordsCount())
		extRight=_tstoi(recordSet.GetFieldString(0));

	str.Format(_T("SELECT Permissions FROM rights WHERE UserId='%d' AND Database='%s'"),userId,database);
	recordSet.Query(str,&db);

	if(recordSet.RecordsCount())
	{
		str=recordSet.GetFieldString(0);
		int tmpTokenPos=0;
		for(int i=0;i<33;i++)
		{
			int tmp=_tstoi(str.Tokenize(_T(","),tmpTokenPos));
			rightsArray[i]=char(tmp);
		}
	}
	
	rightsArray[0]=rightsArray[0]|extRight;
	if((extRight&8)==8)//ha szuperuserek vagyunk, akkor mindenhez van jogunk
	{
		for(int i=1;i<33;i++)
			rightsArray[i]=char(1);
	}
	
	db.Close();
	return 0;
}

int CSqliteServer::ExRightNumByName(LPCTSTR permStr)
{
	CString str=permStr;
	if(str.CompareNoCase(_T("createdatabase"))==0)
		return 4;
	if(str.CompareNoCase(_T("superuser"))==0)
		return 8;

	return -1;
}

int CSqliteServer::RightNumByName(LPCTSTR permStr)
{
	CString str=permStr;
	
	if(str.CompareNoCase(_T("SQLITE_COPY"))==0)
		return 0;
	if(str.CompareNoCase(_T("SQLITE_CREATE_INDEX"))==0)
		return 1;
	if(str.CompareNoCase(_T("SQLITE_CREATE_TABLE"))==0)
		return 2;
	if(str.CompareNoCase(_T("SQLITE_CREATE_TEMP_INDEX"))==0)
		return 3;
	if(str.CompareNoCase(_T("SQLITE_CREATE_TEMP_TABLE"))==0)
		return 4;
	if(str.CompareNoCase(_T("SQLITE_CREATE_TEMP_TRIGGER"))==0)
		return 5;
	if(str.CompareNoCase(_T("SQLITE_CREATE_TEMP_VIEW"))==0)
		return 6;
	if(str.CompareNoCase(_T("SQLITE_CREATE_TRIGGER"))==0)
		return 7;
	if(str.CompareNoCase(_T("SQLITE_CREATE_VIEW"))==0)
		return 8;
	if(str.CompareNoCase(_T("SQLITE_DELETE"))==0)
		return 9;
	if(str.CompareNoCase(_T("SQLITE_DROP_INDEX"))==0)
		return 10;
	if(str.CompareNoCase(_T("SQLITE_DROP_TABLE"))==0)
		return 11;
	if(str.CompareNoCase(_T("SQLITE_DROP_TEMP_INDEX"))==0)
		return 12;
	if(str.CompareNoCase(_T("SQLITE_DROP_TEMP_TABLE"))==0)
		return 13;
	if(str.CompareNoCase(_T("SQLITE_DROP_TEMP_TRIGGER"))==0)
		return 14;
	if(str.CompareNoCase(_T("SQLITE_DROP_TEMP_VIEW"))==0)
		return 15;
	if(str.CompareNoCase(_T("SQLITE_DROP_TRIGGER"))==0)
		return 16;
	if(str.CompareNoCase(_T("SQLITE_DROP_VIEW"))==0)
		return 17;
	if(str.CompareNoCase(_T("SQLITE_INSERT"))==0)
		return 18;
	if(str.CompareNoCase(_T("SQLITE_PRAGMA"))==0)
		return 19;
	if(str.CompareNoCase(_T("SQLITE_READ"))==0)
		return 20;
	if(str.CompareNoCase(_T("SQLITE_SELECT"))==0)
		return 21;
	if(str.CompareNoCase(_T("SQLITE_TRANSACTION"))==0)
		return 22;
	if(str.CompareNoCase(_T("SQLITE_UPDATE"))==0)
		return 23;
	if(str.CompareNoCase(_T("SQLITE_ATTACH"))==0)
		return 24;
	if(str.CompareNoCase(_T("SQLITE_DETACH"))==0)
		return 25;
	if(str.CompareNoCase(_T("SQLITE_ALTER_TABLE"))==0)
		return 26;
	if(str.CompareNoCase(_T("SQLITE_REINDEX"))==0)
		return 27;
	if(str.CompareNoCase(_T("SQLITE_ANALYZE"))==0)
		return 28;
	if(str.CompareNoCase(_T("SQLITE_CREATE_VTABLE"))==0)
		return 29;
	if(str.CompareNoCase(_T("SQLITE_DROP_VTABLE"))==0)
		return 30;
	if(str.CompareNoCase(_T("SQLITE_FUNCTION"))==0)
		return 31;

	if(str.CompareNoCase(_T("readonly"))==0)
		return 80;
	if(str.CompareNoCase(_T("readwrite"))==0)
		return 90;
	if(str.CompareNoCase(_T("sqlite_all"))==0)
		return 100;

	return -1;
}

void CSqliteServer::RightNameByNum(int num, CStringArray* rightStrArray)
{
	if (num>=100)
	{
		num-=100;
		if((num&8)==8)
			rightStrArray->Add(_T("superuser"));
		if((num&4)==4)
			rightStrArray->Add(_T("createdatabase"));
		if((num&1)==1)
			rightStrArray->Add(_T("readonly"));
		if((num&2)==2)
			rightStrArray->Add(_T("readwrite"));		
		if((num&16)==16)
			rightStrArray->Add(_T("sqlite_all"));
		return;
	}
	if(num==0)
		rightStrArray->Add(_T("SQLITE_COPY"));
	if(num==1)
		rightStrArray->Add(_T("SQLITE_CREATE_INDEX"));
	if(num==2)
		rightStrArray->Add(_T("SQLITE_CREATE_TABLE"));
	if(num==3)
		rightStrArray->Add(_T("SQLITE_CREATE_TEMP_INDEX"));
	if(num==4)
		rightStrArray->Add(_T("SQLITE_CREATE_TEMP_TABLE"));
	if(num==5)
		rightStrArray->Add(_T("SQLITE_CREATE_TEMP_TRIGGER"));
	if(num==6)
		rightStrArray->Add(_T("SQLITE_CREATE_TEMP_VIEW"));
	if(num==7)
		rightStrArray->Add(_T("SQLITE_CREATE_TRIGGER"));
	if(num==8)
		rightStrArray->Add(_T("SQLITE_CREATE_VIEW"));
	if(num==9)
		rightStrArray->Add(_T("SQLITE_DELETE"));
	if(num==10)
		rightStrArray->Add(_T("SQLITE_DROP_INDEX"));
	if(num==11)
		rightStrArray->Add(_T("SQLITE_DROP_TABLE"));
	if(num==12)
		rightStrArray->Add(_T("SQLITE_DROP_TEMP_INDEX"));
	if(num==13)
		rightStrArray->Add(_T("SQLITE_DROP_TEMP_TABLE"));
	if(num==14)
		rightStrArray->Add(_T("SQLITE_DROP_TEMP_TRIGGER"));
	if(num==15)
		rightStrArray->Add(_T("SQLITE_DROP_TEMP_VIEW"));
	if(num==16)
		rightStrArray->Add(_T("SQLITE_DROP_TRIGGER"));
	if(num==17)
		rightStrArray->Add(_T("SQLITE_DROP_VIEW"));
	if(num==18)
		rightStrArray->Add(_T("SQLITE_INSERT"));
	if(num==19)
		rightStrArray->Add(_T("SQLITE_PRAGMA"));
	if(num==20)
		rightStrArray->Add(_T("SQLITE_READ"));
	if(num==21)
		rightStrArray->Add(_T("SQLITE_SELECT"));
	if(num==22)
		rightStrArray->Add(_T("SQLITE_TRANSACTION"));
	if(num==23)
		rightStrArray->Add(_T("SQLITE_UPDATE"));
	if(num==24)
		rightStrArray->Add(_T("SQLITE_ATTACH"));
	if(num==25)
		rightStrArray->Add(_T("SQLITE_DETACH"));
	if(num==26)
		rightStrArray->Add(_T("SQLITE_ALTER_TABLE"));
	if(num==27)
		rightStrArray->Add(_T("SQLITE_REINDEX"));
	if(num==28)
		rightStrArray->Add(_T("SQLITE_ANALYZE"));
	if(num==29)
		rightStrArray->Add(_T("SQLITE_CREATE_VTABLE"));
	if(num==30)
		rightStrArray->Add(_T("SQLITE_DROP_VTABLE"));
	if(num==31)
		rightStrArray->Add(_T("SQLITE_FUNCTION"));
}

int CSqliteServer::ProcessGrantQuery(LPCTSTR userName,LPCTSTR grantCommand,CUIntArray* colTypes,CStringArray* colList,std::vector<TCHAR*> *resTable)
{
	if(!GetAuthMode())//ha nem aktiv az authentication 
	{
		errorMsg=_T("Auth mode not active");
		return 1;
	}

	CString strGrant=grantCommand;
	CString str;
	CStringArray cmdString;

	int tokenPos = 0;
	while (true)//szetbontjuk a sztringet parameterekre
	{
		str = strGrant.Tokenize(_T(" "), tokenPos);
		if(str.Compare(_T(""))==0)
			break;
		cmdString.Add(str);
	}

	CSqliteDB db;
	CSqliteDBRecordSet rs;
	if(db.Connect(_T("security.sqlite"),_T("sqliteServerAuthDatabasePassword"),SQLITE_OPEN_READWRITE))//ha nem lehet megnyitni az adatbazist
	{
		errorMsg.Format(_T("Error opening security database"));
		return 1;
	}
//GETCURRENTUSER
	if(cmdString.GetAt(1).CompareNoCase(_T("getcurrentuser"))==0)
	{
		if(cmdString.GetCount()!=2)//ha nem stimmel a parameter szam
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		
		colTypes->Add(SQLITE_TEXT);
		colList->Add(_T("Name"));
//		CStringArray* tmpArray = new CStringArray;
//		tmpArray->Add(userName);
//		resTable->Add(tmpArray);

		
		int tLen=_tcslen(userName)+1;
		LPTSTR sT=new TCHAR[tLen];
		_tcscpy_s(sT,tLen,userName);
		resTable->push_back(sT);




		return 0;
	}
//GETUSERS
	if(cmdString.GetAt(1).CompareNoCase(_T("getusers"))==0)
	{
		if(cmdString.GetCount()!=2)//ha nem stimmel a parameter szam
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}

		str.Format(_T("SELECT Name, group_concat(Database,', ') AS Databases FROM users,rights WHERE users.rowid=rights.UserId GROUP BY users.rowid UNION SELECT Name, '' FROM users WHERE rowid NOT IN (SELECT UserId FROM rights)  ORDER BY users.Name"));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		CStringArray tmpArray;
		for(UINT i=0;i<rs.RecordsCount();i++)
		{
			//CStringArray* tmpArray = new CStringArray;
			for(UINT z=0;z<rs.ColumnsCount();z++)
			{
				//tmpArray->Add(rs.GetFieldString(z));
				int tLen=_tcslen(rs.GetFieldString(z))+1;
				LPTSTR sT=new TCHAR[tLen];
				_tcscpy_s(sT,tLen,rs.GetFieldString(z));
				resTable->push_back(sT);

				if(i==0)
				{
					colTypes->Add(rs.GetColumnType(z));//tipus tomb feltoltese
					colList->Add(rs.GetColumnName(z));//oszlopnev tomb feltoltese
				}
			}
			//resTable->Add(tmpArray);
			rs.MoveNext();
		}

		return 0;
	}
//GETDATABASES
	if(cmdString.GetAt(1).CompareNoCase(_T("getdatabases"))==0)
	{
		if(cmdString.GetCount()!=2)//ha nem stimmel a parameter szam
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}

		str.Format(_T("SELECT distinct(Database), group_concat(Name,', ') AS Users FROM rights,users WHERE Database!='security.sqlite' AND users.rowid=rights.UserId ORDER BY Database"));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}

		CStringArray tmpArray;
		for(UINT i=0;i<rs.RecordsCount();i++)
		{
			//CStringArray* tmpArray = new CStringArray;
			for(UINT z=0;z<rs.ColumnsCount();z++)
			{
				//tmpArray->Add(rs.GetFieldString(z));
				int tLen=_tcslen(rs.GetFieldString(z))+1;
				LPTSTR sT=new TCHAR[tLen];
				_tcscpy_s(sT,tLen,rs.GetFieldString(z));
				resTable->push_back(sT);
				
				if(i==0)
				{
					colTypes->Add(rs.GetColumnType(z));//tipus tomb feltoltese
					colList->Add(rs.GetColumnName(z));//oszlopnev tomb feltoltese
				}
			}
			//resTable->Add(tmpArray);
			rs.MoveNext();
		}

		return 0;
	}
//GETDBUSERS
	if(cmdString.GetAt(1).CompareNoCase(_T("getdbusers"))==0)
	{
		if(cmdString.GetCount()!=3)//ha nem stimmel a parameter szam
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT Database FROM rights WHERE Database='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("Database %s does not exists"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT Name FROM users,rights WHERE users.rowid=rights.UserId AND Database='%s' ORDER BY Name"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}

		CStringArray tmpArray;
		for(UINT i=0;i<rs.RecordsCount();i++)
		{
			//CStringArray* tmpArray = new CStringArray;
			for(UINT z=0;z<rs.ColumnsCount();z++)
			{
				//tmpArray->Add(rs.GetFieldString(z));
				int tLen=_tcslen(rs.GetFieldString(z))+1;
				LPTSTR sT=new TCHAR[tLen];
				_tcscpy_s(sT,tLen,rs.GetFieldString(z));
				resTable->push_back(sT);

				if(i==0)
				{
					colTypes->Add(rs.GetColumnType(z));//tipus tomb feltoltese
					colList->Add(rs.GetColumnName(z));//oszlopnev tomb feltoltese
				}
			}
			//resTable->Add(tmpArray);
			rs.MoveNext();
		}

		return 0;
	}
//GETUSERRIGHTS
	if(cmdString.GetAt(1).CompareNoCase(_T("getuserrights"))==0)
	{
		if(cmdString.GetCount()!=3 && cmdString.GetCount()!=4)//ha nem stimmel a parameter szam
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		if(cmdString.GetCount()==4)
		{
			str.Format(_T("SELECT Database FROM rights WHERE Database='%s'"),cmdString.GetAt(3));
			if(rs.Query(str,&db))
			{
				errorMsg.Format(_T("%s"),rs.GetLastError());
				db.Close();
				return 1;
			}
			if(!rs.RecordsCount())
			{
				errorMsg.Format(_T("Database %s does not exists"),cmdString.GetAt(3));
				db.Close();
				return 1;
			}
		}
		else
			cmdString.Add(_T(""));//ha nincs adatbazis megadva

		str.Format(_T("SELECT Name FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("User %s does not exists"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}

		BYTE tmpPerm [33];
		memset( tmpPerm, 0, sizeof(tmpPerm));
		
		str.Format(_T("SELECT Permissions FROM rights,users WHERE rights.UserId=users.rowid AND Name='%s' AND Database='%s'"),cmdString.GetAt(2),cmdString.GetAt(3));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(rs.RecordsCount())
		{
			CString tmpRights=rs.GetFieldString(0);
			int tmpTokenPos=0;
			for(int i=0;i<33;i++)
				tmpPerm[i]=_tstoi(tmpRights.Tokenize(_T(","),tmpTokenPos));
		}

		str.Format(_T("SELECT Permissions FROM extrights,users WHERE extrights.UserId=users.rowid AND Name='%s'"),cmdString.GetAt(2));
		rs.Query(str,&db);
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}

		if(rs.RecordsCount())
			tmpPerm[0]|=_tstoi(rs.GetFieldString(0));

		CStringArray tmpPermsArray;
		RightNameByNum(tmpPerm[0]+100,&tmpPermsArray);
		bool allsql=true;
		for(int i=1;i<33;i++)
			if(!tmpPerm[i])
			{
				allsql=false;
				break;
			}
		if(allsql)
			RightNameByNum(116,&tmpPermsArray);
		else
		{
			for(int i=1;i<33;i++)
			if(tmpPerm[i])
				RightNameByNum(i-1,&tmpPermsArray);
		}

		colTypes->Add(SQLITE_TEXT);//tipus tomb feltoltese
		colList->Add(_T("Permission"));//oszlopnev tomb feltoltese
		
		CStringArray tmpArray;
		for(int i=0;i<tmpPermsArray.GetCount();i++)
		{
			//CStringArray* tmpArray = new CStringArray;
			//tmpArray->Add(tmpPermsArray.GetAt(i));
			//resTable->Add(tmpArray);
			int tLen=_tcslen(tmpPermsArray.GetAt(i))+1;
			LPTSTR sT=new TCHAR[tLen];
			_tcscpy_s(sT,tLen,tmpPermsArray.GetAt(i));
			resTable->push_back(sT);
		}
		return 0;
	}

	errorMsg.Format(_T("Bad grant command"));//ha nem stimmel a grant parancs
	return 1;
}

int CSqliteServer::ProcessGrantExec(LPCTSTR grantCommand,LPCTSTR userName, BYTE* userRights)
{
	if(!GetAuthMode())//ha nem aktiv az authentication 
	{
		errorMsg=_T("Auth mode not active");
		return 1;
	}

	CString strGrant=grantCommand;
	CString str;
	CStringArray cmdString;

	int tokenPos = 0;
	while (true)//szetbontjuk a sztringet parameterekre
	{
		str = strGrant.Tokenize(_T(" "), tokenPos);
		if(str.Compare(_T(""))==0)
			break;
		cmdString.Add(str);
	}

	CString trueUser, truePass;
	CSqliteDB db;
	CSqliteDBRecordSet rs;
	if(db.Connect(_T("security.sqlite"),_T("sqliteServerAuthDatabasePassword"),SQLITE_OPEN_READWRITE))//ha nem lehet megnyitni az adatbazist
	{
		errorMsg.Format(_T("Error opening security database"));
		return 1;
	}
//CREATEUSER
	if(cmdString.GetAt(1).CompareNoCase(_T("createuser"))==0)
	{
		int num=0,exRights=0;

		if(cmdString.GetCount()<4)//ha nem stimmel a parameter szam
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		if((userRights[0]&8)!=8)//ha nem vagyunk szuperuserek
		{
			errorMsg.Format(_T("No right to create user"));
			db.Close();
			return 1;
		}
		for(int i=4;i<cmdString.GetCount();i++)//beallitjuk a jogokat
		{
			int num=(ExRightNumByName(cmdString.GetAt(i)));
			if (num<0)
			{
				errorMsg.Format(_T("Bad permission parameter"));
				db.Close();
				return 1;
			}
			exRights|=num;
		}
		str.Format(_T("SELECT * FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(rs.RecordsCount())
		{
			errorMsg.Format(_T("User %s is alredy exists"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		truePass=cmdString.GetAt(3);
		HashString(&truePass);
		str.Format(_T("INSERT INTO users VALUES ('%s','%s')"),cmdString.GetAt(2),truePass);
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. User %s not created"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT rowid FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s. User not created"),rs.GetLastError());
			db.Close();
			return 1;
		}
		str.Format(_T("INSERT INTO extrights VALUES ('%s','%d')"),rs.GetFieldString(0),exRights);
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. User %s rights was not applied. Remove this user please and create again"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		return 0;
	}
//DELETEUSER
	if(cmdString.GetAt(1).CompareNoCase(_T("deleteuser"))==0)
	{
		if(cmdString.GetCount()!=3)//ha nem harom parameter van
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		if((userRights[0]&8)!=8)//ha nem vagyunk szuperuserek
		{
			errorMsg.Format(_T("No right to delete user"));
			db.Close();
			return 1;
		}
		if(cmdString.GetAt(2).CompareNoCase(_T("admin"))==0)//admint nem lehet torolni
		{
			errorMsg.Format(_T("Can't remove admin user"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT rowid FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s. User not deleted"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())//ha nincs ilyen user
		{
			errorMsg.Format(_T("User %s is not exists"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}

		str.Format(_T("DELETE FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. User %s not deleted"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		str.Format(_T("DELETE FROM extrights WHERE UserId='%s'"),rs.GetFieldString(0));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. The exrights of user: %s not deleted. Security database is corrupt"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		str.Format(_T("DELETE FROM rights WHERE UserId='%s'"),rs.GetFieldString(0));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. The rights of user: %s not deleted. Security database is corrupt"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		return 0;
	}
//CHANGEPASS
	if(cmdString.GetAt(1).CompareNoCase(_T("changepass"))==0)
	{
		if(cmdString.GetCount()!=3 && cmdString.GetCount()!=4)//ha nem harom vagy 4 parameter van
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		if(cmdString.GetCount()==4 && cmdString.GetAt(2).CompareNoCase(userName)!=0 && (userRights[0]&8)!=8)//ha van usernev es jelszo is, es nem a sajat usernevunket hasznaljuk, es nem vagyunk szuperuserek
		{
			errorMsg.Format(_T("No right to modify pass of user %s"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		if(cmdString.GetCount()==4)//ha 4 parameter van
		{
			trueUser.Format(_T("%s"),cmdString.GetAt(2));
			truePass.Format(_T("%s"),cmdString.GetAt(3));
		}
		else//ha 3 parameter van
		{
			trueUser.Format(_T("%s"),userName);
			truePass.Format(_T("%s"),cmdString.GetAt(2));
		}
		HashString(&truePass);

		str.Format(_T("SELECT * FROM users WHERE Name='%s'"),trueUser);
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s. Password not changed"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())//ha nincs ilyen user
		{
			errorMsg.Format(_T("User %s is not exists"),trueUser);
			db.Close();
			return 1;
		}
		str.Format(_T("UPDATE users SET Pass='%s' WHERE Name='%s'"),truePass,trueUser);
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. Password not changed of user: %s"),db.GetLastError(),trueUser);
			db.Close();
			return 1;
		}
		return 0;
	}
//SETDBACCESS
	if(cmdString.GetAt(1).CompareNoCase(_T("setdbaccess"))==0)
	{
		if(cmdString.GetCount()<5)//ha kevesebb mint 5 parameter van
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT * FROM rights WHERE Database='%s'"),cmdString.GetAt(3));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("Database: %s does not exists"),cmdString.GetAt(3));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT Name FROM users,(SELECT min(rowid),UserId FROM rights WHERE Database='%s') AS perm WHERE users.rowid=perm.UserId"),cmdString.GetAt(3));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		CString tmpStr=rs.GetFieldString(0);

		if((userRights[0]&8)!=8 && tmpStr.Compare(userName)!=0 )// ha nem vagyunk szuperuserek es nem mi vagyunk a tulajdonosok
		{
			errorMsg.Format(_T("No permission to modify access to database %s"),cmdString.GetAt(3));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT rowid FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))//ha hiba a lekerdezesben
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("No user name: %s"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}

		BYTE rightArray [33];
		int Right;
		memset( rightArray, 0, sizeof(rightArray));
		for (int i=4;i<cmdString.GetCount();i++)
		{
			Right=RightNumByName(cmdString.GetAt(i));
			if (Right>=0 && Right < 40)
				rightArray[Right+1]=1;
			else if (Right==80)//readonly
				rightArray[0]|=1;
			else if (Right==90)//readwrite
				rightArray[0]|=2;
			else if (Right==100)//all sql right
				for (int z=1;z<33;z++)
					rightArray[z]=1;
		}
		CString rStr=_T(""),numStr;
		for (int i=0;i<33;i++)
		{
			numStr.Format(_T("%d,"),rightArray[i]);
			rStr.Append(numStr);
		}
		rStr.Delete(rStr.GetLength()-1);//toroljuk az utolso vesszot
		
		str.Format(_T("DELETE FROM rights WHERE UserId='%s' AND Database='%s'"),rs.GetFieldString(0),cmdString.GetAt(3));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. Error removing pervious rights"),db.GetLastError());
			db.Close();
			return 1;
		}

		str.Format(_T("INSERT INTO rights VALUES ('%s','%s','%s')"),rs.GetFieldString(0),cmdString.GetAt(3),rStr);
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. Error setting db rights"),db.GetLastError());
			db.Close();
			return 1;
		}
		return 0;
	}
//DELETEDBACCESS
	if(cmdString.GetAt(1).CompareNoCase(_T("deletedbaccess"))==0)
	{
		if(cmdString.GetCount()!=4)//ha nem 4 parameter van
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT * FROM rights WHERE Database='%s'"),cmdString.GetAt(3));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("Database: %s does not exists"),cmdString.GetAt(3));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT Name FROM users,(SELECT min(rowid),UserId FROM rights WHERE Database='%s') AS perm WHERE users.rowid=perm.UserId"),cmdString.GetAt(3));
		if(rs.Query(str,&db))//user aki letrehozta az adatbazist
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		CString tmpStr=rs.GetFieldString(0);//user aki letrehozta az adatbazist

		if((userRights[0]&8)!=8 && tmpStr.Compare(userName)!=0 )// ha nem vagyunk szuperuserek es nem mi vagyunk a tulajdonosok
		{
			errorMsg.Format(_T("No permission to modify database access %s"),cmdString.GetAt(3));
			db.Close();
			return 1;
		}
		if(tmpStr.Compare(cmdString.GetAt(2))==0)//ha a tulajdonos akarja torolni sajat magat az adatbazis hozzaferesbol
		{
			errorMsg.Format(_T("Can't remove the access of creator"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT users.rowid FROM users,rights WHERE users.Name='%s' AND users.rowid=rights.UserId AND rights.Database='%s'"),cmdString.GetAt(2),cmdString.GetAt(3));
		if(rs.Query(str,&db))//ha hiba a lekerdezesben
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("User: %s has no right to database: %s"),cmdString.GetAt(2),cmdString.GetAt(3));
			db.Close();
			return 1;
		}

		str.Format(_T("DELETE FROM rights WHERE UserId='%s' AND Database='%s'"),rs.GetFieldString(0),cmdString.GetAt(3));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. Error removing user database access"),db.GetLastError());
			db.Close();
			return 1;
		}
		return 0;
	}
//CREATEDATABASE
	if(cmdString.GetAt(1).CompareNoCase(_T("createdatabase"))==0)
	{
		if(cmdString.GetCount()!=3)//ha nem 3 parameter van
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		if((userRights[0]&8)!=8 && (userRights[0]&4)!=4)// ha nem crealhatunk
		{
			errorMsg.Format(_T("No permission to create database"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT * FROM rights WHERE Database='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))//ha hiba a lekerdezesben 
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(rs.RecordsCount())
		{
			errorMsg.Format(_T("Database %s is already exists"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}

		if(cmdString.GetAt(2).FindOneOf(_T(":\\/,*&%$"))!=-1)
		{
			errorMsg.Format(_T("Name of database must not contain special characters"));
			db.Close();
			return 1;
		}

		CFile f;
		CFileException e;
		if(!f.Open(cmdString.GetAt(2), CFile::modeCreate | CFile::modeWrite, &e))
		{
			errorMsg.Format(_T("Can't create db file: %s"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}

		str.Format(_T("SELECT rowid FROM users WHERE Name='%s'"),userName);
		if(rs.Query(str,&db))//ha hiba a lekerdezesben
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("No user name: %s"),userName);//ez ugye nem lehetseges
			db.Close();
			return 1;
		}
		str.Format(_T("INSERT INTO rights VALUES ('%s','%s','3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1')"),rs.GetFieldString(0),cmdString.GetAt(2));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. Error setting db rights"),db.GetLastError());
			db.Close();
			return 1;
		}
		return 0;
	}
//SETPERMISSIONS
	if(cmdString.GetAt(1).CompareNoCase(_T("setpermissions"))==0)
	{
		if(cmdString.GetCount()<3)//ha kevesebb mint 3
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		if((userRights[0]&8)!=8)// ha su vagyunk
		{
			errorMsg.Format(_T("No permission to modify user extra rights"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT rowid FROM users WHERE Name='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))//ha hiba a lekerdezesben
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("No user name: %s"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}

		int exRights=0;
		for(int i=3;i<cmdString.GetCount();i++)
		{
			int num=(ExRightNumByName(cmdString.GetAt(i)));
			if (num<0)
			{
				errorMsg.Format(_T("Bad permission parameter"));
				db.Close();
				return 1;
			}
			exRights|=num;
		}
		str.Format(_T("UPDATE extrights SET Permissions='%d' WHERE UserId='%s'"),exRights,rs.GetFieldString(0));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. User %s rights was not applied"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		return 0;
	}
//DROPDATABASE
	if(cmdString.GetAt(1).CompareNoCase(_T("dropdatabase"))==0)
	{
		if(cmdString.GetCount()!=3)//ha nem 3 parameter van
		{
			errorMsg.Format(_T("Bad parameters"));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT * FROM rights WHERE Database='%s'"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		if(!rs.RecordsCount())
		{
			errorMsg.Format(_T("Database: %s does not exists"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		str.Format(_T("SELECT Name FROM users,(SELECT min(rowid),UserId FROM rights WHERE Database='%s') AS perm WHERE users.rowid=perm.UserId"),cmdString.GetAt(2));
		if(rs.Query(str,&db))
		{
			errorMsg.Format(_T("%s"),rs.GetLastError());
			db.Close();
			return 1;
		}
		CString tmpStr=rs.GetFieldString(0);

		if((userRights[0]&8)!=8 && tmpStr.Compare(userName)!=0 )// ha nem vagyunk szuperuserek es nem mi vagyunk a tulajdonosok
		{
			errorMsg.Format(_T("No permission to remove database %s"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		if(!DeleteFile(cmdString.GetAt(2)))//ha nem tudjuk fizikailag torolni
		{
			errorMsg.Format(_T("Error removing database file: %s. Maybe it is in use"),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		str.Format(_T("DELETE FROM rights WHERE Database='%s'"),cmdString.GetAt(2));
		if(db.Execute(str))
		{
			errorMsg.Format(_T("%s. Database %s not deleted"),db.GetLastError(),cmdString.GetAt(2));
			db.Close();
			return 1;
		}
		return 0;
	}

	errorMsg.Format(_T("Bad grant command"));
	return 1;
}

//////////////////////////////////////////////////////////////////////
// Socket Stack Class (buffer a halozatos olvasas/iras adatainak)
//////////////////////////////////////////////////////////////////////

CSocketStack::CSocketStack(SOCKET sock)
{
	InitBuffer();
	nSock=sock;
	contSign=0;
	m_iToGet=0;
	m_err=0;
}

CSocketStack::~CSocketStack()
{
	delete [] m_buffStart;//toroljuk az adattarolo buffert
}

void CSocketStack::InitBuffer()
{
	m_buffStart = new char[SOCKET_STACK_BUFFER_SIZE+SOCKET_STACK_HEADER_SIZE];
	m_buffBase = m_buffStart+SOCKET_STACK_HEADER_SIZE;//a header utani cim.
	m_pPos  = m_buffBase;//a bufferen beluli pozicio (az elejere allitjuk)
}

void CSocketStack::Reset(bool trueReset)
{
	m_err=0;
	if(trueReset)
		m_pPos=m_buffStart;//pozicio atallitasa az elejere (header ele)
	else
		m_pPos = m_buffBase;//pozicio atalitasa az elejere (header utan)
}

void CSocketStack::SetSocket(SOCKET sock)
{
	nSock=sock;
}

bool CSocketStack::CheckSocket()
{
	if(nSock==INVALID_SOCKET)
	{
		m_err=SOCKET_STACK_SOCKET_ERROR;
		return true;
	}
	return false;
}

bool CSocketStack::CheckSize(int iCheckLen)//buffer meret ellenorzes, hogy belefer -e a beirando adat (csak writenal)
{
	if(iCheckLen>SOCKET_STACK_BUFFER_SIZE)
	{
		m_err=SOCKET_STACK_TOO_BIG_DATA;
		return true;
	}
	if(m_pPos-m_buffBase+iCheckLen>SOCKET_STACK_BUFFER_SIZE)
	{
		contSign=1;
		if(SendMessageBuffer())
			return true;
		Reset();
	}
	return false;
}

bool CSocketStack::WriteByte(unsigned char b)//byte irasa a bufferbe
{
	if(CheckSize(1))//meret ellenorzes
		return true;
	*m_pPos = b;//adat beiras
	m_pPos++;//pozicio noveles
	return false;
}

bool CSocketStack::WriteInt(int i)//integer irasa a bufferbe
{
	if(CheckSize(sizeof(int)))//meret ellenorzes/noveles
		return true;
	*((int*)m_pPos) = i;//adat beirasa
	m_pPos+=sizeof(int);//pozicio mozgatasa
	return false;
}

bool CSocketStack::WriteDouble(double i)//integer irasa a bufferbe
{
	if(CheckSize(sizeof(double)))//meret ellenorzes/noveles
		return true;
	*((double*)m_pPos) = i;//adat beirasa
	m_pPos+=sizeof(double);//pozicio mozgatasa
	return false;
}

//bool CSocketStack::WriteStr(TCHAR* str)//char* irasa a bufferbe
bool CSocketStack::WriteStr( const TCHAR* str )//char* irasa a bufferbe
{
	int iLen = (int)_tcslen(str)+1;//a karakterek szama a lezaro 0-val egyutt

	if (str == 0)//ha ures a string
	{
		if(WriteByte(0))//egy 0-t irunk
			return true;
		return false;
	}
	if (iLen<254)//ha a karakterek szama < 254
	{
		if(WriteByte(iLen))//akkor kiirjuk
			return true;
	}
	else//kulonben
	{
		if(WriteByte(255))//255-ot irunk, majd utanna a meretet int-kent.
			return true;
		if(WriteInt(iLen))
			return true;
	}

	if(CheckSize(iLen*sizeof(TCHAR)))//byte meret alapjan ellenorzunk
		return true;

	memcpy(m_pPos,str,iLen*sizeof(TCHAR));//beleirjuk az adatokat
	m_pPos+=iLen*sizeof(TCHAR);//noveljuk a poziciot
	return false;
}

bool CSocketStack::WriteStrArray(TCHAR **strArray, int iLen)//char* tomb irasa a bufferbe
{
	if(WriteInt(iLen))//kiirjuk a tomb meretet
		return true;
	for (int i=0;i<iLen;i++)
	{
		if(WriteStr(strArray[i]))//majd a stringeket
			return true;
	}
	return false;
}

bool CSocketStack::ContinueRecvMessage(int minSize)
{
	if(!contSign)//ha nem folytatodik az adatfolyam
	{
		m_err=SOCKET_STACK_NO_MORE_DATA;
		return true;
	}
	if(RecvMessage())
		return true;
	int recSize=m_iToGet;
	if(recSize<minSize)
	{
		m_err=SOCKET_STACK_TOO_FEW_DATA;
		return true;
	}
	return false;
}

unsigned char CSocketStack::ReadByte()//bajt olvasasa a bufferbol a m_pPos -tol
{
	if(m_pPos > m_buffBase+m_iToGet-1)
		if(ContinueRecvMessage(1))
			return 0;
	unsigned char c = *m_pPos;
	m_pPos++;
	return c;
}

int CSocketStack::ReadInt()//int olvasasa a bufferbol a m_pPos -tol
{
	if(m_pPos > m_buffBase+m_iToGet-sizeof(int))
		if(ContinueRecvMessage(sizeof(int)))
			return 0;
	int iRes = *((int*)m_pPos);//a bufferbol az adott poziciotol olvasunk egy int tipust
	m_pPos+=sizeof(int);//tovabblepunk a bufferben az olvasott merettel
	return iRes;
}

double CSocketStack::ReadDouble()//int olvasasa a bufferbol a m_pPos -tol
{
	if(m_pPos > m_buffBase+m_iToGet-sizeof(double))
		if(ContinueRecvMessage(sizeof(double)))
			return 0;
	double iRes = *((double*)m_pPos);//a bufferbol az adott poziciotol olvasunk egy int tipust
	m_pPos+=sizeof(double);//tovabblepunk a bufferben az olvasott merettel
	return iRes;
}

TCHAR* CSocketStack::ReadStr()//egy stringet olvasunk (char*)
{
	unsigned char bLen = ReadByte();// a buffer elso byte eleme a karakterek szama + 0 lezaras
	int iRealLen;
	if (bLen == 0)//ures a string tomb
		return 0;
	if ((unsigned char)bLen == (unsigned char)255)//ha a string hosszabb mint 255
		iRealLen=ReadInt()*sizeof(TCHAR);//akkor a tomb meretet byteban a kovetkezo int tartalmazza
	else
		iRealLen = (int)bLen*sizeof(TCHAR);//kulonben annyi, amennyit az elejen beolvastunk, byteban
	
	if(m_pPos > m_buffBase+m_iToGet-iRealLen)
		if(ContinueRecvMessage(iRealLen))
			return 0;
	TCHAR* pRes = (TCHAR*) m_pPos;//a string itt kezdodik
	m_pPos+=iRealLen;//tovabb mozgatjuk a poziciot a karakterek szamaval
	return pRes;
}

TCHAR** CSocketStack::ReadStrArray(int * iLen)//egy stringekbol allo tombot olvasunk (char**)
{
	*iLen = ReadInt();//a buffer elso int eleme a tomb merete
	TCHAR **ppchar = new TCHAR*[*iLen];//keszitunk egy tombot az adott merettel
	for(int i=0;i<*iLen;i++)//feltoltjuk a tombot
		ppchar[i] = ReadStr();
	return ppchar; //visszaterunk a tomb cimevel, es a meretenek cimevel
}

void CSocketStack::FlushSocket()
{
	int recNum;
	do 
	{
		Reset();
		recNum=recv(nSock,m_pPos,SOCKET_STACK_BUFFER_SIZE,0);//buffer feltoltese
	}
	while (recNum>0);//egesz addig, mig van adat fogadjuk az adatokat
	
	Reset();
}

void CSocketStack::SetHeader()//a buffer elejere beszurja a buffer hasznos meretet (kuldesnel)
{
	char* hPos=m_buffStart;//header (meret) pozicio
	*((int*)hPos) = m_pPos-m_buffBase;//adat meret beirasa

	hPos+=sizeof(int);//header (adatfolytatasjel) pozicioja
	*hPos = contSign;//adatfolytatasjel beiras
}

void CSocketStack::ReadHeader()//visszaolvassa a buffer hasznos meretet a headerbol
{
	m_iToGet = *((int*)m_pPos);
	m_pPos+=sizeof(int);
	contSign = *m_pPos;
	m_pPos++;
}

int CSocketStack::SendBuffer()//buffer tartalmanak kuldese a socketen keresztul
{
	return send(nSock,m_buffStart,(int)(m_pPos-m_buffStart),0);//az m_buffBase tartalmat kuldi
}

int CSocketStack::SendMessageBuffer()//buffer kuldese. A kuldes eredmenye alapjan hatarozzuk meg a visszateresi hibakodot
{
	if(CheckSocket())
		return m_err;
	SetHeader();
	int sent = SendBuffer();//kuldi a  buffert
	contSign=0;//kuldes utan toroljuk az adat folytatas flaget
	if (sent != m_pPos-m_buffStart)//ha nem sikerult a teljes buffert elkuldeni
	{
		m_err=SOCKET_STACK_SEND_ERROR;
		int z=WSAGetLastError();
		return m_err;
	}
	else
		return SOCKET_STACK_OK;
}

int CSocketStack::Invoke()//egy kuldes-fogadas egymas utan
{
	int sent_res = SendMessageBuffer();//elkuldi a buffert az adott socketnek, de elotte beilleszti a headert.
	if (sent_res == SOCKET_STACK_OK)
		return RecvMessage();
	else 
		return sent_res;
}

int CSocketStack::RecvBuffer(int iMaxLen)//adat fogadasa (buffer feltoltese a socketen keresztul)iMaxLen a fogadhato osszes adat mennyisege
{
	if(CheckSocket())
		return 0;
	int iBuffSize=iMaxLen;
	int m_iRecvBufferRet=0;//ez tarolja az egyidoben erkezett adatok szamat
	int m_iRecvBufferCount=0;
	do 
	{
		m_iRecvBufferRet=recv(nSock,m_pPos+m_iRecvBufferCount,iBuffSize,0);//buffer feltoltese
		if (m_iRecvBufferRet>0)//ha erkezett adat
		{
			m_iRecvBufferCount+=m_iRecvBufferRet;//a teljes fogadott adatmennyiseget noveljuk az erkezett adat mennyiseggel
			iBuffSize-=m_iRecvBufferRet;//csokkentjuk a fogadhato adat mennyiseget a mar beerkezett mennyiseggel
		}
	} while (m_iRecvBufferRet>0 && m_iRecvBufferCount<iMaxLen);//egesz addig, mig van adat fogadjuk az adatokat
	return m_iRecvBufferCount;//az osszes fogadott adat mennyisege
}

int CSocketStack::RecvMessage()//buffer fogadasa es a hibakod meghatarozasa
{
	Reset(true);//a header ele allunk
	int iReceived = RecvBuffer(SOCKET_STACK_HEADER_SIZE);//headernyi adatot fogad es beolvassa a bufferbe
	if (iReceived != SOCKET_STACK_HEADER_SIZE)
	{
		m_err=SOCKET_STACK_RECV_HEADER_ERROR;
		return m_err;
	}
	Reset(true);//a header ele allunk
	ReadHeader();//kiolvassuk a headert a bufferbol, mely a fogadando adat merete es a folytatas flag, es beallitjuk az m_iToGet valtozot es a contSign-t ez alapjan

	if (m_iToGet > SOCKET_STACK_BUFFER_SIZE)//ha tul nagy adatot jelez a fogadott header (1Mb+header a max)
	{
		m_iToGet=0;
		m_err=SOCKET_STACK_TOO_BIG_DATA;
		return m_err;
	}

	iReceived = RecvBuffer(m_iToGet);//beolvassuk az adatokat a bufferbe
	if (m_iToGet != iReceived)//ha nem mindet sikerult
	{
		iReceived=0;
		m_err=SOCKET_STACK_RECV_BODY_ERROR;
		return m_err;
	}

	Reset();//header utan allunk
	return SOCKET_STACK_OK;
}

int CSocketStack::GetErrorCode()
{
	return m_err;
}

const TCHAR* CSocketStack::GetErrorMessageStr(int iErr)//a hibakod megadasaval visszaadja a hiba szoveget
{
	switch (iErr)
	{
		case SOCKET_STACK_OK: return _T("No Error");
		case SOCKET_STACK_SEND_ERROR: return _T("Stack send error");
		case SOCKET_STACK_RECV_HEADER_ERROR : return _T("Stack receive header error");
		case SOCKET_STACK_RECV_BODY_ERROR: return _T("Stack receive body error");
		case SOCKET_STACK_ACCEPT_ERROR : return _T("Stack accept error");
		case SOCKET_STACK_CONNECT_ERROR : return _T("Stack connect error");
		case SOCKET_STACK_TOO_BIG_DATA: return _T("Stack wrong format or too big message");
		case SOCKET_STACK_SERVER_STUB_ERROR : return _T("Stack server stub error");
		case SOCKET_STACK_NOT_OPEN : return _T("No database is open.");
		case SOCKET_STACK_NO_RIGHTS : return _T("Connected user has insufficient permissions.");
		case SOCKET_STACK_SOCKET_ERROR : return _T("Invalid socket.");
		case SOCKET_STACK_NO_MORE_DATA : return _T("No more data.");
		case SOCKET_STACK_TOO_FEW_DATA : return _T("Too few data received.");
	}

	if (iErr > SOCKET_STACK_USER_ERROR_BASE) return _T("Stack user error");
	return _T("Unknown stack error");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////// Global Functions //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Socket functions
//////////////////////////////////////////////////////////////////////

int socket_init() //elmeletileg 1x kell meghivni az alkalmazas elejen. Nelkule nem mukodik a halozati kommunikacio
{
	int res;
	struct WSAData wsaData;
	if ((res = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0)
	{
		AfxMessageBox(_T("WSAStartup failed."));
	}
	return res;
}

int socket_uninit()//elmeletileg 1x kell meghivni az alkalmazas vegen
{
	int res = WSACleanup();
	return res;
}

SOCKET socket_accept(SOCKET sockfd, struct sockaddr * sock_addr, int * sock_addr_length)//varakozik a kliens kapcsolodasara
{
	return accept(sockfd, (struct sockaddr *)sock_addr, sock_addr_length);//a kliens kapcsolat sockettel ter vissza
}

SOCKET socket_server(unsigned short port)//server tipusu socket letrehozasa az adott porton
{
	struct sockaddr_in addr;

	addr.sin_family = PF_INET;//ipv4
	addr.sin_port = htons(port);//port
	memset(addr.sin_zero, 0, 8);

	SOCKET sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_IP);

	addr.sin_addr.s_addr = htonl(INADDR_ANY);//minden ip-n figyel
	int val = 1;

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&val, sizeof(val)) == SOCKET_ERROR)//mar hasznalatban levo cimre is portra is bindelheto
		return INVALID_SOCKET;
	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == SOCKET_ERROR)//bindeles
		return INVALID_SOCKET;
	if (listen(sockfd, SOMAXCONN) == SOCKET_ERROR)
		return INVALID_SOCKET;

	return sockfd;
}

SOCKET socket_client(char* port, char* host, struct addrinfo **aiList)//client tipusu socket letrehozasa
{
	static struct addrinfo aiHints={0,0,0,0,0,0,0,0};

	aiHints.ai_family = AF_INET;//ipv4
	aiHints.ai_socktype = SOCK_STREAM;
	aiHints.ai_protocol = IPPROTO_TCP;//tcp

	if (getaddrinfo(host, port, &aiHints, aiList) != 0)
		return INVALID_SOCKET;//nevfeloldas
	return socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

SOCKET socket_client(WCHAR* port, WCHAR* host, struct addrinfo **aiList)//unicode valtozat
{
	char * port_mb=wchartochar(port);
	char * host_mb=wchartochar(host);
	SOCKET sockfd=socket_client(port_mb, host_mb, aiList);
	if (port_mb) delete [] port_mb;
	if (host_mb) delete [] host_mb;
	return sockfd;
}

void socket_close(SOCKET sock)//socket bezarasa
{
	closesocket(sock);//kapcsolat bezarasa
}

wchar_t * chartowchar(char * instr)//unicode char to char (nem hasznaljuk)
{
	if (instr==0)
		return 0;
	else
	{
		size_t count=strlen(instr)+1;
		wchar_t * out = new wchar_t[count];
		size_t num_res;
		mbstowcs_s(&num_res,out,count,instr,count-1);
		return out;
	}
}

char * wchartochar(wchar_t * instr)//char to unicode char
{
	if (instr==0)
		return 0;
	else
	{
		size_t count=wcslen(instr)+1;
		char * out = new char[count];
		size_t num_res;
		wcstombs_s(&num_res,out,count,instr,count-1);
		return out;
	}
}

//////////////////////////////////////////////////////////////////////
// Getting address and hostname by socket
//////////////////////////////////////////////////////////////////////

CString getClientIpAddress(SOCKET sock)
{
	struct sockaddr_in add;
	int size=sizeof(add);
	int ret=getpeername(sock,(sockaddr*)&add, &size);
	if (ret==SOCKET_ERROR )
		return _T("");

	CString str(inet_ntoa(add.sin_addr));
	
	return str;
}

CString getClientHostName(SOCKET sock)
{
	struct sockaddr_in add;
	struct hostent *hostdata;
	int size=sizeof(add);
	int ret=getpeername(sock,(sockaddr*)&add, &size);
	if (ret==SOCKET_ERROR )
		return _T("");

	hostdata=gethostbyaddr((const char*)&add.sin_addr.s_addr,4,PF_INET);
	if(hostdata==NULL)
		return _T("");
	
	CString str(hostdata->h_name);
		
	return str;
}

//////////////////////////////////////////////////////////////////////
// String cryption function //LPCTSTR string titkosito (true:bekodol, false:kikodol)
//////////////////////////////////////////////////////////////////////

void cryptString(CString *strText, bool cr)
{
	//srand ((unsigned)time(NULL));//ezt kell a program elejen valahol meghivni
	CString hexStr,str;
	int strLength=strText->GetLength();
	int byteSize;
	BYTE* tempString;

	if(cr)
	{
		byteSize=sizeof(TCHAR)*strLength;
		tempString=new BYTE[byteSize+1];
		tempString[byteSize]=0;
		memcpy(tempString,strText->GetBuffer(),byteSize);

		for(int i=0;i<(byteSize);i++)
		{
			BYTE rnd=rand() % 255;
			BYTE num=tempString[i]^rnd;
			//tempString[i]=num;
			hexStr.Format(_T("%.2x"),num);
			str.Append(hexStr);
			hexStr.Format(_T("%.2x"),rnd);
			str.Insert(0,hexStr);
		}
		strText->Format(_T("%s"),str);
	}
	else
	{
		byteSize=strLength/4;
		tempString=new BYTE[byteSize+2];
		tempString[byteSize]=0;
		tempString[byteSize+1]=0;

		for(int i=0;i<byteSize;i++)
		{
			int num1=_tcstol(strText->Mid(i*2+strLength/2,2),NULL,16);
			int num2=_tcstol(strText->Mid(strLength/2-i*2-2,2),NULL,16);
			num1^=num2;
			tempString[i]=num1;
		}
		strText->Format(_T("%s"),(TCHAR*)tempString);
	}
	delete [] tempString;
}

void HashString(CString *strText)
{
	unsigned long hash1=523646;
	unsigned long hash2=98546;
	unsigned long hash3=1177675;
	unsigned long hash4=968664;
	
	for(int i=0;i<strText->GetLength();i++)
	{
		hash1=((hash1<<5)+hash1)+strText->GetAt(i);
		hash2=((hash2<<5)+hash2)+strText->GetAt(i);
		hash3=((hash3<<5)+hash3)+strText->GetAt(i);
		hash4=((hash4<<5)+hash4)+strText->GetAt(i);
	}

	strText->Format(_T("%x%x%x%x"),hash1,hash2,hash3,hash4);
}
