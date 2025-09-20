/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#ifndef SQLSTREAMER_H
#define SQLSTREAMER_H

#include <cstring>

#include "data/Options.h"
#include "data/databases/SQLFactory.h"

class CSQLStreamer : public std::unique_ptr<CSQLDatabase>
{
public:
    inline          CSQLStreamer( CSQLDatabase *pSQLDatabase, CAppOptions *pAppOptions )
                  : std::unique_ptr<CSQLDatabase>( pSQLDatabase )
                  , m_pAppOptions( pAppOptions ){}
    inline virtual ~CSQLStreamer(){}

    void Write( void *add_ptr, long add_size );
    void Write( std::string &buffer );

    std::string              m_Buffer;
    std::vector<std::string> m_PKs;
    unsigned long            m_nSize;
    unsigned long            m_nI;
    CAppOptions             *m_pAppOptions;
};

void Write( CSQLStreamer &stream, CAppOptions &obj );

#endif // SQLSTREAMER_H
