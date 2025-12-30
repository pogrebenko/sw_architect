/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#ifndef SQLPIPE_H
#define SQLPIPE_H

#include "common/export.h"
#include "psql/sptr.h"
#include "psql/sqltype.h"
#include "common/VectorOfPtr.h"

// .\MSDev98\Bin\autoexp.dat
//
// The file autoexp.dat controls how the debugger displays data types 
// in windows, Quickwatch, and DataTips.
// You can modify this file to add meaningful displays for your own datatypes.
//
// CSQLPipe=Query: <m_PipeString._Ptr,s> Handle: <m_Connect,i>

class CSQLManager;

class CSQLPipe
{

friend class CSQLManager;

public:
// ------- user constructors -------
           IMPORT_C CSQLPipe( PHDBPROVIDER hHandle = NULL, bool bAutoCommit = true );
           IMPORT_C CSQLPipe( const CSQLPipe& );
  IMPORT_C virtual ~CSQLPipe(); 
// SQL User methods
  IMPORT_C bool Execute(); // compile and execute
  IMPORT_C bool Fetch();
  IMPORT_C bool Compile(); // only compile
  IMPORT_C void Commit();
  IMPORT_C void Rollback();
  IMPORT_C void Flush();   // clear inner buffers
  IMPORT_C void ShowAlert( bool bAlert );
  IMPORT_C long last_insert_rowid();
  IMPORT_C PDatabases getDatabase(); // database

// ------- define operators -------
  IMPORT_C CSQLPipe& operator  = ( const CSQLPipe& );
  IMPORT_C CSQLPipe& operator += ( const CSQLPipe& );

// ------- define friends -------
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe, const CSQLPipe );

  IMPORT_C friend CSQLPipe operator + ( const PTCHAR*, const CSQLPipe );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe, const PTCHAR* );

  IMPORT_C friend CSQLPipe operator + ( const long    , const CSQLPipe );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe, const long     );
  IMPORT_C friend CSQLPipe operator + ( const int     , const CSQLPipe );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe, const int      );
  IMPORT_C friend CSQLPipe operator + ( const double  , const CSQLPipe );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe, const double   );
  IMPORT_C friend CSQLPipe operator + ( const float   , const CSQLPipe );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe, const float    );

  IMPORT_C friend CSQLPipe operator + ( const unsigned long , const CSQLPipe       );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe      , const unsigned long  );
  IMPORT_C friend CSQLPipe operator + ( const unsigned int  , const CSQLPipe       );
  IMPORT_C friend CSQLPipe operator + ( const CSQLPipe      , const unsigned int   );
  
  CSQLPipe( STD_STRING *s );
  CSQLPipe( const CSQLPipe&, const CSQLPipe& );

  IMPORT_C CSQLPipe( const PTCHAR * str );
  CSQLPipe( const long   val );
  CSQLPipe( const int    val );
  CSQLPipe( const double val );
  CSQLPipe( const float  val );
  CSQLPipe( const unsigned long   val );
  CSQLPipe( const unsigned int    val );

  STD_STRING *m_PipeString;

protected:

  CVectorOfPtr< CSmPt >  *m_Pipes;

  CSQLPipe( const PTCHAR * str, CSmPt &pt, int size = 0, bool is_buff = true, int type = -1 );

  const CSQLPipe& Buffs( const PTCHAR * col, CSmPt &pt );
  const CSQLPipe& Binds( const PTCHAR * col, CSmPt &pt );
  const CSQLPipe& Binds( const PTCHAR * col, const PTCHAR * ops, CSmPt &pt );

  CSQLManager  *m_Manager;
  PHDBPROVIDER  m_Connect;
  bool          m_bAutoCommit;

private:

  CSQLPipe( void ** ); 
  CSQLPipe( const long   *val );
  CSQLPipe( const int    *val );
  CSQLPipe( const double *val );
  CSQLPipe( const float  *val );
  CSQLPipe( const unsigned long   *val );
  CSQLPipe( const PSQL_DATETIME   *val );
//  CSQLPipe( const unsigned int    *val );
};

class Buff : public CSQLPipe
{
public:
  IMPORT_C Buff( const PTCHAR * str, CSmPt pt );
  IMPORT_C Buff( const PTCHAR * str, CSmPt pt, int size );

  IMPORT_C Buff( CSmPt pt );
  IMPORT_C Buff( CSmPt pt, int size );
};

class Bind : public CSQLPipe
{
public:
  IMPORT_C Bind( const PTCHAR * str, CSmPt pt );
  IMPORT_C Bind( const PTCHAR * str, CSmPt pt, int size );
  IMPORT_C Bind( const PTCHAR * str, CSmPt pt, int size, int type );

  IMPORT_C Bind( CSmPt pt );
  IMPORT_C Bind( CSmPt pt, int size );
  IMPORT_C Bind( CSmPt pt, int size, int type );
};

#endif
