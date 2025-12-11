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

TCHAR* Value2Str( TCHAR *buff, int size, const long val );

CSqlConnectInfo::CSqlConnectInfo()
: TSqlConnectInfo()
, m_Error( nullptr )
{
    m_IsAutoCommit = true;

    m_nThread = pthread_self();
}

CSqlConnectInfo::CSqlConnectInfo( CSqlConnectInfo &c )
: TSqlConnectInfo()
, m_Error( nullptr )
{
    *this = c;

    m_nThread = pthread_self();
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
        if( m_nDB == ID_PDATABASES_SQLITE )
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
            SQLRETURN rc = sqlite3_open( (const char*)this->m_BaseName, &connect );
            if( rc != SQL_SUCCESS )
            {
                GetErrorInfo( NULL );
                return false;
            }
            
            /* Finetune sqlite (quite slow without these) */
            rc = sqlite3_extended_result_codes( connect, true );
            rc = sqlite3_exec( connect, "PRAGMA cache_size = 8000;"    , 0, 0, 0 );
            rc = sqlite3_exec( connect, "PRAGMA synchronous = OFF;"    , 0, 0, 0 );
            rc = sqlite3_exec( connect, "PRAGMA journal_mode = MEMORY;", 0, 0, 0 );
            rc = sqlite3_enable_shared_cache( true );
        
            iHdbc = (SQLHDBC) connect;
            
            m_IsOpen = true;
        }
        else
        if( m_nDB == ID_PDATABASES_MYSQL )
        {
            MYSQL *initialize = mysql_init( NULL ); //initialize the instance
            iHenv = ( SQLHENV ) initialize;
            if( iHenv )
            {
                SQLRETURN rc = 0;
                //rc = mysql_options( initialize, MYSQL_SET_CHARSET_NAME, "ascii" );
                rc = mysql_options( initialize, MYSQL_SET_CHARSET_NAME, "utf8" );
                //SET SESSION SQL_MODE='ALLOW_INVALID_DATES'
                //SET time_zone = "+00:00"
                
                //unsigned int timeout = 20;        
                //rc = mysql_options( initialize, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&timeout );                
                
                if( strcmp( (const char*)m_ServerName, "." ) == 0 ) // socket use option (Unix/Linux)
                {
                    unsigned int opt = MYSQL_PROTOCOL_SOCKET;
                    rc = mysql_options(initialize, MYSQL_OPT_PROTOCOL, (char const*)&opt);
                    _tcscpy( (TCHAR*)m_ServerName, "localhost" );
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
                
                iHdbc = (SQLHDBC) connect;
                
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
        else
        if( m_nDB == ID_PDATABASES_ODBC )
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
                        rc = SQLConnect(iHdbc, m_BaseName, SQL_NTS, m_UserName, SQL_NTS, m_UserPass, SQL_NTS);
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
            if( m_nDB == ID_PDATABASES_SQLITE )
            {
                sqlite3_close( (sqlite3*) iHdbc );
            }
            else
            if( m_nDB == ID_PDATABASES_MYSQL )
            {
                //mysql_close( (MYSQL*) iHdbc );
            }
            else
            if( m_nDB == ID_PDATABASES_ODBC )
            {
                SQLDisconnect( iHdbc );
                SQLFreeHandle( SQL_HANDLE_DBC, iHdbc );
            }
            iHdbc = SQL_NULL_HANDLE;
        }
    }
    
    if( iHenv )
    {
        if( m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else
        if( m_nDB == ID_PDATABASES_MYSQL )
        {
            mysql_close( (MYSQL*) iHenv );
        }
        if( m_nDB == ID_PDATABASES_ODBC )
        {
            SQLFreeHandle( SQL_HANDLE_ENV, iHenv ); 
        }
        iHenv = SQL_NULL_HANDLE;
    }

    m_IsOpen = false;
}

bool
CSqlConnectInfo::GetErrorInfo( SQLHSTMT aHstmt )
{
    long nErrMsg = 0;
    
    delete m_Error;
           m_Error = new TSqlErrorInfo;

    if( m_nDB == ID_PDATABASES_SQLITE )
    {
        if( this->iHdbc )
        {
            sqlite3 *connect = (sqlite3 *) this->iHdbc;
            
            m_Error->ErrorNum = sqlite3_errcode( connect );

            int extended_errcode = sqlite3_extended_errcode( connect );
            Value2Str( (TCHAR*)m_Error->SqlState, sizeof( m_Error->SqlState ), extended_errcode );
            
            const char* sErrMsg = sqlite3_errmsg( connect );
            nErrMsg = strlen( sErrMsg );

            strncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < SQL_MAX_MESSAGE_LENGTH ? nErrMsg : SQL_MAX_MESSAGE_LENGTH - 1 );
        }
    }
    else
    if( m_nDB == ID_PDATABASES_MYSQL )
    {
        if( aHstmt != NULL )
        {
            m_Error->ErrorNum = mysql_stmt_errno( (MYSQL_STMT*) aHstmt );
            
            const char* sErrMsg = mysql_stmt_sqlstate( (MYSQL_STMT*) aHstmt );
            nErrMsg = strlen( sErrMsg );
            if( nErrMsg > 0 )
            {
                strncpy( (char*)m_Error->SqlState, sErrMsg, nErrMsg < sizeof(m_Error->SqlState) ? nErrMsg : SQL_MAX_MESSAGE_LENGTH - 1 );
            }
            
            sErrMsg = mysql_stmt_error( (MYSQL_STMT*) aHstmt );
            nErrMsg = strlen( sErrMsg );
            if( nErrMsg > 0 )
            {
                strncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < SQL_MAX_MESSAGE_LENGTH ? nErrMsg : SQL_MAX_MESSAGE_LENGTH - 1 );
            }
        }
        else
        {
            if( this->iHenv == NULL )
            {
                strcpy( (char*)m_Error->SqlError, "Could not initialize Mysql connection" );
            }
            else
            if( this->iHdbc == NULL )
            {
                MYSQL *connect = (MYSQL *) this->iHenv;
                
                const char* sErrMsg = mysql_error( connect );
                nErrMsg = strlen( sErrMsg );
                if( nErrMsg > 0 )
                {
                    strncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < SQL_MAX_MESSAGE_LENGTH ? nErrMsg : SQL_MAX_MESSAGE_LENGTH - 1 );
                }
            }
            else
            {
                MYSQL *connect = (MYSQL *) this->iHdbc;
                
                const char* sErrMsg = mysql_error( connect );
                nErrMsg = strlen( sErrMsg );
                if( nErrMsg > 0 )
                {
                    strncpy( (char*)m_Error->SqlError, sErrMsg, nErrMsg < SQL_MAX_MESSAGE_LENGTH ? nErrMsg : SQL_MAX_MESSAGE_LENGTH - 1 );
                }
            }
        }
    }
    else
    if( m_nDB == ID_PDATABASES_ODBC )
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
                    rc = SQLGetDiagRec( SQL_HANDLE_STMT, aHstmt, RecNumber, m_Error->SqlState, &m_Error->ErrorNum, m_Error->SqlError, ErrMsgLen, &pcbErrorMsgLen );
                    break;
                case 2:
                    if( iHdbc  != SQL_NULL_HDBC )
                    rc = SQLGetDiagRec( SQL_HANDLE_DBC , iHdbc , RecNumber, m_Error->SqlState, &m_Error->ErrorNum, m_Error->SqlError, ErrMsgLen, &pcbErrorMsgLen );
                    break;
            }
        }

        if( rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO )
            return true;
        else
            _tcscpy( (TCHAR*)m_Error->SqlError, _T("Severe Error in ODBC Driver") );
    }

    return nErrMsg > 0;
}
