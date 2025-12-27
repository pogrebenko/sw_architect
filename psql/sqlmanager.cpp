/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/
#include "psql/sqlbridge.h"
#include "psql/sqlmanager.h"

extern CSQLBridge g_SQLBridge;

CSQLBuilder::CSQLBuilder( HDBPROVIDER Connect )
: m_Connect( Connect )
{
    m_pSQLBridge = &g_SQLBridge;
}

CSqlConnectInfo*
CSQLBuilder::GetSession( HDBPROVIDER hconnect )
{
    long connect = m_pSQLBridge->findIndex( hconnect );
    if( connect >= 0 )
    {
        return m_pSQLBridge->Connects()->at( m_pSQLBridge->findIndex( hconnect ) );
    }
    return NULL;
}

SQLRETURN
CSQLBuilder::Execute( SQLHSTMT query, CVectorOfPtr<CSmPt> *in )
{
    this->Clear();
    
    SQLRETURN rc = this->Before( query );
    if( rc != SQL_SUCCESS )
    {
        return rc;
    }

    for( unsigned short i = 0; i < in->size(); i ++ )
    {
        CSmPt *db_data = in->at( i );
        short  db_type = 0;
        
        switch( db_data->DataType() ) 
        {
            case datShort    : db_type = SQL_C_SHORT         ; break;
            case datLong     : db_type = SQL_C_LONG          ; break;
            case datFloat    : db_type = SQL_C_FLOAT         ; break;
            case datDouble   : db_type = SQL_C_DOUBLE        ; break;
            case datChars    : db_type = SQL_C_CHAR          ; break;
            case datBytes    : db_type = SQL_C_BINARY        ; break;
            case datDate     : db_type = SQL_C_TYPE_DATE     ; break;
            case datTime     : db_type = SQL_C_TYPE_TIME     ; break;
            case datDateTime : db_type = SQL_C_TYPE_TIMESTAMP; break;
            case datTimeStamp: db_type = SQL_C_TYPE_TIMESTAMP; break;
        }
        
        rc = AccessSingleMethod( query, (unsigned short)i, db_type, db_data );
        if( rc != SQL_SUCCESS )
        {
            return rc;
        }
    }

    return this->After( query );
}

CSQLBind::CSQLBind( HDBPROVIDER Connect )
: CSQLBuilder( Connect )
{
}

void
CSQLBind::Clear()
{
    m_Binds.clear();
}

SQLRETURN
CSQLBind::Before( SQLHSTMT /*query*/ )
{
    return SQL_SUCCESS;
}

SQLRETURN
CSQLBind::After( SQLHSTMT query )
{
    SQLRETURN rc = SQL_SUCCESS;
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            if( m_Binds.size() > 0 )
            {
                MYSQL_STMT *stmt = (MYSQL_STMT*)query;
                //DB_CHECK_PARAMS( m_pBind, stmt );
                //mysql_stmt_attr_set( stmt, STMT_ATTR_PREBIND_PARAMS, &size );
                //rc = mysql_stmt_bind_param( stmt, m_pBinds );
                rc = mysql_stmt_bind_param( stmt, m_Binds.data() );
                //MYSQL_RES *metadata = mysql_stmt_result_metadata( stmt );
            }
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
        }
    }
    return rc;
}

SQLRETURN
CSQLBind::AccessSingleMethod( SQLHSTMT query, unsigned short num, short type, CSmPt *db_data )
{
    void     *var    = db_data->DataPointer();
    long      MaxLen = db_data->DataSize();
    SQLRETURN rc     = SQL_SUCCESS;

    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
            num = num + 1;
            switch( type )
            {
                case SQL_C_SHORT : break;
                case SQL_C_LONG  : {
                    long value = *(long*)var;
                    rc = sqlite3_bind_int64( (sqlite3_stmt*)query, num, value );
                } break;
                case SQL_C_FLOAT : break;
                case SQL_C_DOUBLE: break;
                case SQL_C_CHAR  : 
                {
                    switch( db_data->DataConvert() )
                    {
                        case datLong   : 
                        {
                            long value = *(long*)var;
                            rc = sqlite3_bind_int64( (sqlite3_stmt*)query, num, value );
                        }
                        break;
                        case datDouble :
                        case datFloat  : break;
                        default        :
                        {
                            const char * value = (const char*)var;
                            rc = sqlite3_bind_text( (sqlite3_stmt*)query, num, value, -1, SQLITE_TRANSIENT ); // SQLITE_STATIC
                        } break;
                    }
                } break;
                case SQL_C_BINARY        : break;
                case SQL_C_TYPE_DATE     : break;
                case SQL_C_TYPE_TIME     : break;
                case SQL_C_TYPE_TIMESTAMP: break;
            }
        }
        else
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            MYSQL_BIND Bind; memset( &Bind, 0, sizeof( MYSQL_BIND ) );
            switch( type )
            {
                case SQL_C_SHORT         : Bind.buffer_type = MYSQL_TYPE_SHORT    ; break;
                case SQL_C_LONG          : Bind.buffer_type = MYSQL_TYPE_LONG     ; break;
                case SQL_C_FLOAT         : Bind.buffer_type = MYSQL_TYPE_FLOAT    ; break;
                case SQL_C_DOUBLE        : Bind.buffer_type = MYSQL_TYPE_DOUBLE   ; break;
                case SQL_C_BINARY        : Bind.buffer_type = MYSQL_TYPE_LONG_BLOB; break;
                case SQL_C_TYPE_DATE     : Bind.buffer_type = MYSQL_TYPE_DATE     ; break;
                case SQL_C_TYPE_TIME     : Bind.buffer_type = MYSQL_TYPE_TIME     ; break;
                case SQL_C_TYPE_TIMESTAMP: Bind.buffer_type = MYSQL_TYPE_DATETIME ; break;
                case SQL_C_CHAR          :
                {
                    switch( db_data->DataConvert() )
                    {
                        case datLong   : Bind.buffer_type = MYSQL_TYPE_LONG  ; break;
                        case datDouble : Bind.buffer_type = MYSQL_TYPE_DOUBLE; break;
                        case datFloat  : Bind.buffer_type = MYSQL_TYPE_FLOAT ; break;
                        default        :
                        {
                            const char * value = (const char*)var;
                            Bind.buffer_type   = MYSQL_TYPE_STRING;
                            Bind.buffer_length = value ? strlen( value ) : 0; //MaxLen;
                        } break;
                    }
                } break;
            }
            Bind.buffer = var;
            m_Binds.push_back( Bind );
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            switch( type )
            {
            case SQL_C_SHORT  :
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, var, 0, &MaxLen );
                break;
            case SQL_C_LONG  : {
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, var, 0, &MaxLen );
            }
            break;
            case SQL_C_FLOAT    :
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, var, 0, &MaxLen );
                break;
            case SQL_C_DOUBLE    :
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, var, 0, &MaxLen );
                break;
            case SQL_C_CHAR   : {
                switch( db_data->DataConvert() )
                {
                    case datLong   :
                    {
                        db_data->DataPt.DataLength = SQL_NTS;
                        rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, MaxLen, 0, var, 0, &db_data->DataPt.DataLength );
                    }
                    break;
                    case datDouble :
                    case datFloat  :
                    {
                        db_data->DataPt.DataLength = SQL_NTS;
                        rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_DECIMAL, MaxLen, 0, var, 0, &db_data->DataPt.DataLength );
                    } break;
                    default        :
                    {
                        db_data->DataPt.DataLength = SQL_NTS;
                        rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, MaxLen, 0, var, 0, &db_data->DataPt.DataLength );
                    }
                    break;
                }
            }
            break;
            case SQL_C_BINARY : {
                static long length = SQL_DATA_AT_EXEC;
                db_data->DataPt.DataLength = MaxLen;
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, MaxLen, 0, var, 0, &length );
            }
            break;
            case SQL_C_TYPE_DATE:
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, 0, var, 0, &MaxLen );
                break;
            case SQL_C_TYPE_TIME:
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_TYPE_TIME, SQL_TYPE_TIME, 0, 0, var, 0, &MaxLen );
                break;
            case SQL_C_TYPE_TIMESTAMP:
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 0, 0, var, 0, &MaxLen );
                break;
            }
        }
    }
    
    return rc;
}

CSQLBuff::CSQLBuff( HDBPROVIDER Connect )
: CSQLBuilder( Connect )
{
}

void
CSQLBuff::Clear()
{
    m_Buffs.clear();
}

SQLRETURN
CSQLBuff::Before( SQLHSTMT /*query*/ )
{
    return SQL_SUCCESS;
}

void
CSQLBuff::ClearBuffs()
{
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            std::for_each( m_Buffs.begin(), m_Buffs.end(),
                []( auto Item ){ memset( Item.buffer, 0, Item.buffer_length ); }
            );
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
        }
    }
}

SQLRETURN
CSQLBuff::After( SQLHSTMT query )
{
    SQLRETURN rc = SQL_SUCCESS;
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            if( m_Buffs.size() > 0 )
            {
                MYSQL_STMT *stmt = (MYSQL_STMT*)query;
                //rc = mysql_stmt_attr_set( stmt, STMT_ATTR_PREFETCH_ROWS, &size );
                //rc = mysql_stmt_fetch_column( stmt, m_pBuffs, 0, 0 );
                //rc = mysql_stmt_bind_result( stmt, m_pBuffs );
                rc = mysql_stmt_bind_result( stmt, m_Buffs.data() );
            }
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
        }
    }
    return rc;
}

SQLRETURN
CSQLBuff::AccessSingleMethod( SQLHSTMT query, unsigned short num, short type, CSmPt *db_data )
{
    void     *var    = db_data->DataPointer();
    long      MaxLen = db_data->DataSize();
    SQLRETURN rc     = SQL_SUCCESS;

    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
            switch( type )
            {
                case SQL_C_LONG : {
                    int res = sqlite3_column_int( (sqlite3_stmt*)query, num );
                    *((long*)var) = res;
                }
                break;
                case SQL_C_FLOAT  : break;
                case SQL_C_DOUBLE : break;
                case SQL_C_CHAR   : {
                    const char *res = (const char*)sqlite3_column_text( (sqlite3_stmt*)query, num );
                    strcpy( (char*)var, res );
                }
                break;
                case SQL_C_BINARY        : break;
                case SQL_C_TYPE_DATE     : break;
                case SQL_C_TYPE_TIME     : break;
                case SQL_C_TYPE_TIMESTAMP: break;
            }
        }
        else
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            MYSQL_BIND Bind; memset( &Bind, 0, sizeof( MYSQL_BIND ) );
            switch( type )
            {
                case SQL_C_SHORT         : Bind.buffer_type = MYSQL_TYPE_SHORT    ; break;
                case SQL_C_LONG          : Bind.buffer_type = MYSQL_TYPE_LONG     ; break;
                case SQL_C_FLOAT         : Bind.buffer_type = MYSQL_TYPE_FLOAT    ; break;
                case SQL_C_DOUBLE        : Bind.buffer_type = MYSQL_TYPE_DOUBLE   ; break;
                case SQL_C_BINARY        : Bind.buffer_type = MYSQL_TYPE_LONG_BLOB; break;
                case SQL_C_TYPE_DATE     : Bind.buffer_type = MYSQL_TYPE_DATE     ; break;
                case SQL_C_TYPE_TIME     : Bind.buffer_type = MYSQL_TYPE_TIME     ; break;
                case SQL_C_TYPE_TIMESTAMP: Bind.buffer_type = MYSQL_TYPE_DATETIME ; break;
                case SQL_C_CHAR          :
                {
                    switch( db_data->DataConvert() )
                    {
                        case datLong   : Bind.buffer_type = MYSQL_TYPE_LONG  ; break;
                        case datDouble : Bind.buffer_type = MYSQL_TYPE_DOUBLE; break;
                        case datFloat  : Bind.buffer_type = MYSQL_TYPE_FLOAT ; break;
                        default        :
                        {
                            Bind.buffer_type   = MYSQL_TYPE_STRING;
                            Bind.buffer_length = MaxLen;
                        } break;
                    }
                } break;
            }
            Bind.buffer = var;
            m_Buffs.push_back( Bind );
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            switch( type )
            {
                case SQL_C_LONG : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, 0, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_FLOAT : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, 0, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_DOUBLE : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_CHAR : {
                    db_data->DataPt.DataLength = SQL_NTS;
                    rc = SQLBindCol( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_BINARY : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_TYPE_DATE: {
                    rc = SQLBindCol( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case SQL_C_TYPE_TIME: {
                    rc = SQLBindCol( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case SQL_C_TYPE_TIMESTAMP: {
                    rc = SQLBindCol( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
            }
        }
    }
    return rc;
}

CSQLData::CSQLData( HDBPROVIDER Connect )
: CSQLBuilder( Connect )
{
}

void
CSQLData::Clear()
{
}

SQLRETURN
CSQLData::Before( SQLHSTMT )
{
    return SQL_SUCCESS;
}

SQLRETURN
CSQLData::After( SQLHSTMT )
{
    return SQL_SUCCESS;
}

SQLRETURN
CSQLData::AccessSingleMethod( SQLHSTMT query, unsigned short num, short type, CSmPt *db_data )
{
    void     *var    = db_data->DataPointer();
    long      MaxLen = db_data->DataSize();
    SQLRETURN rc     = SQL_SUCCESS;
    
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            switch( type )
            {
                case SQL_C_LONG : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_FLOAT : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_DOUBLE : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_CHAR : {
                    db_data->DataPt.DataLength = SQL_NTS;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case SQL_C_BINARY :
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                    break;
                case SQL_C_TYPE_DATE: {
                    rc = SQLGetData( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case SQL_C_TYPE_TIME: {
                    rc = SQLGetData( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case SQL_C_TYPE_TIMESTAMP: {
                    rc = SQLGetData( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
            }
        }
    }

    return rc;
}

CSQLManager::CSQLManager( HDBPROVIDER Connect, bool bAutoCommit )
{
    m_pSQLBridge = &g_SQLBridge;

    m_isCompiled  = false;
    m_bShowAlert  = true;
    m_Connect     = Connect;
    m_bAutoCommit = bAutoCommit;

    m_Buff = new CSQLBuff( Connect );
    m_Bind = new CSQLBind( Connect );
//  m_Data = new CSQLData( Connect );

    m_DataPtrBuff = new CVectorOfPtr<CSmPt>( false ); //, MIN_EXT_SIZE
    m_DataPtrBind = new CVectorOfPtr<CSmPt>( false ); //, MIN_EXT_SIZE

    iHstmt = SQL_NULL_HANDLE;
}

void
CSQLManager::ShowAlert( bool bAlert )
{
    m_bShowAlert = bAlert;
}

CSQLManager::~CSQLManager()
{
    Close();

    if( iHstmt )
    {
        CSqlConnectInfo *session = GetSession( m_Connect );
        if( session != NULL )
        {
            if( session->m_nDB == ID_PDATABASES_SQLITE )
            {
                /*SQLRETURN rc =*/ sqlite3_finalize( (sqlite3_stmt*)iHstmt );
                // if( SQL_SUCCESS != rc )
                // {
                //     if( m_bShowAlert )
                //     {
                //         if( session->GetErrorInfo( iHstmt ) )
                //         {
                //             m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //         }
                //     }
                // }
            }
            else
            if( session->m_nDB == ID_PDATABASES_MYSQL )
            {
                /*bool rc =*/ mysql_stmt_close( (MYSQL_STMT*)iHstmt );
                // if( !rc )
                // {
                //     if( m_bShowAlert )
                //     {
                //         if( session->GetErrorInfo( iHstmt ) )
                //         {
                //             m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //         }
                //     }
                // }
            }
            else
            if( session->m_nDB == ID_PDATABASES_ODBC )
            {
                /*SQLRETURN rc =*/ SQLFreeStmt( iHstmt, SQL_DROP );
                //SQLRETURN rc = SQLFreeHandle( SQL_HANDLE_STMT, iHstmt );
                // if( SQL_SUCCESS != rc )
                // {
                //     if( m_bShowAlert )
                //     {
                //         if( session->GetErrorInfo( iHstmt ) )
                //         {
                //             m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //         }
                //     }
                // }
            }
        }
    }
    
    iHstmt = SQL_NULL_HANDLE;

    delete m_Buff;
    delete m_Bind;
//  delete m_Data;

    delete m_DataPtrBuff;
    delete m_DataPtrBind;
}

CSqlConnectInfo*
CSQLManager::GetSession( HDBPROVIDER hconnect )
{
    long connect = m_pSQLBridge->findIndex( hconnect );
    if( connect >= 0 )
    {
        return m_pSQLBridge->Connects()->at( m_pSQLBridge->findIndex( hconnect ) );
    }
    return NULL;
}

bool
CSQLManager::Compile( CSQLPipe *pipe )
{
    return Compile( pipe, GetSession( m_Connect ) );
}

bool
CSQLManager::Execute( CSQLPipe *pipe )
{
    return Execute( pipe, GetSession( m_Connect ) );
}

bool
CSQLManager::Fetch()
{
    return Fetch( iHstmt );
}

void
CSQLManager::Close()
{
    Close( GetSession( m_Connect ), iHstmt );
}

void
CSQLManager::Commit()
{
    Commit( GetSession( m_Connect ), iHstmt );
}

void
CSQLManager::Rollback()
{
    Rollback( GetSession( m_Connect ), iHstmt );
}

bool
CSQLManager::Compile( CSQLPipe *pipe, CSqlConnectInfo *session )
{
    if( !m_isCompiled )
    {
        CVectorOfPtr<CSmPt> *datas = pipe->m_Pipes;

        m_DataPtrBuff->Flush();
        m_DataPtrBind->Flush();

        for( unsigned long i = 0; i < datas->size(); i ++ ) {
            if( sqlBuff == datas->at( i )->DataState() )
                m_DataPtrBuff->push_back( datas->at( i ) );
            else 
            if( sqlBind == datas->at( i )->DataState() )
                m_DataPtrBind->push_back( datas->at( i ) );
        }

        if( Open() )
        {
            SQLTCHAR *query = (SQLTCHAR*)pipe->m_PipeString->c_str();
            long length = pipe->m_PipeString->length();

            if( session != NULL )
            {
                if( session->m_nDB == ID_PDATABASES_SQLITE )
                {
                    sqlite3      *connect = (sqlite3 *) session->iHdbc;
                    sqlite3_stmt *stmt    = SQL_NULL_HANDLE;
                    SQLRETURN rc = sqlite3_prepare( connect, (const char*)query, -1, &stmt, NULL );
                    if( SQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                    iHstmt = (SQLHSTMT)stmt;
                }
                else 
                if( session->m_nDB == ID_PDATABASES_MYSQL )
                {
                    MYSQL_STMT *stmt = (MYSQL_STMT *) iHstmt;

                    SQLRETURN rc = mysql_stmt_prepare( stmt, (const char*)query, length );

                    if( SQL_SUCCESS == rc )
                        rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );
                    
                    if( SQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                else 
                if( session->m_nDB == ID_PDATABASES_ODBC )
                {
                    SQLRETURN rc = SQLPrepare( iHstmt, query, length ); //SQL_NTS );

                    if( SQL_SUCCESS == rc )
                        rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );

                    if( SQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }

                m_isCompiled = true;
            }
        }
    }

    return true;
}

bool
CSQLManager::Execute( CSQLPipe *pipe, CSqlConnectInfo *session )
{
    if( Compile( pipe, session ) )
    {
        SQLRETURN rc = m_Bind->Execute( iHstmt, m_DataPtrBind );
        if( SQL_SUCCESS == rc )
        {
            CSqlConnectInfo *session = GetSession( m_Connect );
            if( session != NULL )
            {
                if( session->m_nDB == ID_PDATABASES_SQLITE )
                {
                    rc = sqlite3_step( (sqlite3_stmt*)iHstmt );
                    if( SQLITE_OK != rc && SQLITE_DONE != rc && SQLITE_ROW  != rc )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                else
                if( session->m_nDB == ID_PDATABASES_MYSQL )
                {
                    MYSQL_STMT *stmt = (MYSQL_STMT *) iHstmt;
                    rc = mysql_stmt_execute( stmt );
                    if( SQL_SUCCESS != rc && SQL_NO_DATA_FOUND != rc )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                else
                if( session->m_nDB == ID_PDATABASES_ODBC )
                {
                    rc = SQLExecute( iHstmt );
                    if( SQL_SUCCESS != rc && SQL_NO_DATA_FOUND != rc )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
            }
        }

        if( m_DataPtrBuff->size() == 0L )
        {
            if( m_bAutoCommit ) Commit();

            Close();
        }

        return true;
    }

    return false;
}

bool
CSQLManager::Fetch( SQLHSTMT query )
{
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session ) 
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
            SQLRETURN rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );
            if( SQL_SUCCESS != rc )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                    }
                }
                return false;
            }

            rc = sqlite3_step( (sqlite3_stmt*)query );
            if( SQLITE_ROW != rc )
            {
                if( SQLITE_DONE == rc )
                {
                    if( m_bAutoCommit ) Commit();
                    Close();
                }
                else
                {
                    if( m_bShowAlert )
                    {
                        if( session->GetErrorInfo( query ) )
                        {
                            m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                        }
                    }
                }
                return false;
            }
        }
        else 
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            m_Buff->ClearBuffs();

            MYSQL_STMT *stmt = (MYSQL_STMT *) iHstmt;
            SQLRETURN rc = mysql_stmt_fetch( stmt );
            if( SQL_SUCCESS != rc )
            {
                if( SQL_NO_DATA_FOUND == rc )
                {
                    if( m_bAutoCommit ) Commit();
                    Close();
                }
                else
                {
                    if( m_bShowAlert )
                    {
                        if( session->GetErrorInfo( query ) )
                        {
                            m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                        }
                    }
                }
                return false;
            }
            else
            {
            }
        }
        else
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            SQLRETURN rc = SQLFetch( query );
            if( SQL_SUCCESS != rc )
            {
                if( SQL_NO_DATA_FOUND == rc )
                {
                    if( m_bAutoCommit ) Commit();
                    Close();
                }
                else
                {
                    if( m_bShowAlert )
                    {
                        if( session->GetErrorInfo( query ) )
                        {
                            m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                        }
                    }
                }
                return false;
            }
            else
            {
                //SQLRETURN rc = m_Data->Execute( query, m_DataPtrBuff );
                //if( SQL_SUCCESS != rc )
                //    return false;
            }
        }
    }
    return true;
}

bool
CSQLManager::Open()
{
    if( iHstmt != SQL_NULL_HANDLE ) return true;

    bool res = false;
            
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session ) 
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
            return true;
        }
        else 
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            MYSQL_STMT *stmt = mysql_stmt_init( (MYSQL*)session->iHdbc );
            if( stmt )
            {
                iHstmt = (SQLHSTMT)stmt;
                res = true;
            }
            else
            {
                if( m_bShowAlert )
                {
                    iHstmt = SQL_NULL_HANDLE;
                    if( session->GetErrorInfo( iHstmt ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                    }
                }
            }
        }
        else 
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            CSqlConnectInfo *session = GetSession( m_Connect );
            SQLRETURN rc = SQLAllocHandle( SQL_HANDLE_STMT, session->iHdbc, &iHstmt );
            if( SQL_SUCCESS == rc )
            {
            //     UDWORD TimeOut = 5;
            //     rc = SQLSetStmtOption( iHstmt, SQL_QUERY_TIMEOUT, TimeOut );
            //     if( SQL_SUCCESS != rc )
            //     {
            //        if( m_bShowAlert )
            //        {
            //           session->GetErrorInfo( iHstmt );
            //           m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
            //        }
            //     }
                res = true;
            }
            else
            {
                if( m_bShowAlert )
                {
                    iHstmt = SQL_NULL_HANDLE;
                    if( session->GetErrorInfo( iHstmt ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                    }
                }
            }
        }
    }
    
    return res;
}

void
CSQLManager::Close( CSqlConnectInfo *session, SQLHSTMT query )
{
    if( query == SQL_NULL_HANDLE ) return;
    
    if( session != NULL )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
            SQLRETURN rc = sqlite3_reset( (sqlite3_stmt*)query );
            if( SQL_SUCCESS != rc )
            {
                // if( m_bShowAlert )
                // {
                //     if( session->GetErrorInfo( query ) )
                //     {
                //         m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //     }
                // }
            }

            rc = sqlite3_clear_bindings( (sqlite3_stmt*)query );
            if( SQL_SUCCESS != rc )
            {
                // if( m_bShowAlert )
                // {
                //     if( session->GetErrorInfo( query ) )
                //     {
                //         m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //     }
                // }
            }
        }
        else 
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
            bool rc = mysql_stmt_free_result( (MYSQL_STMT*)query );
            if( !rc )
            {
                // if( m_bShowAlert )
                // {
                //     if( session->GetErrorInfo( query ) )
                //     {
                //         m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //     }
                // }
            }
        }
        else 
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            SQLRETURN 
            //  rc = SQLFreeStmt( query, SQL_CLOSE );
            //  if( SQL_SUCCESS == rc )
            //  rc = SQLFreeStmt( query, SQL_UNBIND );
            //  if( SQL_SUCCESS == rc )
            rc = SQLFreeStmt( query, SQL_RESET_PARAMS );
            if( SQL_SUCCESS != rc )
            {
                // if( m_bShowAlert )
                // {
                //     if( session->GetErrorInfo( query ) )
                //     {
                //         m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                //     }
                // }
            }
        }
    }
}

long
CSQLManager::last_insert_rowid( CSqlConnectInfo *session )
{
    if( session->m_nDB == ID_PDATABASES_SQLITE )
    {
        sqlite3 *connect = (sqlite3 *) session->iHdbc;
        return sqlite3_last_insert_rowid( connect );
    }
    else 
    if( session->m_nDB == ID_PDATABASES_MYSQL )
    {
        //MYSQL* connect = (MYSQL *) session->iHdbc;
        //return mysql_insert_id( connect )
        if( iHstmt != NULL )
        {
            MYSQL_STMT *stmt = (MYSQL_STMT*) this->iHstmt;
            return mysql_stmt_insert_id( stmt );
        }
    }
    else 
    if( session->m_nDB == ID_PDATABASES_ODBC )
    {
    }
    return 0;
}

long
CSQLManager::last_insert_rowid()
{
    return last_insert_rowid( GetSession( m_Connect ) );
}

PDatabases 
CSQLManager::getDatabase()
{
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        return session->getDatabase();
    }
    return ID_PDATABASES_NONE;
}

void
CSQLManager::Commit( CSqlConnectInfo *session, SQLHSTMT query )
{
    if( !session->m_IsAutoCommit )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else 
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
        }
        else 
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, session->iHdbc, SQL_COMMIT );
            if( SQL_SUCCESS != rc )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                    }
                }
            }
        }
    }
}

void
CSQLManager::Rollback( CSqlConnectInfo *session, SQLHSTMT query )
{
    if( !session->m_IsAutoCommit )
    {
        if( session->m_nDB == ID_PDATABASES_SQLITE )
        {
        }
        else 
        if( session->m_nDB == ID_PDATABASES_MYSQL )
        {
        }
        else 
        if( session->m_nDB == ID_PDATABASES_ODBC )
        {
            SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, session->iHdbc, SQL_ROLLBACK );
            if( SQL_SUCCESS != rc )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (TCHAR*)NULL );
                    }
                }
            }
        }
    }
}

void
CSQLManager::Flush()
{
    Close();

    m_DataPtrBuff->Flush();
    m_DataPtrBind->Flush();

    m_isCompiled = false;
}
