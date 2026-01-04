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

CSQLBuilder::CSQLBuilder( PHDBPROVIDER Connect )
: m_Connect( Connect )
{
    m_pSQLBridge = &g_SQLBridge;
}

CSqlConnectInfo*
CSQLBuilder::GetSession( PHDBPROVIDER hconnect )
{
    long connect = m_pSQLBridge->findIndex( hconnect );
    if( connect >= 0 )
    {
        return m_pSQLBridge->Connects()->at( m_pSQLBridge->findIndex( hconnect ) );
    }
    return NULL;
}

PSQLRETURN
CSQLBuilder::Execute( PSQLHSTMT query, CVectorOfPtr<CSmPt> *in )
{
    this->Clear();
    
    PSQLRETURN rc = this->Before( query );
    if( rc != PSQL_SUCCESS )
    {
        return rc;
    }

    for( unsigned short i = 0; i < in->size(); i ++ )
    {
        CSmPt *db_data = in->at( i );
        rc = AccessSingleMethod( query, (unsigned short)i, db_data->DataType(), db_data );
        if( rc != PSQL_SUCCESS )
        {
            return rc;
        }
    }

    return this->After( query );
}

CSQLBind::CSQLBind( PHDBPROVIDER Connect )
: CSQLBuilder( Connect )
{
}

void
CSQLBind::Clear()
{
#ifdef DEFINE_MYSQL
    m_Binds.clear();
#endif
#ifdef DEFINE_POSTGRESQL
    m_Values .clear();
    m_Lengths.clear();
    m_Formats.clear();
#endif
}

PSQLRETURN
CSQLBind::Before( PSQLHSTMT /*query*/ )
{
    return PSQL_SUCCESS;
}

PSQLRETURN
CSQLBind::After( PSQLHSTMT query )
{
    PSQLRETURN rc = PSQL_SUCCESS;
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        switch( session->m_nDB )
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
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
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
    }
    return rc;
}

PSQLRETURN
CSQLBind::AccessSingleMethod( PSQLHSTMT query, unsigned short num, short type, CSmPt *db_data )
{
    void      *var    = db_data->DataPointer();
    long       MaxLen = db_data->DataSize();
    PSQLRETURN rc     = PSQL_SUCCESS;

    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        switch( session->m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
            num = num + 1;
            switch( type )
            {
            case datShort  : {
                int value = *(int*)var;
                rc = sqlite3_bind_int( (sqlite3_stmt*)query, num, value );
            } break;
            case datLong   : {
                long value = *(long*)var;
                rc = sqlite3_bind_int64( (sqlite3_stmt*)query, num, value );
            } break;
            case datFloat  : {
                double value = *(double*)var;
                rc = sqlite3_bind_double( (sqlite3_stmt*)query, num, value );
            } break;
            case datDouble : {
                double value = *(double*)var;
                rc = sqlite3_bind_double( (sqlite3_stmt*)query, num, value );
            } break;
            case datChars  :
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
            case datBytes    : {
                rc = sqlite3_bind_blob64( (sqlite3_stmt*)query, num, var, MaxLen, NULL );
            } break;
            case datDate     : break;
            case datTime     : break;
            case datTimeStamp: break;
            }
        }
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
        {
            MYSQL_BIND Bind; memset( &Bind, 0, sizeof( MYSQL_BIND ) );
            switch( type )
            {
            case datShort         : Bind.buffer_type = MYSQL_TYPE_SHORT    ; break;
            case datLong          : Bind.buffer_type = MYSQL_TYPE_LONG     ; break;
            case datFloat         : Bind.buffer_type = MYSQL_TYPE_FLOAT    ; break;
            case datDouble        : Bind.buffer_type = MYSQL_TYPE_DOUBLE   ; break;
            case datBytes         : Bind.buffer_type = MYSQL_TYPE_LONG_BLOB; break;
            case datDate          : Bind.buffer_type = MYSQL_TYPE_DATE     ; break;
            case datTime          : Bind.buffer_type = MYSQL_TYPE_TIME     ; break;
            case datTimeStamp     : Bind.buffer_type = MYSQL_TYPE_DATETIME ; break;
            case datChars         :
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
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            switch( type )
            {
            case datShort  :
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_SHORT, SQL_SMALLINT, 0, 0, var, 0, &MaxLen );
                break;
            case datLong  : {
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, var, 0, &MaxLen );
            }
            break;
            case datFloat :
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_FLOAT, 0, 0, var, 0, &MaxLen );
                break;
            case datDouble    :
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, var, 0, &MaxLen );
                break;
            case datChars   : {
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
            case datBytes : {
                static long length = SQL_DATA_AT_EXEC;
                db_data->DataPt.DataLength = MaxLen;
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_VARBINARY, MaxLen, 0, var, 0, &length );
            }
            break;
            case datDate:
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, 0, var, 0, &MaxLen );
                break;
            case datTime:
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_TYPE_TIME, SQL_TYPE_TIME, 0, 0, var, 0, &MaxLen );
                break;
            case datTimeStamp:
                rc = SQLBindParameter( query, num, SQL_PARAM_INPUT, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 0, 0, var, 0, &MaxLen );
                break;
            }
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            if( type == datChars )
            {
                const char * value = (const char*)var;
                m_Lengths.push_back( value ? strlen( value ) : 0 );
                m_Formats.push_back( 0 );
            }
            else
            {
                m_Lengths.push_back( MaxLen );
                m_Formats.push_back( 1 );
            }
            m_Values.push_back( (char*)var );
        }
        break;
#endif
        default: break;
        }
    }

    return rc;
}

CSQLBuff::CSQLBuff( PHDBPROVIDER Connect )
: CSQLBuilder( Connect )
{
}

void
CSQLBuff::Clear()
{
#ifdef DEFINE_MYSQL
    m_Buffs.clear();
#endif
}

PSQLRETURN
CSQLBuff::Before( PSQLHSTMT /*query*/ )
{
    return PSQL_SUCCESS;
}

void
CSQLBuff::ClearBuffs()
{
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        switch( session->m_nDB )
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
            std::for_each( m_Buffs.begin(), m_Buffs.end(),
                []( auto Item ){ memset( Item.buffer, 0, Item.buffer_length ); }
            );
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
        }
        break;
#endif
        }
    }
}

PSQLRETURN
CSQLBuff::After( PSQLHSTMT query )
{
    PSQLRETURN rc = PSQL_SUCCESS;
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        switch( session->m_nDB )
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
            if( m_Buffs.size() > 0 )
            {
                MYSQL_STMT *stmt = (MYSQL_STMT*)query;
                //rc = mysql_stmt_attr_set( stmt, STMT_ATTR_PREFETCH_ROWS, &size );
                //rc = mysql_stmt_fetch_column( stmt, m_pBuffs, 0, 0 );
                //rc = mysql_stmt_bind_result( stmt, m_pBuffs );
                rc = mysql_stmt_bind_result( stmt, m_Buffs.data() );
            }
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
        }
        break;
#endif
        }
    }
    return rc;
}

PSQLRETURN
CSQLBuff::AccessSingleMethod( PSQLHSTMT query, unsigned short num, short type, CSmPt *db_data )
{
    void     *var    = db_data->DataPointer();
    long      MaxLen = db_data->DataSize();
    PSQLRETURN rc     = PSQL_SUCCESS;

    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
        switch( session->m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
            sqlite3_stmt *stmt = (sqlite3_stmt*)query;
            switch( type )
            {
                case datShort : {
                    int res = sqlite3_column_int( stmt, num );
                    *((long*)var) = res;
                }
                break;
                case datFloat  : {
                    double res = sqlite3_column_double( stmt, num );
                    *((float*)var) = res;
                }
                break;
                case datDouble : {
                    double res = sqlite3_column_double( stmt, num );
                    *((double*)var) = res;
                }
                break;
                case datChars   : {
                    const char *res = (const char*)sqlite3_column_text( stmt, num );
                    strcpy( (char*)var, res );
                }
                break;
                case datBytes    : {
                    const void *res = sqlite3_column_blob( stmt, num );
                    var = (void*)res;
                }
                break;
                case datDate     : break;
                case datTime     : break;
                case datTimeStamp: break;
            }
        }
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
        {
            MYSQL_BIND Bind; memset( &Bind, 0, sizeof( MYSQL_BIND ) );
            switch( type )
            {
                case datShort         : Bind.buffer_type = MYSQL_TYPE_SHORT    ; break;
                case datLong          : Bind.buffer_type = MYSQL_TYPE_LONG     ; break;
                case datFloat         : Bind.buffer_type = MYSQL_TYPE_FLOAT    ; break;
                case datDouble        : Bind.buffer_type = MYSQL_TYPE_DOUBLE   ; break;
                case datBytes         : Bind.buffer_type = MYSQL_TYPE_LONG_BLOB; break;
                case datDate          : Bind.buffer_type = MYSQL_TYPE_DATE     ; break;
                case datTime          : Bind.buffer_type = MYSQL_TYPE_TIME     ; break;
                case datTimeStamp     : Bind.buffer_type = MYSQL_TYPE_DATETIME ; break;
                case datChars         :
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
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            switch( type )
            {
                case datLong : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, 0, &db_data->DataPt.DataLength );
                }
                break;
                case datFloat : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, 0, &db_data->DataPt.DataLength );
                }
                break;
                case datDouble : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datChars : {
                    db_data->DataPt.DataLength = SQL_NTS;
                    rc = SQLBindCol( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datBytes : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLBindCol( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datDate: {
                    rc = SQLBindCol( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case datTime: {
                    rc = SQLBindCol( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case datTimeStamp: {
                    rc = SQLBindCol( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
            }
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
        }
        break;
#endif
        }
    }
    return rc;
}

CSQLData::CSQLData( PHDBPROVIDER Connect )
: CSQLBuilder( Connect )
{
}

void
CSQLData::Clear()
{
}

PSQLRETURN
CSQLData::Before( PSQLHSTMT )
{
    return PSQL_SUCCESS;
}

PSQLRETURN
CSQLData::After( PSQLHSTMT )
{
    return PSQL_SUCCESS;
}

PSQLRETURN
CSQLData::AccessSingleMethod( PSQLHSTMT query, unsigned short num, short type, CSmPt *db_data )
{
    void     *var    = db_data->DataPointer();
    long      MaxLen = db_data->DataSize();
    PSQLRETURN rc     = PSQL_SUCCESS;
    
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session != NULL )
    {
    switch( session->m_nDB )
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
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            switch( type )
            {
                case datLong : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datFloat : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datDouble : {
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datChars : {
                    db_data->DataPt.DataLength = SQL_NTS;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                }
                break;
                case datBytes :
                    db_data->DataPt.DataLength = SQL_NULL_DATA;
                    rc = SQLGetData( query, num, type, var, MaxLen, &db_data->DataPt.DataLength );
                    break;
                case datDate: {
                    rc = SQLGetData( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case datTime: {
                    rc = SQLGetData( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
                case datTimeStamp: {
                    rc = SQLGetData( query, num, type, var, MaxLen, &MaxLen );
                }
                break;
            }
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
        }
        break;
#endif
    }
    }

    return rc;
}

CSQLManager::CSQLManager( PHDBPROVIDER Connect, bool bAutoCommit )
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

    iHstmt = PSQL_NULL_HANDLE;

    m_Rows = 0;
    m_Cols = 0;
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
            switch( session->m_nDB )
            {
            case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
            case ID_PDATABASES_SQLITE :
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
            break;
#endif
#ifdef DEFINE_MYSQL
            case ID_PDATABASES_MYSQL :
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
            break;
#endif
#ifdef DEFINE_ODBC
            case ID_PDATABASES_ODBC :
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
            break;
#endif
#ifdef DEFINE_POSTGRESQL
            case ID_PDATABASES_POSTGRESQL :
            {
                //PQclear( (PGresult*)iHstmt );
            }
            break;
#endif
#ifdef DEFINE_ORACLE
            case ID_PDATABASES_ORACLE :
            {
                OCIHandleFree((dvoid *)iHstmt, OCI_HTYPE_STMT);
            }
            break;
#endif
            }
        }
    }
    
    iHstmt = PSQL_NULL_HANDLE;

    delete m_Buff;
    delete m_Bind;
//  delete m_Data;

    delete m_DataPtrBuff;
    delete m_DataPtrBind;
}

CSqlConnectInfo*
CSQLManager::GetSession( PHDBPROVIDER hconnect )
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
            PTCCHAR *query = pipe->m_PipeString->c_str();
            long length = pipe->m_PipeString->length();

            if( session != NULL )
            {
                switch( session->m_nDB )
                {
                case ID_PDATABASES_NONE : return false;
#ifdef DEFINE_SQLITE
                case ID_PDATABASES_SQLITE :
                {
                    sqlite3      *connect = (sqlite3 *) session->iHdbc;
                    sqlite3_stmt *stmt    = PSQL_NULL_HANDLE;
                    PSQLRETURN rc = sqlite3_prepare( connect, query, -1, &stmt, NULL );
                    if( PSQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                    iHstmt = (PSQLHSTMT)stmt;
                }
                break;
#endif
#ifdef DEFINE_MYSQL
                case ID_PDATABASES_MYSQL :
                {
                    MYSQL_STMT *stmt = (MYSQL_STMT *) iHstmt;

                    PSQLRETURN rc = mysql_stmt_prepare( stmt, query, length );

                    if( PSQL_SUCCESS == rc )
                        rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );
                    
                    if( PSQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                break;
#endif
#ifdef DEFINE_ODBC
                case ID_PDATABASES_ODBC :
                {
                    SQLRETURN rc = SQLPrepare( iHstmt, (SQLCHAR*)query, length ); //SQL_NTS );

                    if( SQL_SUCCESS == rc )
                        rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );

                    if( SQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                break;
#endif
#ifdef DEFINE_POSTGRESQL
                case ID_PDATABASES_POSTGRESQL :
                {
                    PGconn *conn = (PGconn *) session->iHdbc;

                    PGresult *res = PQprepare( conn, query, query, m_DataPtrBind->size(), NULL );

                    ExecStatusType est = PQresultStatus( res );
                    PSQLRETURN rc = est == PGRES_COMMAND_OK ? PSQL_SUCCESS : PSQL_ERROR;

                    if( PSQL_SUCCESS == rc )
                    {
                        //PGresult *params = PQdescribePrepared( conn, stmtName );
                    }

                    if( PSQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        PQclear( res );
                        return false;
                    }
                    PQclear( res );
                }
                break;
#endif
#ifdef DEFINE_ORACLE
                case ID_PDATABASES_ORACLE :
                {
                    ub4 language = OCI_DEFAULT; ub4 mode = OCI_DEFAULT;
                    OCIError *errhp = nullptr;
                    sword rc = OCIStmtPrepare( (OCIStmt*)iHstmt, errhp, (text *)query, length, language, mode );
                    if( OCI_SUCCESS == rc )
                        rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );

                    if( SQL_SUCCESS != rc )
                    {
                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                break;
#endif
                default : return false;
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
        PSQLRETURN rc = m_Bind->Execute( iHstmt, m_DataPtrBind );
        if( PSQL_SUCCESS == rc )
        {
            CSqlConnectInfo *session = GetSession( m_Connect );
            if( session != NULL )
            {
                switch( session->m_nDB )
                {
                case ID_PDATABASES_NONE : return false;
#ifdef DEFINE_SQLITE
                case ID_PDATABASES_SQLITE :
                {
                    rc = sqlite3_step( (sqlite3_stmt*)iHstmt );
                    if( SQLITE_OK != rc && SQLITE_DONE != rc && SQLITE_ROW  != rc )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                break;
#endif
#ifdef DEFINE_MYSQL
                case ID_PDATABASES_MYSQL :
                {
                    MYSQL_STMT *stmt = (MYSQL_STMT *) iHstmt;
                    rc = mysql_stmt_execute( stmt );
                    if( PSQL_SUCCESS != rc && PSQL_NO_DATA_FOUND != rc )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                break;
#endif
#ifdef DEFINE_ODBC
                case ID_PDATABASES_ODBC :
                {
                    rc = SQLExecute( iHstmt );
                    if( SQL_SUCCESS != rc && SQL_NO_DATA_FOUND != rc )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }
                }
                break;
#endif
#ifdef DEFINE_POSTGRESQL
                case ID_PDATABASES_POSTGRESQL :
                {
                    int resultFormat = 1; // 0 for text format, 1 for binary format
                    PTCCHAR *query = pipe->m_PipeString->c_str();
                    PGconn *conn = (PGconn *) session->iHdbc;
                    PGresult *res = PQexecPrepared( conn, query, m_DataPtrBind->size(),
                                                   m_Bind->m_Values.data(), m_Bind->m_Lengths.data(), m_Bind->m_Formats.data(), resultFormat );
                    iHstmt = (PSQLHSTMT)res;

                    ExecStatusType est = PQresultStatus( res );
                    if ( est != PGRES_COMMAND_OK && est != PGRES_TUPLES_OK )
                    {
                        if( m_bAutoCommit ) Rollback();

                        if( m_bShowAlert )
                        {
                            if( session->GetErrorInfo( iHstmt ) )
                            {
                                m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                            }
                        }
                        return false;
                    }

                    if( est == PGRES_TUPLES_OK )
                    {
                        m_Rows = PQntuples( res );
                        m_Cols = PQnfields( res );
                    }
                }
                break;
#endif
                default : return false;
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
CSQLManager::Fetch( PSQLHSTMT query )
{
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session ) 
    {
        switch( session->m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
            PSQLRETURN rc = m_Buff->Execute( iHstmt, m_DataPtrBuff );
            if( PSQL_SUCCESS != rc )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
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
                            m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                        }
                    }
                }
                return false;
            }
        }
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
        {
            m_Buff->ClearBuffs();

            MYSQL_STMT *stmt = (MYSQL_STMT *) iHstmt;
            PSQLRETURN rc = mysql_stmt_fetch( stmt );
            if( PSQL_SUCCESS != rc )
            {
                if( PSQL_NO_DATA_FOUND == rc )
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
                            m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                        }
                    }
                }
                return false;
            }
            else
            {
            }
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
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
                            m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
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
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            if( m_Row < m_Rows )
            {
                PGresult *res = (PGresult *) iHstmt;
                for( unsigned short i = 0; i < m_DataPtrBuff->size() && i < m_Cols; i ++ )
                {
                    CSmPt *db_data = m_DataPtrBuff->at( i );
                    long   MaxLen = db_data->DataSize();
                    int    length = PQgetlength( res, m_Row, i );
                    char  *buff =  PQgetvalue( res, m_Row, i );
                    memcpy( db_data->DataPointer(), buff, MaxLen > length ? length : MaxLen );
                }
                m_Row ++;
                if( m_Row >= m_Rows )
                {
                    if( m_bAutoCommit ) Commit();
                    Close();
                    return false;
                }
            }
            else
                return false;
        }
        break;
#endif
        }
    }
    return true;
}

bool
CSQLManager::Open()
{
    if( iHstmt != PSQL_NULL_HANDLE ) return true;

    bool res = false;
            
    CSqlConnectInfo *session = GetSession( m_Connect );
    if( session ) 
    {
        switch( session->m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
            return true;
        }
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
        {
            MYSQL_STMT *stmt = mysql_stmt_init( (MYSQL*)session->iHdbc );
            if( stmt )
            {
                iHstmt = (PSQLHSTMT)stmt;
                res = true;
            }
            else
            {
                if( m_bShowAlert )
                {
                    iHstmt = PSQL_NULL_HANDLE;
                    if( session->GetErrorInfo( iHstmt ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
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
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            return true;
        }
        break;
#endif
#ifdef DEFINE_ORACLE
        case ID_PDATABASES_ORACLE :
        {
            CSqlConnectInfo *session = GetSession( m_Connect );
            OCIEnv *envhp = (OCIEnv *)session->iHdbc;
            OCIStmt *stmthp = nullptr;
            sword rc = OCIHandleAlloc((dvoid *)envhp, (dvoid **)&stmthp, OCI_HTYPE_STMT, (size_t)0, (dvoid **)0);
            if( SQL_SUCCESS == rc )
            {
                iHstmt = stmthp;
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
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
        }
    }
    
    return res;
}

void
CSQLManager::Close( CSqlConnectInfo *session, PSQLHSTMT query )
{
    if( query == PSQL_NULL_HANDLE ) return;
    
    if( session != NULL )
    {
        switch( session->m_nDB )
        {
        case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
        case ID_PDATABASES_SQLITE :
        {
            PSQLRETURN rc = sqlite3_reset( (sqlite3_stmt*)query );
            if( PSQL_SUCCESS != rc )
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
            if( PSQL_SUCCESS != rc )
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
        break;
#endif
#ifdef DEFINE_MYSQL
        case ID_PDATABASES_MYSQL :
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
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
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
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            //PQclear( (PGresult*)query );
        }
        break;
#endif
#ifdef DEFINE_ORACLE
        case ID_PDATABASES_ORACLE :
        {
            sword rc = OCIHandleFree((dvoid *)query, OCI_HTYPE_STMT);
            if( OCI_SUCCESS != rc )
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
        break;
#endif
        }
    }
}

long
CSQLManager::last_insert_rowid( CSqlConnectInfo *session )
{
    switch( session->m_nDB )
    {
    case ID_PDATABASES_NONE : {} break;
#ifdef DEFINE_SQLITE
    case ID_PDATABASES_SQLITE :
    {
        sqlite3 *connect = (sqlite3 *) session->iHdbc;
        return sqlite3_last_insert_rowid( connect );
    }
    break;
#endif
#ifdef DEFINE_MYSQL
    case ID_PDATABASES_MYSQL :
    {
        //MYSQL* connect = (MYSQL *) session->iHdbc;
        //return mysql_insert_id( connect )
        if( iHstmt != NULL )
        {
            MYSQL_STMT *stmt = (MYSQL_STMT*) this->iHstmt;
            return mysql_stmt_insert_id( stmt );
        }
    }
    break;
#endif
#ifdef DEFINE_ODBC
    case ID_PDATABASES_ODBC :
    {
    }
    break;
#endif
#ifdef DEFINE_POSTGRESQL
    case ID_PDATABASES_POSTGRESQL :
    {
        if( m_Rows > 0 && m_Cols > 0 )
        {
            PGresult* res = (PGresult*)this->iHstmt;
            return std::stol( PQgetvalue( res, ++ m_Row, 0 ) );
        }
    }
    break;
#endif
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
CSQLManager::Commit( CSqlConnectInfo *session, PSQLHSTMT query )
{
    if( !session->m_IsAutoCommit )
    {
        switch( session->m_nDB )
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
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, session->iHdbc, SQL_COMMIT );
            if( SQL_SUCCESS != rc )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            PGconn *conn = (PGconn *) session->iHdbc;
            PGresult *res = PQexec( conn, _T( "COMMIT" ) );
            if( PQresultStatus( res ) != PGRES_COMMAND_OK )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
        }
    }
}

void
CSQLManager::Rollback( CSqlConnectInfo *session, PSQLHSTMT query )
{
    if( !session->m_IsAutoCommit )
    {
        switch( session->m_nDB )
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
        }
        break;
#endif
#ifdef DEFINE_ODBC
        case ID_PDATABASES_ODBC :
        {
            SQLRETURN rc = SQLEndTran( SQL_HANDLE_DBC, session->iHdbc, SQL_ROLLBACK );
            if( SQL_SUCCESS != rc )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
#ifdef DEFINE_POSTGRESQL
        case ID_PDATABASES_POSTGRESQL :
        {
            PGconn *conn = (PGconn *) session->iHdbc;
            PGresult *res = PQexec( conn, _T( "ROLLBACK" ) );
            if( PQresultStatus( res ) != PGRES_COMMAND_OK )
            {
                if( m_bShowAlert )
                {
                    if( session->GetErrorInfo( query ) )
                    {
                        m_pSQLBridge->ShowSQLErrorNow( session->m_Error, (PTCHAR*)NULL );
                    }
                }
            }
        }
        break;
#endif
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

    m_Row = 0;
    m_Rows = 0;

    m_Cols = 0;
}
