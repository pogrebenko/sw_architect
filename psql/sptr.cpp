/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#include "psql/sptr.h"

CDataPt::CDataPt( const void *pt, int typ, int sz, int st )
{
  DataPointer = (void*)pt;
  DataType    = typ;
  DataSize    = sz ;
  DataState   = st ;
  DataLength  =   0;
  DataConvert =  -1;
}

CDataPt::CDataPt( CDataPt &c )
{
  DataPointer = c.DataPointer;
  DataType    = c.DataType   ;
  DataSize    = c.DataSize   ;
  DataState   = c.DataState  ;
  DataLength  = c.DataLength ;
  DataConvert = c.DataConvert;
}

CDataPt::~CDataPt()
{
}

void CDataPt::CopyData( const CDataPt &c )
{
  if( DataType == datBytes )
     for( long i = 0; i < c.DataSize; i ++ ) ((unsigned char*)DataPointer)[ i ] = ((unsigned char*)c.DataPointer)[ i ];
  else
     memcpy( DataPointer, c.DataPointer, c.DataSize );
}

CSmPt::CSmPt(){}
CSmPt::CSmPt(const CSmPt  &c){ *this = c; }

EXPORT_C CSmPt::CSmPt( std::string &str ) : DataPt( str.c_str(), datChars, str.length() ) {}

EXPORT_C CSmPt::CSmPt(const short  *pt        ) : DataPt( pt, datShort , sizeof(short )) {}
EXPORT_C CSmPt::CSmPt(const long   *pt        ) : DataPt( pt, datLong  , sizeof(long  )) {}
EXPORT_C CSmPt::CSmPt(const double *pt        ) : DataPt( pt, datDouble, sizeof(double)) {}
EXPORT_C CSmPt::CSmPt(const float  *pt        ) : DataPt( pt, datFloat , sizeof(float )) {}
EXPORT_C CSmPt::CSmPt(const short  *pt, int sz) : DataPt( pt, datChars , sz*sizeof(unsigned short)) {}
EXPORT_C CSmPt::CSmPt(const TCHAR  *pt, int sz) : DataPt( pt, datChars , sz*sizeof(TCHAR)) {}

EXPORT_C CSmPt::CSmPt(const unsigned long   *pt        ) : DataPt( pt, datLong  , sizeof(long  )) {}
EXPORT_C CSmPt::CSmPt(const unsigned short  *pt, int sz) : DataPt( pt, datChars , sz*sizeof(unsigned short)) {}
EXPORT_C CSmPt::CSmPt(      unsigned char   *pt, int sz) : DataPt( pt, datBytes , sz) {}

EXPORT_C CSmPt::CSmPt(const PSQL_DATE      *pt ) : DataPt( pt, datDate     , sizeof(PSQL_DATE     )/*-sizeof(short)*/) {}
EXPORT_C CSmPt::CSmPt(const PSQL_TIME      *pt ) : DataPt( pt, datTime     , sizeof(PSQL_TIME     )/*-sizeof(short)*/) {}
EXPORT_C CSmPt::CSmPt(const PSQL_TIMESTAMP *pt ) : DataPt( pt, datTimeStamp, sizeof(PSQL_TIMESTAMP)) {}
EXPORT_C CSmPt::CSmPt(const PSQL_DATETIME  *pt ) : DataPt( pt, datDateTime , sizeof(PSQL_DATETIME )) {}

EXPORT_C CSmPt::~CSmPt() {}

CSmPt::CSmPt( int typ, void* pt, int sz, int st ) : DataPt( pt, typ, sz, st ) {}

CSmPt& 
CSmPt::operator = ( const CSmPt &c ) { DataPt = c.DataPt; return *this; }

void CSmPt::CopyData( const CSmPt &c )
{
  DataPt.CopyData( c.DataPt );
}

void* CSmPt::DataPointer() const { return DataPt.DataPointer; }
int   CSmPt::DataType   () const { return DataPt.DataType   ; }
int   CSmPt::DataSize   () const { return DataPt.DataSize   ; }
int   CSmPt::DataState  () const { return DataPt.DataState  ; }
int   CSmPt::DataConvert() const { return DataPt.DataConvert; }
