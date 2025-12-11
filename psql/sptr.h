/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#ifndef SPTR_H
#define SPTR_H

#include "common/export.h"
#include "psql/sqltype.h"

enum 
{
  datDummy = 0,
  datLong,
  datDouble,
  datChars,
  datBytes,
  datFloat,
  datShort,
  datDate,
  datTime,
  datTimeStamp,
  datDateTime
};

enum 
{
  sqlDummy = 0,
  sqlBind,
  sqlBuff
};

class CSmPt;
class CSQLBind;
class CSQLBuff;
class CSQLData;
class CSQLPipe;

class CDataPt
{

friend class CSmPt;
friend class CSQLBind;
friend class CSQLBuff;
friend class CSQLData;
friend class CSQLPipe;

public:

  CDataPt( const void *pt=0, int typ=datDummy, int sz=0, int st = sqlDummy );
  CDataPt( CDataPt &c );

  virtual ~CDataPt();

  void CopyData( const CDataPt &c );

protected:

  void *DataPointer;
  int   DataType   ;
  int   DataSize   ;
  int   DataState  ;
  int   DataConvert;
  long  DataLength ;
};

class CSmPt
{

friend class CSQLBind;
friend class CSQLBuff;
friend class CSQLData;
friend class CSQLPipe;

public:

		         CSmPt();
		         CSmPt(const CSmPt &c);

        IMPORT_C CSmPt( std::string &str );

// signed section
		IMPORT_C CSmPt(const short  *pt);
		IMPORT_C CSmPt(const long   *pt);
		IMPORT_C CSmPt(const double *pt);
		IMPORT_C CSmPt(const float  *pt);
		IMPORT_C CSmPt(const short *pt, int sz = 0 );
		IMPORT_C CSmPt(const TCHAR *pt, int sz = 0 );
// unsigned section
		IMPORT_C CSmPt(const unsigned long  *pt);
		IMPORT_C CSmPt(const unsigned short *pt, int sz = 0 );
		IMPORT_C CSmPt(      unsigned char  *pt, int sz = 0 );

		IMPORT_C CSmPt(const PSQL_DATE *pt);
		IMPORT_C CSmPt(const PSQL_TIME *pt);
		IMPORT_C CSmPt(const PSQL_TIMESTAMP *pt);
		IMPORT_C CSmPt(const PSQL_DATETIME  *pt);
        
		         CSmPt( int typ, void* pt, int sz, int st );

		IMPORT_C virtual ~CSmPt();

  CSmPt& operator = ( const CSmPt &c );

  void CopyData( const CSmPt &c );

  void* DataPointer() const;
  int   DataType   () const;
  int   DataSize   () const;
  int   DataState  () const;
  int   DataConvert() const;

protected:

  CDataPt DataPt;
};

#endif
