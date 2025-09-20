/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "mysql.h"

CMySQL::CMySQL() {}

std::string
CMySQL::getType( FieldDataType_t FieldDatadtType )
{
    std::string dt;
    switch( FieldDatadtType )
    {
        case FieldDataTypeNone      :                   break;
        case FieldDataTypeInteger   : dt = "INT"      ; break;
        case FieldDataTypeDecimal   : dt = "DECIMAL"  ; break;
        case FieldDataTypeTimestamp : dt = "TIMRSTAMP"; break;
        case FieldDataTypeTime      : dt = "TIME"     ; break;
        case FieldDataTypeDate      : dt = "DATE"     ; break;
        case FieldDataTypeBlob      : dt = "BLOB"     ; break;
        case FieldDataTypeVarchar   : dt = "VARCHAR"  ; break;
        case FieldDataTypeChar      : dt = "CHAR"     ; break;
    }
    return dt;
}
