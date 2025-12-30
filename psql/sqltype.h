/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.0                              */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#ifndef SQLTYPE_H
#define SQLTYPE_H

typedef void* 		     PSQLHANDLE;
typedef PSQLHANDLE       PSQLHENV;
typedef PSQLHANDLE       PSQLHDBC;
typedef PSQLHANDLE       PSQLHSTMT;
typedef PSQLHANDLE       PSQLHDESC;
typedef PSQLHANDLE       PHDBPROVIDER;
typedef signed short int PSQLRETURN;
typedef char             PTCHAR;
typedef const PTCHAR     PTCCHAR;

#define PSQL_NULL_HANDLE           0L
#define PSQL_SUCCESS               0
#define PSQL_ERROR                -1
#define PSQL_NO_DATA_FOUND       100
#define PSQL_MAX_STATE_LENGTH     32
#define PSQL_MAX_MESSAGE_LENGTH 1024

//#define DEFINE_SQLITE
//#define DEFINE_MYSQL
//#define DEFINE_ODBC
//#define DEFINE_POSTGRESQL

enum PDatabases
{
#ifdef DEFINE_SQLITE
    ID_PDATABASES_SQLITE= 1, // sudo apt-get install sqlite3 libsqlite3-dev
#endif
#ifdef DEFINE_MYSQL
    ID_PDATABASES_MYSQL = 2, // sudo apt-get install libmysqlclient-dev // ??? libmysqlcppconn-dev
#endif
#ifdef DEFINE_ODBC
    ID_PDATABASES_ODBC  = 3, // sudo apt-get install unixodbc-dev, mysql-connector-odbc // ??? odbcinst unixodbc // ??? odbcinst1debian2 libodbc1
#endif
#ifdef DEFINE_POSTGRESQL
    ID_PDATABASES_POSTGRESQL  = 4, // sudo apt install libpq-dev
#endif
    ID_PDATABASES_NONE  = 0
};

// show ODBC info
// odbcinst -j

#ifdef __linux__
#ifdef DEFINE_SQLITE
#include <sqlite3.h>
#endif
#ifdef DEFINE_MYSQL
#include <mysql/mysql.h>
#endif
#ifdef DEFINE_ODBC
#include <sqlext.h>
#endif
#ifdef DEFINE_POSTGRESQL
#include <postgresql/libpq-fe.h>
#endif
#endif

typedef struct tagPSQL_DATE
{
//            short	year;
//   unsigned short	month;
//   unsigned short	day;
//   unsigned short	empty;
    unsigned int year, month, day, hour, minute, second;
    unsigned long second_part;
    bool neg;
#ifdef DEFINE_MYSQL
    enum enum_mysql_timestamp_type time_type;
#endif
    int time_zone_displacement;
} PSQL_DATE;

typedef struct tagPSQL_TIME
{
//   unsigned short	hour;
//   unsigned short	minute;
//   unsigned short	second;
//   unsigned short	empty;
    unsigned int year, month, day, hour, minute, second;
    unsigned long second_part;
    bool neg;
#ifdef DEFINE_MYSQL
    enum enum_mysql_timestamp_type time_type;
#endif
    int time_zone_displacement;
} PSQL_TIME;

typedef struct tagPSQL_TIMESTAMP
{
//            short   year;
//   unsigned short   month;
//   unsigned short   day;
//   unsigned short   hour;
//   unsigned short   minute;
//   unsigned short   second;
//   unsigned int     fraction;
    unsigned int year, month, day, hour, minute, second;
    unsigned long second_part;
    bool neg;
#ifdef DEFINE_MYSQL
    enum enum_mysql_timestamp_type time_type;
#endif
    int time_zone_displacement;
} PSQL_TIMESTAMP;

typedef struct tagPSQL_DATETIME
{
//            short   year;
//   unsigned short   month;
//   unsigned short   day;
//   unsigned short   hour;
//   unsigned short   minute;
//   unsigned short   second;
//   unsigned int     fraction;
    unsigned int year, month, day, hour, minute, second;
    unsigned long second_part;
    bool neg;
#ifdef DEFINE_MYSQL
    enum enum_mysql_timestamp_type time_type;
#endif
    int time_zone_displacement;
} PSQL_DATETIME;

// typedef struct MYSQL_TIME {
// unsigned int year, month, day, hour, minute, second;
// unsigned long second_part;
// bool neg;
// enum enum_mysql_timestamp_type time_type;
// int time_zone_displacement;
// } MYSQL_TIME;

#define tcsncpy(to,from,lenght,max_lenght) if(lenght>0) _tcsncpy((PTCHAR*)to,(const PTCHAR*)from,lenght<max_lenght?lenght:max_lenght-1)
#define tcscpy(to,from) tcsncpy(to,from,_tcslen((const PTCHAR*)from),sizeof(to))

#endif
