/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/
#include "psql/sqlinfo.h"
#include "psql/sqlbridge.h"

// Initialize connections container
CSQLBridge g_SQLBridge;

void DefaultShowSQLErrorNow( const char *buff )
{
    _fprintf( stdout, _T("%s"), buff );
}

void (*g_ShowSQLErrorNow)( const char* ) = DefaultShowSQLErrorNow;

CSQLBridge::CSQLBridge()
: m_pSqlInfo( nullptr )
{
  Initialize();
}

CSQLBridge::~CSQLBridge()
{
  Destroy();
}

CSqlConnectInfoArray* 
CSQLBridge::Connects()
{
  return m_pSqlInfo;
}

void
CSQLBridge::Initialize()
{
  if( m_pSqlInfo == NULL )
     m_pSqlInfo = new CSqlConnectInfoArray;
}

void
CSQLBridge::Destroy()
{
  if( m_pSqlInfo )
      m_pSqlInfo->Flush();

  delete m_pSqlInfo;
         m_pSqlInfo = NULL;
}

// long
// CSQLBridge::findIndex( PHDBPROVIDER pHandle )
// {
//   if( Connects() == NULL )
//   {
//      ShowSQLErrorNow( NULL, (PTCHAR*)_T("not build connections") );
//      return -1;
//   }

//   if( Connects()->size() == 0 )
//   {
//      ShowSQLErrorNow( NULL, (PTCHAR*)_T("connections is empty") );
//      return -1;
//   }

//   if( pHandle )
//   {
//      for( unsigned long n = 0; n < Connects()->size(); ++ n )
//      {
//        if( Connects()->at( n ) == pHandle )
//          return n;
//      }

//      ShowSQLErrorNow( NULL, (PTCHAR*)_T("connection not found") );

//      return -1;
//   }

//   return 0;
// }

long
CSQLBridge::findIndex( PHDBPROVIDER pHandle )
{
    if( Connects() == NULL )
    {
        ShowSQLErrorNow( NULL, _T("not build connections") );
        return -1;
    }

    if( Connects()->size() == 0 )
    {
        ShowSQLErrorNow( NULL, _T("connections is empty") );
        return -1;
    }

    PTCHAR sBuff[ 64 ]; memset( sBuff, 0, sizeof( sBuff ) );
    if( pHandle )
    {
#ifdef __windows__
        DWORD nPid = GetCurrentProcessId();
#else
        pid_t nPid = getpid();
#endif
        std::thread::id nThread = std::this_thread::get_id(); // pthread_t pthread_self();

        for( unsigned long n = 0; n < Connects()->size(); n ++ )
        {
            CSqlConnectInfo *pItem = Connects()->at( n );
            if( pItem == pHandle )
            {
                if( pItem->m_nPid == nPid )
                {
                    if( pItem->m_nThread == nThread )
                    {
                        return n;
                    }
                    else
                    {
                        sprintf( sBuff, "Connection %p from another thread: %lu. Create connection in own thread %lu.", pHandle, pItem->m_nThread , nThread);
                        ShowSQLErrorNow( NULL, sBuff );
                    }
                }
                else
                {
                    sprintf( sBuff, "Connection %p from another process: %i. Create connection in own process %i.", pHandle, pItem->m_nPid, nPid );
                    ShowSQLErrorNow( NULL, sBuff );
                }
            }
        }

        sprintf( sBuff, "Connection %p in process %i, thread %lu not found.", pHandle, nPid, nThread );
        ShowSQLErrorNow( NULL, sBuff );

        return -1;
    }

    return 0;
}

void
CSQLBridge::Start( PHDBPROVIDER hconnect )
{
  long connect = findIndex( hconnect );
  if( connect >= 0 )
     Connects()->at( connect )->Open();
}

void
CSQLBridge::Stop( PHDBPROVIDER hconnect )
{
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     auto conns = Connects();
     conns->at( connect )->Close();
     //conns->erase( conns->begin() + connect );
  }
}

void      
CSQLBridge::ShowErrorNow( PHDBPROVIDER hconnect, PSQLHSTMT hstmt )
{
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     CSqlConnectInfo *session = Connects()->at( connect ); 
     session->GetErrorInfo( hstmt );
     ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
  }
}

int*
CSQLBridge::GetUserCode( PHDBPROVIDER hconnect )
{ 
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     return &Connects()->at( connect )->m_UserCode; 
  }
  return NULL;
}

bool
CSQLBridge::Connect( CSqlConnectInfo &Info, PHDBPROVIDER *pHandle )
{
    if( pHandle ) { *pHandle = NULL;}

    CSqlConnectInfo *pNew = new CSqlConnectInfo( Info );
    if( pNew->Open() )
    {
        auto conns = Connects();
        conns->push_back( pNew );
        if( pHandle )
        {
            *pHandle = conns->at( conns->size() - 1 );
        }
        return true;
    }

    ShowSQLErrorNow( pNew->m_Error, (PTCHAR*)NULL );

    pNew->Close();
    delete pNew;

    return false;
}

PTCHAR*
CSQLBridge::GetSysadm( PHDBPROVIDER hconnect )
{
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     return (PTCHAR*)Connects()->at( connect )->m_SysAdmin;
  }
  return NULL;
}

PTCHAR*
CSQLBridge::GetBaseName( PHDBPROVIDER hconnect )
{ 
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     return (PTCHAR*)Connects()->at( connect )->m_BaseName;
  }
  return NULL;
}

PDatabases
CSQLBridge::GetDatabase( PHDBPROVIDER hconnect )
{
    long connect = findIndex( hconnect );
    if( connect >= 0 )
    {
        return Connects()->at( connect )->m_nDB;
    }
    return PDatabases::ID_PDATABASES_NONE;
}


PTCHAR*
CSQLBridge::GetUserName( PHDBPROVIDER hconnect )
{ 
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     return (PTCHAR*)Connects()->at( connect )->m_UserName;
  }
  return NULL;
}

PTCHAR*
CSQLBridge::GetUserPass( PHDBPROVIDER hconnect )
{ 
  long connect = findIndex( hconnect );
  if( connect >= 0 )
  {
     return (PTCHAR*)Connects()->at( findIndex( hconnect ) )->m_UserPass;
  }
  return NULL;
}

void
CSQLBridge::ShowSQLErrorNow( TSqlErrorInfo *err_info, const PTCHAR *err_text  )
{
  STD_STRING buff;

  if( err_info != NULL )
  {
    PTCHAR sNum[16]; memset( sNum, 0, sizeof(sNum) );
	_tprintf( sNum, _T("%i"), err_info->ErrorNum );

	buff  = _T("SQL-Error:");
    buff += (PTCHAR*)err_info->SqlError;
	buff += _T(", ");

	buff += _T("State:");
    buff += (PTCHAR*)err_info->SqlState;
	buff += _T(", ");
	buff += _T("Nummer:");
	buff += sNum;
	buff += _T("\n");
  }
  else
  {
	buff  = _T("SQL-Error:");
    if( err_text != NULL )
        buff  = (PTCHAR*)err_text;
	else
		buff  = _T("empty");
	buff  = _T(", ");

	buff += _T("State:");
	buff += _T("empty");
	buff += _T(", ");
	buff += _T("Nummer:");
	buff += _T("empty");
	buff += _T("\n");
  }

  if( ::g_ShowSQLErrorNow )
  {
      ::g_ShowSQLErrorNow( buff.c_str() );
  }
}
