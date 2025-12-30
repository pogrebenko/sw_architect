/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include "common/export.h"
#include "common/VectorOfPtr.h"

#include "psql/sqlpipe.h"
#include "psql/sqltype.h"
#include "psql/sqlinfo.h"

class CSQLBridge;

class CSQLBuilder
{
public:

  CSQLBuilder( PHDBPROVIDER Connect );
  
  virtual ~CSQLBuilder() {}

  virtual PSQLRETURN AccessSingleMethod( PSQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data ) = 0;
  virtual void Clear() = 0;
  virtual PSQLRETURN Before( PSQLHSTMT query ) = 0;
  virtual PSQLRETURN After( PSQLHSTMT query ) = 0;

  virtual PSQLRETURN Execute( PSQLHSTMT query, CVectorOfPtr<CSmPt> *in );
    
protected:

  CSqlConnectInfo* GetSession( PHDBPROVIDER hconnect );

  PHDBPROVIDER m_Connect;
  CSQLBridge  *m_pSQLBridge;
};

class CSQLBind : public CSQLBuilder
{
public:
    
  CSQLBind( PHDBPROVIDER Connect );
  
  virtual PSQLRETURN AccessSingleMethod( PSQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data );
  virtual void Clear();
  virtual PSQLRETURN Before( PSQLHSTMT query );
  virtual PSQLRETURN After( PSQLHSTMT query );
  
#ifdef DEFINE_POSTGRESQL
  std::vector<char*> m_Values;
  std::vector<int  > m_Lengths;
  std::vector<int  > m_Formats;
#endif
private:
#ifdef DEFINE_MYSQL
  std::vector<MYSQL_BIND> m_Binds;
#endif
};

class CSQLBuff : public CSQLBuilder
{
public:
    
  CSQLBuff( PHDBPROVIDER Connect );
  
  virtual PSQLRETURN AccessSingleMethod( PSQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data );
  virtual void Clear();
  virtual PSQLRETURN Before( PSQLHSTMT query );
  virtual PSQLRETURN After( PSQLHSTMT query );

  void ClearBuffs();
  
private:
#ifdef DEFINE_MYSQL
  std::vector<MYSQL_BIND> m_Buffs;
#endif
};

class CSQLData : public CSQLBuilder
{
public:
    
  CSQLData( PHDBPROVIDER Connect );
  
  virtual PSQLRETURN AccessSingleMethod( PSQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data );
  virtual void Clear();
  virtual PSQLRETURN Before( PSQLHSTMT query );
  virtual PSQLRETURN After( PSQLHSTMT query );
};

class CSQLManager
{
public:

           CSQLManager( PHDBPROVIDER Connect, bool bAutoCommit );
  virtual ~CSQLManager();

  bool  Compile( CSQLPipe *pipe );
  bool  Execute( CSQLPipe *pipe );

  bool  Fetch();
  void  Commit();
  void  Rollback();
  void  Flush();
  long  last_insert_rowid();
  void  Close();
  PDatabases getDatabase();

  CSqlConnectInfo* GetSession( PHDBPROVIDER hconnect );

  bool Open();
  bool Compile( CSQLPipe *pipe, CSqlConnectInfo *session );
  bool Execute( CSQLPipe *pipe, CSqlConnectInfo *session );
  bool Fetch( PSQLHSTMT query );
  void Commit( CSqlConnectInfo *session, PSQLHSTMT query );
  void Rollback( CSqlConnectInfo *session, PSQLHSTMT query );
  long last_insert_rowid( CSqlConnectInfo *session );
  void Close( CSqlConnectInfo *session, PSQLHSTMT query );

  void  ShowAlert( bool bAlert );

  CVectorOfPtr<CSmPt> *m_DataPtrBuff, *m_DataPtrBind;

  CSQLBuff   *m_Buff;
  CSQLBind   *m_Bind;
//  CSQLData   *m_Data;

  PHDBPROVIDER m_Connect;
  bool         m_isCompiled;
  bool         m_bAutoCommit;
  bool         m_bShowAlert;

  PSQLHSTMT    iHstmt;

  CSQLBridge  *m_pSQLBridge;
};

#endif 
