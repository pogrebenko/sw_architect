/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "SQLFactory.h"

#include "mysql.h"

CSQLFactory::CSQLFactory( DatabaseType_t DatabaseType )
: m_DatabaseType( DatabaseType )
{

}

CSQLDatabase*
CSQLFactory::buildDatabase()
{
    switch( m_DatabaseType )
    {
    case DatabaseTypeMySQL : return new CMySQL();
    case DatabaseTypeNone  : break;
    }
    return nullptr;
}
