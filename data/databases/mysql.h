/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#ifndef MYSQL_H
#define MYSQL_H

#include "data/databases/SQLFactory.h"

class CMySQL : public CSQLDatabase
{
public:
    CMySQL();

    std::string getType( FieldDataType_t FieldDatadtType ) override;
};

#endif // MYSQL_H
