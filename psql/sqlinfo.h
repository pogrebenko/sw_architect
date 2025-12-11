/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#ifndef SQLINFO_H
#define SQLINFO_H

#include "common/export.h"
#include "common/VectorOfPtr.h"

#include "psql/sqltype.h"

#include <bits/pthreadtypes.h>

extern void (*g_ShowSQLErrorNow)( const char* );

struct TSqlErrorInfo 
{
   SQLTCHAR SqlError[SQL_MAX_MESSAGE_LENGTH];
   SQLTCHAR SqlState[32];
   int      ErrorNum;

   inline TSqlErrorInfo() { init(); }
   inline TSqlErrorInfo( TSqlErrorInfo &c );
   inline virtual ~TSqlErrorInfo() {}

   TSqlErrorInfo& operator = ( TSqlErrorInfo &c )
   {
       ErrorNum   = c.ErrorNum;
       _tcscpy( (TCHAR*)SqlError, (const TCHAR*)c.SqlError );
       _tcscpy( (TCHAR*)SqlState, (const TCHAR*)c.SqlState );
       return *this;
   }

   void init()
   {
       ErrorNum = 0;

       memset( SqlError, 0, sizeof( SqlError ) );
       memset( SqlState, 0, sizeof( SqlState ) );
   }
};

struct TSqlConnectInfo
{
    SQLTCHAR   m_ServerName[ 64 ];
    SQLTCHAR   m_BaseName  [ 32 ];
    SQLTCHAR   m_UserName  [ 32 ];
    SQLTCHAR   m_UserPass  [ 32 ];
    SQLTCHAR   m_SysAdmin  [ 32 ];
    int        m_Port;
    SQLTCHAR   m_UnixSocket[ 64 ];

    int        m_UserCode    ;
    bool       m_IsOpen      ;
    bool       m_IsAutoCommit;

    SQLHENV    iHenv;
    SQLHDBC    iHdbc;

    PDatabases m_nDB;
    
    inline TSqlConnectInfo() { init(); }
    inline TSqlConnectInfo( TSqlConnectInfo &c );
    inline virtual ~TSqlConnectInfo() {}
    
    TSqlConnectInfo& operator = ( TSqlConnectInfo &c )
    {
        m_nDB   = c.m_nDB;
        
        _tcscpy( (TCHAR*)m_ServerName, (const TCHAR*)c.m_ServerName );
        _tcscpy( (TCHAR*)m_BaseName  , (const TCHAR*)c.m_BaseName   );
        _tcscpy( (TCHAR*)m_UserName  , (const TCHAR*)c.m_UserName   );
        _tcscpy( (TCHAR*)m_UserPass  , (const TCHAR*)c.m_UserPass   );
        _tcscpy( (TCHAR*)m_SysAdmin  , (const TCHAR*)c.m_SysAdmin   );
        _tcscpy( (TCHAR*)m_UnixSocket, (const TCHAR*)c.m_UnixSocket );
        
        m_Port         = c.m_Port        ;
        m_UserCode     = c.m_UserCode    ;
        m_IsOpen       = c.m_IsOpen      ;
        m_IsAutoCommit = c.m_IsAutoCommit;
        
        iHenv = c.iHenv;
        iHdbc = c.iHdbc;
        
        return *this;
    }
    
    void init()
    {
        m_nDB = PDatabases::ID_PDATABASES_NONE;
        
        memset( m_ServerName, 0, sizeof( m_ServerName ) );
        memset( m_BaseName  , 0, sizeof( m_BaseName   ) );
        memset( m_UserName  , 0, sizeof( m_UserName   ) );
        memset( m_UserPass  , 0, sizeof( m_UserPass   ) );
        memset( m_SysAdmin  , 0, sizeof( m_SysAdmin   ) );
        memset( m_UnixSocket, 0, sizeof( m_UnixSocket ) );
        
        m_Port         = 0;
        m_UserCode     = 0;
        m_IsOpen       = false;
        m_IsAutoCommit = false;
        
        iHenv = NULL;
        iHdbc = NULL;
    }
};

class CSqlConnectInfo : public TSqlConnectInfo
{
public:

            CSqlConnectInfo();
            CSqlConnectInfo( CSqlConnectInfo &c );
   virtual ~CSqlConnectInfo();


   CSqlConnectInfo& operator = ( CSqlConnectInfo &c )
   {
       *((TSqlConnectInfo*)this) = c;
       return *this;
   }

   bool Open();
   void Close();
   
   PDatabases getDatabase() { return m_nDB; }

   bool GetErrorInfo( SQLHSTMT hstmt );
   
   pthread_t  m_nThread;
   TSqlErrorInfo *m_Error;
};

class CSqlConnectInfoArray : public CVectorOfPtr<CSqlConnectInfo>
{
public :

   CSqlConnectInfoArray()
   {
     m_Error = new TSqlErrorInfo;
   }

   virtual ~CSqlConnectInfoArray()
   {
     delete m_Error;
   }

   TSqlErrorInfo *m_Error;
};

#endif
