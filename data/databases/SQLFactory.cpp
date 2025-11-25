/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "SQLFactory.h"

#include "mysql.h"

CSQLFactory::CSQLFactory( PDatabases DatabaseType )
: m_DatabaseType( DatabaseType )
{

}

CSQLDatabase*
CSQLFactory::buildDatabase()
{
    switch( m_DatabaseType )
    {
        case PDatabases::ID_PDATABASES_MYSQL   : return new CMySQL();
        case PDatabases::ID_PDATABASES_ODBC    :
        case PDatabases::ID_PDATABASES_SQLITE  :
        case PDatabases::ID_PDATABASES_NONE    : break;
    }
    return nullptr;
}
