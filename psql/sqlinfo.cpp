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

PTCHAR* Value2Str( PTCHAR *buff, int size, const long val );

CSqlConnectInfo::CSqlConnectInfo()
: TSqlConnectInfo()
, m_Error( nullptr )
{
    m_IsAutoCommit = true;

#ifdef __windows__
    m_nPid = GetCurrentProcessId();
#else
    m_nPid = getpid();
#endif

    m_nThread = std::this_thread::get_id(); // pthread_self();
}

CSqlConnectInfo::CSqlConnectInfo( CSqlConnectInfo &c )
: TSqlConnectInfo()
, m_Error( nullptr )
{
    *this = c;

#ifdef __windows__
    m_nPid = GetCurrentProcessId();
#else
    m_nPid = getpid();
#endif

    m_nThread = std::this_thread::get_id(); // pthread_self();
}

CSqlConnectInfo::~CSqlConnectInfo()
{
    delete m_Error;
    m_Error = nullptr;

    Close();
}

bool
CSqlConnectInfo::Open()
{
    if( !m_IsOpen )
    {
        switch( m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
//             sqlite3_config( SQLITE_CONFIG_MULTITHREAD );
//             SQLRETURN rc = sqlite3_initialize();
//             if( rc != SQL_SUCCESS )
//             {
//                 GetErrorInfo( NULL );
//                 return false;
//             }
//             sqlite3_config( SQLITE_CONFIG_SINGLETHREAD );
 
//             SQLRETURN rc = sqlite3_open_v2( this->m_BaseName, &iHdbc, SQLITE_OPEN_READWRITE, NULL );
            
            sqlite3 *connect = NULL;
            PSQLRETURN rc = sqlite3_open( (const char*)this->m_BaseName, &connect );
            if( rc != PSQL_SUCCESS )
            {
                GetErrorInfo( NULL );
                return false;
            }
            
            /* Finetune sqlite (quite slow without these) */
            rc = sqlite3_extended_result_codes( connect, true );
            rc = sqlite3_exec( connect, _T("PRAGMA cache_size = 8000;")    , 0, 0, 0 );
            rc = sqlite3_exec( connect, _T("PRAGMA synchronous = OFF;")    , 0, 0, 0 );
            rc = sqlite3_exec( connect, _T("PRAGMA journal_mode = MEMORY;"), 0, 0, 0 );
            rc = sqlite3_enable_shared_cache( true );
        
            iHdbc = (PSQLHDBC) connect;
            
            m_IsOpen = true;
        }
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
        {
            MYSQL *initialize = mysql_init( NULL ); //initialize the instance
            iHenv = ( PSQLHENV ) initialize;
            if( iHenv )
            {
                //rc = mysql_options( initialize, MYSQL_SET_CHARSET_NAME, _T("ascii") );
                PSQLRETURN rc = mysql_options( initialize, MYSQL_SET_CHARSET_NAME, _T("utf8") );
                //SET SESSION SQL_MODE='ALLOW_INVALID_DATES'
                //SET time_zone = "+00:00"
                
                //unsigned int timeout = 20;        
                //rc = mysql_options( initialize, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&timeout );
                
                if( strcmp( (const char*)m_ServerName, _T(".") ) == 0 ) // socket use option (Unix/Linux)
                {
                    unsigned int opt = MYSQL_PROTOCOL_SOCKET;
                    rc = mysql_options(initialize, MYSQL_OPT_PROTOCOL, (char const*)&opt);
                    tcscpy( m_ServerName, _T("localhost") );
                    m_Port = 0;
                }
                else                                                    // generic case
                {
                    memset( m_UnixSocket, 0, sizeof( m_UnixSocket ) );
                }

                MYSQL *connect = mysql_real_connect( initialize, (const char*)m_ServerName, (const char*)m_UserName, (const char*)m_UserPass, (const char*)m_BaseName, m_Port, (m_UnixSocket[0]?(const char*)m_UnixSocket:NULL), 0/*CLIENT_INTERACTIVE*/ );
                if( !connect )
                {
                    GetErrorInfo( NULL );
                    return false;
                }
                
                iHdbc = (PSQLHDBC) connect;
                
                //if( !mysql_autocommit( connect, m_IsAutoCommit ) )
                //{
                //    GetErrorInfo( NULL );
                //}
    
                m_IsOpen = true;
            }
            else
            {
                GetErrorInfo( NULL );
                return false;
            }
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            SQLRETURN rc = 0;
            rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &iHenv);
            if( SQL_SUCCESS == rc )
            {
                rc = SQLSetEnvAttr( iHenv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0 );
                if( SQL_SUCCESS == rc )
                {
                    rc = SQLAllocHandle(SQL_HANDLE_DBC, iHenv, &iHdbc);
                    if( SQL_SUCCESS == rc )
                    {
                        rc = SQLConnect(iHdbc, (SQLCHAR*)m_BaseName, SQL_NTS, (SQLCHAR*)m_UserName, SQL_NTS, (SQLCHAR*)m_UserPass, SQL_NTS);
                        if( SQL_SUCCESS == rc )
                        {
                            long type = ( (m_IsAutoCommit) ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
                            rc = SQLSetConnectAttr( iHdbc, SQL_ATTR_AUTOCOMMIT, (void*)type, 0 );
                        }
                    }
                }
            }
            if( rc != SQL_SUCCESS )
            {
                GetErrorInfo( NULL );
                return false;
            }
            m_IsOpen = true;
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            PTCHAR sPort[ 32 ];
            Value2Str( sPort, sizeof(sPort), m_Port );
            const char *pgoptions = NULL;      /* Connection options (usually NULL) */
            const char *pgtty = NULL;          /* Debug tty (usually NULL) */
            PGconn *initialize = PQsetdbLogin( (char*)m_ServerName, (char*)sPort, pgoptions, pgtty, (char*)m_BaseName,  (char*)m_UserName, (char*)m_UserPass );
            if( initialize )
            {
                iHdbc = (PSQLHDBC) initialize;
                m_IsOpen = true;
            }
            else
            {
                GetErrorInfo( NULL );
                return false;
            }
        }
        break;
#endif
#ifdef DEFINE_ORACLE
        case ID_PDATABASES_ORACLE :
        {
            const char CONNECT_STRING[] = "host[:port]/service_name";
            const char USERNAME[] = "your_username";
            const char PASSWORD[] = "your_password";

            OCIEnv *envhp = nullptr;
            OCIError *errhp = nullptr;
            OCIServer *srvhp = nullptr;
            OCISvcCtx *svchp = nullptr;
            OCISession *authp = nullptr;

            sword rc = OCIEnvCreate( &envhp, OCI_DEFAULT, (dvoid *)0, 0, 0, 0, (size_t)0, (dvoid **)0 ); // Initialize OCI environment, OCI_DEFAULT ???
            if( OCI_SUCCESS == rc )
            {
                rc = OCIHandleAlloc((dvoid *)envhp, (dvoid **)&errhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0); // Allocate error handle
                if( OCI_SUCCESS == rc )
                {
                    rc = OCIHandleAlloc((dvoid *)envhp, (dvoid **)&srvhp, OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0); // Allocate server handle
                    if( OCI_SUCCESS == rc )
                    {
                        rc = OCIServerAttach(srvhp, errhp, (text *)CONNECT_STRING, strlen(CONNECT_STRING), OCI_DEFAULT); // Attach to the server
                        if( OCI_SUCCESS == rc )
                        {
                            rc = OCIHandleAlloc((dvoid *)envhp, (dvoid **)&svchp, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0); // Allocate service context handle and set the server handle in it
                            if( OCI_SUCCESS == rc )
                            {
                                rc = OCIAttrSet((dvoid *)svchp, OCI_HTYPE_SVCCTX, (dvoid *)srvhp, (ub4)0, OCI_ATTR_SERVER, (OCIError *)errhp);
                                if( OCI_SUCCESS == rc )
                                {   // Authenticate the user (begin session)
                                    OCIHandleAlloc((dvoid *)envhp, (dvoid **)&authp, OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0);

                                    OCIAttrSet((dvoid *)authp, OCI_HTYPE_SESSION, (dvoid *)USERNAME, strlen(USERNAME), OCI_ATTR_USERNAME, errhp);
                                    OCIAttrSet((dvoid *)authp, OCI_HTYPE_SESSION, (dvoid *)PASSWORD, strlen(PASSWORD), OCI_ATTR_PASSWORD, errhp);

                                    OCISessionBegin(svchp, errhp, authp, OCI_CRED_RDBMS, (ub4)OCI_DEFAULT);




                                    // Clean up and disconnect
                                    OCISessionEnd(svchp, errhp, authp, OCI_DEFAULT);
                                    OCIServerDetach(srvhp, errhp, OCI_DEFAULT);
                                    OCIHandleFree((dvoid *)authp, OCI_HTYPE_SESSION);
                                    OCIHandleFree((dvoid *)svchp, OCI_HTYPE_SVCCTX);
                                    OCIHandleFree((dvoid *)srvhp, OCI_HTYPE_SERVER);
                                    OCIHandleFree((dvoid *)errhp, OCI_HTYPE_ERROR);
                                    //OCIEnvTerminate(envhp);
                                    //sword   OCITerminate( ub4 mode);
                                }
                            }
                        }
                    }
                }
            }
            if( rc != OCI_SUCCESS )
            {
                GetErrorInfo( NULL );
                return false;
            }
            m_IsOpen = true;
        }
        break;
#endif
        default: break;
        }
    }
    return m_IsOpen;
}

void
CSqlConnectInfo::Close()
{
    if( m_IsOpen )
    {
        if( iHdbc )
        {
            switch( m_nDB )
            {
            case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
            case ID_PDATABASES_SQLITE :
            {
                sqlite3_close( (sqlite3*) iHdbc );
            }
            break;
#endif
#ifdef DEFINE_MYSQL
            case ID_PDATABASES_MYSQL :
            {
                //mysql_close( (MYSQL*) iHdbc );
            }
            break;
#endif
#ifdef DEFINE_ODBC
            case ID_PDATABASES_ODBC :
            {
                SQLDisconnect( iHdbc );
                SQLFreeHandle( SQL_HANDLE_DBC, iHdbc );
            }
            break;
#endif
#ifdef DEFINE_POSTGRESQL
            case ID_PDATABASES_POSTGRESQL :
            {
                PQfinish( (PGconn*) iHdbc );
            }
            break;
#endif
            default : break;
            }
            iHdbc = PSQL_NULL_HANDLE;
        }
    }
    
    if( iHenv )
    {
        switch( m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
        }
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
        {
            mysql_close( (MYSQL*) iHenv );
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            SQLFreeHandle( SQL_HANDLE_ENV, iHenv ); 
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
        }
        break;
#endif
        default : break;
        }
        iHenv = PSQL_NULL_HANDLE;
    }

    m_IsOpen = false;
}

bool
CSqlConnectInfo::GetErrorInfo( PSQLHSTMT aHstmt )
{
    unsigned long nErrMsg = 0;
    
    delete m_Error;
           m_Error = new TSqlErrorInfo;

    switch( m_nDB )
    {
    case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
    case ID_PDATABASES_SQLITE :
    {
        if( this->iHdbc )
        {
            sqlite3 *connect = (sqlite3 *) this->iHdbc;
            
            m_Error->ErrorNum = sqlite3_errcode( connect );

            int extended_errcode = sqlite3_extended_errcode( connect );
            Value2Str( (TCHAR*)m_Error->SqlState, sizeof( m_Error->SqlState ), extended_errcode );
            
            const char* sErrMsg = sqlite3_errmsg( connect );
            nErrMsg = strlen( sErrMsg );

            strncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < PSQL_MAX_MESSAGE_LENGTH ? nErrMsg : PSQL_MAX_MESSAGE_LENGTH - 1 );
        }
    }
    break;
#endif
#ifdef DEFINE_MYSQL
    case ID_PDATABASES_MYSQL :
    {
        if( aHstmt != NULL )
        {
            m_Error->ErrorNum = mysql_stmt_errno( (MYSQL_STMT*) aHstmt );
            
            const char* sErrMsg = mysql_stmt_sqlstate( (MYSQL_STMT*) aHstmt );
            nErrMsg = strlen( sErrMsg );
            if( nErrMsg > 0 )
            {
                _tcsncpy( (char*)m_Error->SqlState, sErrMsg, nErrMsg < sizeof(m_Error->SqlState) ? nErrMsg : PSQL_MAX_MESSAGE_LENGTH - 1 );
            }
            
            sErrMsg = mysql_stmt_error( (MYSQL_STMT*) aHstmt );
            nErrMsg = strlen( sErrMsg );
            if( nErrMsg > 0 )
            {
                _tcsncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < PSQL_MAX_MESSAGE_LENGTH ? nErrMsg : PSQL_MAX_MESSAGE_LENGTH - 1 );
            }
        }
        else
        {
            if( this->iHenv == NULL )
            {
                _tcscpy( (char*)m_Error->SqlError, _T("Could not initialize Mysql connection") );
            }
            else
            if( this->iHdbc == NULL )
            {
                MYSQL *connect = (MYSQL *) this->iHenv;
                
                const char* sErrMsg = mysql_error( connect );
                nErrMsg = strlen( sErrMsg );
                if( nErrMsg > 0 )
                {
                    _tcsncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < PSQL_MAX_MESSAGE_LENGTH ? nErrMsg : PSQL_MAX_MESSAGE_LENGTH - 1 );
                }
            }
            else
            {
                MYSQL *connect = (MYSQL *) this->iHdbc;
                
                const char* sErrMsg = mysql_error( connect );
                nErrMsg = strlen( sErrMsg );
                if( nErrMsg > 0 )
                {
                    _tcsncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < PSQL_MAX_MESSAGE_LENGTH ? nErrMsg : PSQL_MAX_MESSAGE_LENGTH - 1 );
                }
            }
        }
    }
    break;
#endif
#ifdef DEFINE_ODBC
    case ID_PDATABASES_ODBC :
    {
        RETCODE     rc = SQL_NO_DATA_FOUND;
        short       pcbErrorMsgLen = 0, ErrMsgLen = SQL_MAX_MESSAGE_LENGTH;
        SQLSMALLINT RecNumber = 1;

        for( short i = 1; i <= 2 && ( rc == SQL_NO_DATA_FOUND || rc == SQL_INVALID_HANDLE ); i ++ ) 
        {
            switch( i ) 
            {
                case 1:
                    if( aHstmt != SQL_NULL_HSTMT )
                    rc = SQLGetDiagRec( SQL_HANDLE_STMT, aHstmt, RecNumber, (SQLCHAR*)m_Error->SqlState, &m_Error->ErrorNum, (SQLCHAR*)m_Error->SqlError, ErrMsgLen, &pcbErrorMsgLen );
                    break;
                case 2:
                    if( iHdbc  != SQL_NULL_HDBC )
                    rc = SQLGetDiagRec( SQL_HANDLE_DBC , iHdbc , RecNumber, (SQLCHAR*)m_Error->SqlState, &m_Error->ErrorNum, (SQLCHAR*)m_Error->SqlError, ErrMsgLen, &pcbErrorMsgLen );
                    break;
            }
        }

        if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
            return true;
        else
            _tcscpy( (PTCHAR*)m_Error->SqlError, _T("Severe Error in ODBC Driver") );
    }
    break;
#endif
#ifdef DEFINE_POSTGRESQL
    case ID_PDATABASES_POSTGRESQL :
    {
        if( this->iHdbc == NULL )
        {
            _tcscpy( (char*)m_Error->SqlError, _T("Could not initialize PostgreSQL connection") );
        }
        else
        {
            PGconn *connect = (PGconn *) this->iHdbc;
            const char* sErrMsg = PQerrorMessage( connect );
            nErrMsg = strlen( sErrMsg );
            if( nErrMsg > 0 )
            {
                _tcsncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < PSQL_MAX_MESSAGE_LENGTH ? nErrMsg : PSQL_MAX_MESSAGE_LENGTH - 1 );
            }
        }
    }
    break;
#endif
    }
    return nErrMsg > 0;
}
