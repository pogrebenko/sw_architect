/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/
#ifndef SQLBRIDGE_H
#define SQLBRIDGE_H

#include "common/export.h"
#include "psql/sqltype.h"
#include "psql/sqlinfo.h"

class CSQLBridge
{
public:

            CSQLBridge();
   virtual ~CSQLBridge();

   void Initialize();
   void Destroy();

   CSqlConnectInfoArray* Connects();

   void    Start( HDBPROVIDER Handle );
   void    Stop ( HDBPROVIDER Handle );

   bool    Connect( CSqlConnectInfo &Info, HDBPROVIDER *pHandle );

   PDatabases GetDatabase( HDBPROVIDER Handle );
   TCHAR * GetBaseName( HDBPROVIDER Handle );
   TCHAR * GetUserName( HDBPROVIDER Handle );
   TCHAR * GetUserPass( HDBPROVIDER Handle );
   TCHAR * GetSysadm  ( HDBPROVIDER Handle );

   int     * GetUserCode( HDBPROVIDER Handle );

   void      ShowErrorNow( HDBPROVIDER Handle, SQLHSTMT hstmt );
   long      findIndex( HDBPROVIDER pHandle );

   void      ShowSQLErrorNow( TSqlErrorInfo*, const TCHAR* );

   CSqlConnectInfoArray *m_pSqlInfo;
};

#endif
