/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#ifndef SQLFACTORY_H
#define SQLFACTORY_H

#include "common/Consts.h"

class CSQLDatabase
{
public:
    inline          CSQLDatabase(){}
    inline virtual ~CSQLDatabase(){}

    virtual std::string getType( FieldDataType_t FieldDataType ) = 0;
};

class CSQLFactory
{
public:
    CSQLFactory( DatabaseType_t DatabaseType );

    CSQLDatabase* buildDatabase();

    DatabaseType_t m_DatabaseType;
};

#endif // SQLFACTORY_H
