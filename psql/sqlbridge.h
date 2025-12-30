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

   void    Start( PHDBPROVIDER Handle );
   void    Stop ( PHDBPROVIDER Handle );

   bool    Connect( CSqlConnectInfo &Info, PHDBPROVIDER *pHandle );

   PDatabases GetDatabase( PHDBPROVIDER Handle );
   PTCHAR * GetBaseName( PHDBPROVIDER Handle );
   PTCHAR * GetUserName( PHDBPROVIDER Handle );
   PTCHAR * GetUserPass( PHDBPROVIDER Handle );
   PTCHAR * GetSysadm  ( PHDBPROVIDER Handle );

   int     * GetUserCode( PHDBPROVIDER Handle );

   void      ShowErrorNow( PHDBPROVIDER Handle, PSQLHSTMT hstmt );
   long      findIndex( PHDBPROVIDER pHandle );

   void      ShowSQLErrorNow( TSqlErrorInfo*, const PTCHAR* );

   CSqlConnectInfoArray *m_pSqlInfo;
};

#endif
