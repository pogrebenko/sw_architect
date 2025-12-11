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

  CSQLBuilder( HDBPROVIDER Connect );
  
  virtual ~CSQLBuilder() {}

  virtual SQLRETURN AccessSingleMethod( SQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data ) = 0;
  virtual void Clear() = 0;
  virtual SQLRETURN Before( SQLHSTMT query ) = 0;
  virtual SQLRETURN After( SQLHSTMT query ) = 0;

  virtual SQLRETURN Execute( SQLHSTMT query, CVectorOfPtr<CSmPt> *in );
    
protected:

  CSqlConnectInfo* GetSession( HDBPROVIDER hconnect );

  HDBPROVIDER m_Connect;
  CSQLBridge *m_pSQLBridge;
};

class CSQLBind : public CSQLBuilder
{
public:
    
  CSQLBind( HDBPROVIDER Connect );
  
  virtual SQLRETURN AccessSingleMethod( SQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data );
  virtual void Clear();
  virtual SQLRETURN Before( SQLHSTMT query );
  virtual SQLRETURN After( SQLHSTMT query );
  
private:  
  std::vector<MYSQL_BIND> m_Binds;
};

class CSQLBuff : public CSQLBuilder
{
public:
    
  CSQLBuff( HDBPROVIDER Connect );
  
  virtual SQLRETURN AccessSingleMethod( SQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data );
  virtual void Clear();
  virtual SQLRETURN Before( SQLHSTMT query );
  virtual SQLRETURN After( SQLHSTMT query );

  void ClearBuffs();
  
private:  
  std::vector<MYSQL_BIND> m_Buffs;
};

class CSQLData : public CSQLBuilder
{
public:
    
  CSQLData( HDBPROVIDER Connect );
  
  virtual SQLRETURN AccessSingleMethod( SQLHSTMT query, unsigned short i, short db_type, CSmPt *db_data );
  virtual void Clear();
  virtual SQLRETURN Before( SQLHSTMT query );
  virtual SQLRETURN After( SQLHSTMT query );
};

class CSQLManager
{
public:

           CSQLManager( HDBPROVIDER Connect, bool bAutoCommit );
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

  CSqlConnectInfo* GetSession( HDBPROVIDER hconnect );

  bool Open();
  bool Compile( CSQLPipe *pipe, CSqlConnectInfo *session );
  bool Execute( CSQLPipe *pipe, CSqlConnectInfo *session );
  bool Fetch( SQLHSTMT query );
  void Commit( CSqlConnectInfo *session, SQLHSTMT query );
  void Rollback( CSqlConnectInfo *session, SQLHSTMT query );
  long last_insert_rowid( CSqlConnectInfo *session );
  void Close( CSqlConnectInfo *session, SQLHSTMT query );

  void  ShowAlert( bool bAlert );

  CVectorOfPtr<CSmPt> *m_DataPtrBuff, *m_DataPtrBind;

  CSQLBuff   *m_Buff;
  CSQLBind   *m_Bind;
//  CSQLData   *m_Data;

  HDBPROVIDER m_Connect;
  bool        m_isCompiled;
  bool        m_bAutoCommit;
  bool        m_bShowAlert;

  SQLHSTMT    iHstmt;

  CSQLBridge *m_pSQLBridge;
};

#endif 
