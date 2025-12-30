/**************************************************/
/*                                            	   */
/*    Software Libraries :   <Engine> <Sql>       */
/*                                                */
/*    Version  : 2.1<ARMI>                        */
/*    Language : Visual C++ v6.00                 */
/*    Author   : v.pogrebenko@faktor3.de          */
/*                                                */
/**************************************************/

#ifndef EXPORT_H
#define EXPORT_H

// Unicode defines
//#define UNICODE
//#define SQL_WCHART_CONVERT

#include <stdio.h>
#include <string.h>
#include <string>
#include <wchar.h>

#ifndef EXPORT_C
#define EXPORT_C
//#define EXPORT_C extern "C++"
#endif

#ifndef IMPORT_C
#define IMPORT_C
//#define IMPORT_C extern "C++"   
#endif

#ifndef _T
  #if defined(UNICODE) || defined(_UNICODE)
    #define _T(s) (L ## s)
  #else
    #define _T(s) (s)
  #endif
#endif


#ifndef _MSC_VER
#ifdef UNICODE
#ifdef SQL_WCHART_CONVERT
typedef wchar_t             TCHAR;
#else
typedef signed short        TCHAR;
#endif
#else
typedef char				TCHAR;
#endif
#endif


#if defined(UNICODE) || defined(_UNICODE)
	#define STD_STRING std::wstring
    
    //note: descriptions with * in front have replacement functions
    #define _fprintf fwprintf
    //formatting functions
    #define _sntprintf swprintf //* make a formatted a string
    #define _tprintf wprintf //* print a formatted string
    
    //this one has no replacement functions yet, but it is only used in the tests
    #define _vsntprintf vsnwprintf //* print a formatted string using variable arguments
    
    #define _istalnum iswalnum //* alpha/numeric char check
    #define _istalpha iswalpha //* alpha char check
    #define _istspace iswspace //* space char check
    #define _istdigit iswdigit //* digit char check
    #define _totlower towlower //* convert char to lower case
    #define _totupper towupper //* convert char to lower case
    #define _tcslwr wcslwr //* convert string to lower case
    
    //these are the string handling functions
    //we may need to create wide-character/multi-byte replacements for these
    #define _tcscpy wcscpy //copy a string to another string
    #define _tcsncpy wcsncpy //copy a specified amount of one string to another string.
    #define _tcscat wcscat //copy a string onto the end of the other string
    #define _tcschr wcschr //find location of one character
    #define _tcsstr wcsstr //find location of a string
    #define _tcslen wcslen //get length of a string
    #define _tcscmp wcscmp //case sensitive compare two strings
    #define _tcsncmp wcsncmp //case sensitive compare two strings
    #define _tcscspn wcscspn //location of any of a set of character in a string
    
    #define _tcsicmp wcscasecmp //* case insensitive compare two string
    
    //conversion functions
    #define _tcstod wcstod //convert a string to a double
    #define _tcstoi64 wcstoll //* convers a string to an 64bit bit integer
    #define _i64tot lltow //* converts a 64 bit integer to a string (with base)
    
#else //if defined(_ASCII)
	#define STD_STRING std::string
    
    #define _fprintf fprintf
    //formatting functions
    #define _tprintf sprintf    
    #define _sntprintf snprintf    
    //#define _tprintf printf
    #define _vsntprintf vsnprintf 
    
    #define _istalnum isalnum
    #define _istalpha isalpha
    #define _istspace isspace
    #define _istdigit isdigit
    #define _totlower tolower
    #define _totupper toupper
    #define _tcslwr strlwr
    
    //these are the string handling functions
    #define _tcscpy strcpy
    #define _tcsncpy strncpy
    #define _tcscat strcat
    #define _tcschr strchr
    #define _tcsstr strstr
    #define _tcslen strlen
    #define _tcscmp strcmp
    #define _tcsncmp strncmp
    #define _tcsicmp strcasecmp
    #define _tcscspn strcspn
    
    //converstion methods
    #define _tcstod strtod
    #define _tcstoi64 strtoll
    #define _i64tot lltoa
#endif

#endif
