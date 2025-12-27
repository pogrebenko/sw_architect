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

#define SQL_MAX_MESSAGE_LENGTH 1024

#define DEFINE_SQLITE
#define DEFINE_MYSQL
#define DEFINE_ODBC

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
    ID_PDATABASES_NONE  = 0
};

// show ODBC info
// odbcinst -j

#ifdef DEFINE_SQLITE
#include <sqlite3.h>
#endif

#ifdef DEFINE_MYSQL
#include <mysql/mysql.h>
#endif

#ifdef DEFINE_ODBC
#include <sqlext.h>
#endif

typedef void* HDBPROVIDER;

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

#define tcsncpy(to,from,lenght,max_lenght) if(lenght>0) _tcsncpy((TCHAR*)to,(const TCHAR*)from,lenght<max_lenght?lenght:max_lenght-1)

#define tcscpy(to,from) tcsncpy(to,from,_tcslen((const TCHAR*)from),sizeof(to))

#endif
