/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/
#include "psql/sqlmanager.h"
#include "psql/sqlpipe.h"

#define PIPE_ALLOC 16

TCHAR*
Value2Str( TCHAR *buff, int size, const long val )
{
  memset( buff, 0, size );
  _tprintf( buff, "%li", val );
  return buff;
}

TCHAR*
Value2Str( TCHAR *buff, int size, const double val )
{
  memset( buff, 0, size );
  _tprintf( buff, "%18.5f", val );
  return buff;
}

TCHAR*
Value2Str( TCHAR *buff, int size, const PSQL_DATE &val )
{
  memset( buff, 0, size );
  _tprintf( buff, "%02i.%02i.%i", val.day, val.month, val.year );
  return buff;
}

TCHAR*
Value2Str( TCHAR *buff, int size, const PSQL_TIME &val )
{
  memset( buff, 0, size );
  _tprintf( buff, "%02i:%02i:%02i", val.hour, val.minute, val.second );
  return buff;
}

TCHAR*
Value2Str( TCHAR *buff, int size, const PSQL_TIMESTAMP &val )
{
  memset( buff, 0, size );
  _tprintf( buff, "%02i.%02i.%i %02i:%02i:%02i %li", val.day, val.month, val.year, val.hour, val.minute, val.second, val.second_part );
  return buff;
}

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const CSQLPipe mutex2 )
{ return CSQLPipe( mutex1, mutex2 ); }

// signed
EXPORT_C CSQLPipe operator + ( const TCHAR *mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING;
              pstr->operator = ( mutex1 );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const TCHAR *mutex2 )
{ STD_STRING *pstr = new STD_STRING;
              pstr->operator = ( mutex2 );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }

EXPORT_C CSQLPipe operator + ( const long mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), mutex1 ) );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const long mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), mutex2 ) );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }

EXPORT_C CSQLPipe operator + ( const int mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), long( mutex1 ) ) );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const int mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), long( mutex2 ) ) );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }

EXPORT_C CSQLPipe operator + ( const double mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 36 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), mutex1 ) );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const double mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 36 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), mutex2 ) );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }

EXPORT_C CSQLPipe operator + ( const float mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 36 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), double( mutex1 ) ) );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const float mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 36 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), double( mutex2 ) ) );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }

// unsigned

EXPORT_C CSQLPipe operator + ( const unsigned long mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), long( mutex1 ) ) );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const unsigned long mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), long( mutex2 ) ) );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }

EXPORT_C CSQLPipe operator + ( const unsigned int mutex1, const CSQLPipe mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), long( mutex1 ) ) );
  return CSQLPipe( CSQLPipe( pstr ), mutex2 ); }

EXPORT_C CSQLPipe operator + ( const CSQLPipe mutex1, const unsigned int mutex2 )
{ STD_STRING *pstr = new STD_STRING; TCHAR buff[ 18 ];
              pstr->operator = ( ::Value2Str( buff, sizeof( buff ), long( mutex2 ) ) );
  return CSQLPipe( mutex1, CSQLPipe( pstr ) ); }
  
  
EXPORT_C void
CSQLPipe::ShowAlert( bool bAlert )
{
  if( m_Manager )
     m_Manager->ShowAlert( bAlert );
}

EXPORT_C CSQLPipe::CSQLPipe( PHDBPROVIDER hHandle /* = NULL */, bool bAutoCommit /* = true */ )
{
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( PIPE_ALLOC, 0 );

  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = new CSQLManager( hHandle, bAutoCommit );
  m_Connect = hHandle;
  m_bAutoCommit = bAutoCommit;
}

EXPORT_C CSQLPipe::CSQLPipe( const CSQLPipe &mutex )
{
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( mutex.m_PipeString->size(), 0 );

  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = new CSQLManager( mutex.m_Connect, mutex.m_bAutoCommit );
  m_Connect = mutex.m_Connect;

  *m_PipeString = *mutex.m_PipeString;

  for( unsigned long i = 0; i < mutex.m_Pipes->size(); i ++ ) 
      m_Pipes->push_back( new  CSmPt( *mutex.m_Pipes->at( i ) ) );
}

CSQLPipe::CSQLPipe( STD_STRING *str )
{
  m_PipeString = str;

  m_Pipes = new CVectorOfPtr< CSmPt >(false);

  m_Manager = NULL;
  m_Connect = NULL;
}

CSQLPipe::CSQLPipe( const TCHAR * str, CSmPt &pt, int size, bool is_buff, int type )
{
  m_Pipes = new CVectorOfPtr< CSmPt >(false);

  m_Manager = NULL;
  m_Connect = NULL;

  int len = _tcslen( str );
  if( !is_buff )
  {
     if( len > 0 ) len += 2;
     else          len  = 1;
  }
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( len, 0 );

  if( size > 0 ) 
  {
     if( pt.DataPt.DataType == datChars )
     {
#ifdef UNICODE
        pt.DataPt.DataSize = size*sizeof(unsigned short);
#else
         pt.DataPt.DataSize = size;
#endif
     }
     else
        pt.DataPt.DataSize = size;
  }

  pt.DataPt.DataConvert = type;

  if( is_buff ) Buffs( str, pt );
  else          Binds( str, pt );
}

CSQLPipe::CSQLPipe( const CSQLPipe& mutex1, const CSQLPipe& mutex2 )
{
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( mutex1.m_PipeString->size() + mutex2.m_PipeString->size(), 0 );

  m_Pipes = new CVectorOfPtr< CSmPt >(false);

  m_Manager = NULL;
  m_Connect = NULL;

  m_PipeString->operator  = ( mutex1.m_PipeString->c_str() );
  m_PipeString->operator += ( mutex2.m_PipeString->c_str() );

  for( unsigned long i = 0; i < mutex1.m_Pipes->size(); i ++ ) 
      m_Pipes->push_back( new  CSmPt( *mutex1.m_Pipes->at( i ) ) );
  for( unsigned long j = 0; j < mutex2.m_Pipes->size(); j ++ ) 
      m_Pipes->push_back( new  CSmPt( *mutex2.m_Pipes->at( j ) ) );
}

EXPORT_C CSQLPipe::CSQLPipe( const TCHAR *str )
{
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( _tcslen( str ), 0 );

  m_Pipes = new CVectorOfPtr< CSmPt >(false);

  m_Manager = NULL;
  m_Connect = NULL;

  m_PipeString->operator = ( str );
}

CSQLPipe::CSQLPipe( const long   val )
{ 
  TCHAR buff[ 18 ];
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( 18, 0 );
  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = NULL;
  m_Connect = NULL;
  m_PipeString->operator = ( ::Value2Str( buff, sizeof(buff), val ) );
}
CSQLPipe::CSQLPipe( const int    val )
{ 
  TCHAR buff[ 18 ];
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( 18, 0 );
  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = NULL;
  m_Connect = NULL;
  m_PipeString->operator = ( ::Value2Str( buff, sizeof(buff), (long)val ) );
}
CSQLPipe::CSQLPipe( const double val )
{ 
  TCHAR buff[ 36 ];
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( 36, 0 );
  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = NULL;
  m_Connect = NULL;
  m_PipeString->operator = ( ::Value2Str( buff, sizeof(buff), val ) );
}
CSQLPipe::CSQLPipe( const float  val )
{ 
  TCHAR buff[ 36 ];
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( 36, 0 );
  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = NULL;
  m_Connect = NULL;
  m_PipeString->operator = ( ::Value2Str( buff, sizeof(buff), (double)val ) );
}
CSQLPipe::CSQLPipe( const unsigned long   val )
{ 
  TCHAR buff[ 18 ];
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( 18, 0 );
  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = NULL;
  m_Connect = NULL;
  m_PipeString->operator = ( ::Value2Str( buff, sizeof(buff), (long)val ) );
}
CSQLPipe::CSQLPipe( const unsigned int    val )
{ 
  TCHAR buff[ 18 ];
  m_PipeString = new STD_STRING;
//  m_PipeString->resize( 18, 0 );
  m_Pipes = new CVectorOfPtr< CSmPt >(false);
  m_Manager = NULL;
  m_Connect = NULL;
  m_PipeString->operator = ( ::Value2Str( buff, sizeof(buff), (long)val ) );
}

EXPORT_C CSQLPipe::~CSQLPipe()
{
  delete  m_Manager;
  delete  m_Pipes;
  delete  m_PipeString;
}

EXPORT_C void 
CSQLPipe::Commit()
{
  if( m_Manager )
     m_Manager->Commit();
}

EXPORT_C void 
CSQLPipe::Rollback()
{
  if( m_Manager )
     m_Manager->Rollback();
}

EXPORT_C bool
CSQLPipe::Compile()
{
  if( m_Manager )
     return m_Manager->Compile( this );
  return false;
}

EXPORT_C bool
CSQLPipe::Execute()
{
  if( m_Manager )
     return m_Manager->Execute( this );

  return false;
}

EXPORT_C bool
CSQLPipe::Fetch()
{
  if( m_Manager )
     return m_Manager->Fetch();

  return false;
}

EXPORT_C void
CSQLPipe::Flush()
{
  if( m_Manager )
     m_Manager->Flush();

//  m_PipeString->resize( m_PipeString->size(), 0 );
  *m_PipeString = _T("");
}

EXPORT_C long
CSQLPipe::last_insert_rowid()
{
  if( m_Manager )
     return m_Manager->last_insert_rowid();

  return -1;
}

EXPORT_C PDatabases 
CSQLPipe::getDatabase()
{
  if( m_Manager )
     return m_Manager->getDatabase();
  return ID_PDATABASES_NONE;
}

const CSQLPipe& 
CSQLPipe::Buffs( const TCHAR * str, CSmPt &pt )
{
  if( *str )
     m_PipeString->operator += ( str );

  pt.DataPt.DataState = sqlBuff;

  m_Pipes->push_back( new  CSmPt( pt ) );

  return *this;
}

const CSQLPipe& 
CSQLPipe::Binds( const TCHAR * col, CSmPt &pt )
{
  if( col && *col )
//     return Binds( col, _T("=?"), pt );
     return Binds( col, _T("="), pt );
  else
     return Binds( _T(""), _T("?"), pt );
//     return Binds( _T(""), _T(""), pt );
}

const CSQLPipe& 
CSQLPipe::Binds( const TCHAR * col, const TCHAR * ops, CSmPt &pt )
{
  if( *col ) 
  {
     m_PipeString->operator += ( col );
     m_PipeString->operator += ( ops );
  }
//  else
//     m_PipeString->Des().Copy( ops, _tcslen( ops ) );

  m_PipeString->operator += ( _T("?") );

  pt.DataPt.DataState = sqlBind;

  m_Pipes->push_back( new  CSmPt( pt ) );

  return *this;
}




EXPORT_C CSQLPipe& 
CSQLPipe::operator = ( const CSQLPipe &mutex )
{
   Flush();

   m_PipeString->operator = ( mutex.m_PipeString->c_str() );

   m_Pipes->Flush(); 
   for( unsigned long i = 0; i < mutex.m_Pipes->size(); i ++ )
   {
	   CSmPt *pItem = mutex.m_Pipes->at( i );
       m_Pipes->push_back( new CSmPt( *pItem ) );
   }
   return *this;
}

EXPORT_C CSQLPipe& 
CSQLPipe::operator += ( const CSQLPipe &mutex )
{
   m_PipeString->operator += ( mutex.m_PipeString->c_str() );

   for( unsigned long i = 0; i < mutex.m_Pipes->size(); i ++ )
   {
	   CSmPt *pItem = mutex.m_Pipes->at( i );
       m_Pipes->push_back( new CSmPt( *pItem ) );
   }
   return *this;
}











// private constructors

CSQLPipe::CSQLPipe( void ** )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}

CSQLPipe::CSQLPipe( const long * )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}

CSQLPipe::CSQLPipe( const int * )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}

CSQLPipe::CSQLPipe( const double * )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}

CSQLPipe::CSQLPipe( const float * )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}

CSQLPipe::CSQLPipe( const unsigned long * )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}

/*
CSQLPipe::CSQLPipe( const unsigned int * )
{
  m_PipeString = NULL;
  m_Pipes      = NULL;
  m_Manager    = NULL;
  m_Connect    = NULL;
}
*/









EXPORT_C Buff::Buff( const TCHAR * str, CSmPt pt )
: CSQLPipe( str, pt, 0, true )
{
}

EXPORT_C Buff::Buff( const TCHAR * str, CSmPt pt, int size )
: CSQLPipe( str, pt, size, true )
{
}

EXPORT_C Buff::Buff( CSmPt pt )
: CSQLPipe( _T(""), pt, 0, true )
{
}

EXPORT_C Buff::Buff( CSmPt pt, int size )
: CSQLPipe( _T(""), pt, size, true )
{
}

EXPORT_C Bind::Bind( const TCHAR * str, CSmPt pt )
: CSQLPipe( str, pt, 0, false )
{
}

EXPORT_C Bind::Bind( const TCHAR * str, CSmPt pt, int size )
: CSQLPipe( str, pt, size, false )
{
}

EXPORT_C Bind::Bind( const TCHAR * str, CSmPt pt, int size, int type )
: CSQLPipe( str, pt, size, false, type )
{
}

EXPORT_C Bind::Bind( CSmPt pt )
: CSQLPipe( _T(""), pt, 0, false )
{
}

EXPORT_C Bind::Bind( CSmPt pt, int size )
: CSQLPipe( _T(""), pt, size, false )
{
}

EXPORT_C Bind::Bind( CSmPt pt, int size, int type )
: CSQLPipe( _T(""), pt, size, false, type )
{
}
