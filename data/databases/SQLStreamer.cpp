/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "SQLStreamer.h"

void
CSQLStreamer::Write( void *add_ptr, long add_size )
{
    char *pos = (char*)memchr( add_ptr, '\0', add_size );
    m_Buffer.append( (char*)add_ptr, pos ? pos - (char*)add_ptr : add_size );
}

void
CSQLStreamer::Write( std::string &buffer )
{
    m_Buffer.append( buffer );
}

void Write( CSQLStreamer &stream, __Field &obj )
{
    //std::string pk;
    switch( obj.m_nFieldRelationType )
    {
        case FieldRelationTypePrimaryKey : stream.m_PKs.push_back( obj.m_PhysicalName ); break;
        case FieldRelationTypeNone       :
        case FieldRelationTypeForeignKey : break;
    }

    std::string buff = std::string( obj.m_PhysicalName );
    std::string dt = stream.get()->getType( obj.m_nFieldDataType );
    if( !dt.empty() )
    {
        buff += " " + dt;
    }
    if( obj.m_nScale > 0 )
    {
        buff += "(" + std::to_string( obj.m_nScale );
        if( obj.m_nPrecission > 0 )
        {
            buff += "," + std::to_string( obj.m_nPrecission );
        }
        buff += ")";
    }
    buff += obj.m_bAutoIncrement ? " AUTO INCRENEMT" : ""         ;
    buff += obj.m_bAllowsNulls   ? ""                : " NOT NULL";
    if( !obj.m_DefaultValue.empty() )
    {
        buff += " " + obj.m_DefaultValue;
    }
    if( stream.m_nI != stream.m_nSize - 1 )
    {
        buff += ",";
    }
    buff += "\n";

    stream.Write( buff );
}

void Write( CSQLStreamer &stream, __Class &obj )
{
    std::string buff = std::string( "CREATE TABLE " ) + obj.m_PhysicalName + " (\n";
    stream.Write( buff );

    stream.m_PKs.clear();
    stream.m_nSize = obj.m_FieldList.size();
    for( stream.m_nI = 0; stream.m_nI < obj.m_FieldList.size(); ++ stream.m_nI )
    {
        Write( stream, *obj.m_FieldList.at( stream.m_nI ).get() );
    }

    if( stream.m_PKs.size() > 0 )
    {
        buff = std::string("PRIMARY KEY (");
        for( unsigned long n = 0; n < stream.m_PKs.size(); ++ n )
        {
            buff += stream.m_PKs.at( n );
        }
        buff += ")\n";
        stream.Write( buff );
    }

    buff = ");\n\n";
    stream.Write( buff );
}

void Write( CSQLStreamer &stream, __Relation &obj )
{
    auto CL = stream.m_pAppOptions->getClassList();
    auto pClassFrom = CL->at( obj.m_nFrom ).get();
    auto pClassTo   = CL->at( obj.m_nTo   ).get();

    std::string pk; FieldList_t PK; pClassFrom->enumFields( PK, FieldRelationTypePrimaryKey );
    std::for_each( PK.begin(), PK.end(), [&pk,PK]( auto &pField ) { pk += pField->m_PhysicalName; if(pField!=PK.back()) pk += ","; } );

    std::string fk; FieldList_t FK; pClassTo->enumFields( FK, FieldRelationTypeForeignKey );
    std::for_each( FK.begin(), FK.end(), [&fk,FK,pk]( auto &pField ) { if( pk.find_first_of( pField->m_PhysicalName ) >= 0 ) fk += pField->m_PhysicalName; if(pField!=FK.back()) fk += ","; } );

    std::string buff = std::string( "ALTER TABLE " ) + pClassTo->m_PhysicalName
                     + " ADD CONSTRAINT " + obj.m_Name + "\n";

    buff += std::string( "FOREIGN KEY (" ) + fk + ")\n";

    buff += std::string( "REFERENCES " ) + pClassFrom->m_PhysicalName + std::string( " (" ) + pk + ")\n";

    buff += std::string( "ON DELETE" );
    switch( obj.m_nDeleteRule )
    {
        case RelationPropertyRule_t::Cascade    : buff += " CASCADE"    ; break;
        case RelationPropertyRule_t::Restrict   : buff += " RESTRICT"   ; break;
        case RelationPropertyRule_t::NoAction   : buff += " NO ACTION"  ; break;
        case RelationPropertyRule_t::SetNull    : buff += " SET NULL"   ; break;
        case RelationPropertyRule_t::SetDefault : buff += " SET DEFAULT"; break;
    }
    buff += "\n";

    buff += std::string( "ON UPDATE" );
    switch( obj.m_nDeleteRule )
    {
        case RelationPropertyRule_t::Cascade    : buff += " CASCADE"    ; break;
        case RelationPropertyRule_t::Restrict   : buff += " RESTRICT"   ; break;
        case RelationPropertyRule_t::NoAction   : buff += " NO ACTION"  ; break;
        case RelationPropertyRule_t::SetNull    : buff += " SET NULL"   ; break;
        case RelationPropertyRule_t::SetDefault : buff += " SET DEFAULT"; break;
    }
    buff += "\n";

    switch( obj.m_nDeferrability )
    {
        case RelationPropertyDeferrability_t::NotDeferrability             : break;
        case RelationPropertyDeferrability_t::DeferrableInitiallyDeferred  : buff += " DEFERRABLE INITIALLY DEFERRED\n" ; break;
        case RelationPropertyDeferrability_t::DeferrableInitiallyImmediate : buff += " DEFERRABLE INITIALLY IMMEDIATE\n"; break;
    }

    buff += "\n";

    stream.Write( buff );
}

void Write( CSQLStreamer &stream, CAppOptions &obj )
{
    for( unsigned long n = 0; n < obj.getClassList()->size(); ++ n )
    {
        Write( stream, *obj.getClassList()->at( n ).get() );
    }
    for( unsigned long n = 0; n < obj.getRelationList()->size(); ++ n )
    {
        Write( stream, *obj.getRelationList()->at( n ).get() );
    }
}

