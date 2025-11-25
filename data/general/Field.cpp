/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Field.h"

#include <QRectF>
#include <QPainterPath>
#include <QtMath>

#include "common/Utils.h"

__Field&
__Field::operator = ( const __Field &o )
{
    m_LogicalName        = o.m_LogicalName       ;
    m_PhysicalName       = o.m_PhysicalName      ;
    m_nFieldRelationType = o.m_nFieldRelationType;
    m_nFieldDataType     = o.m_nFieldDataType    ;
    m_nPrecission        = o.m_nPrecission       ;
    m_nScale             = o.m_nScale            ;
    m_bAllowsNulls       = o.m_bAllowsNulls      ;
    m_bAutoIncrement     = o.m_bAutoIncrement    ;
    m_DefaultValue       = o.m_DefaultValue      ;
    m_Remarks            = o.m_Remarks           ;
    m_bHover             = o.m_bHover            ;
    m_nFirstPos          = o.m_nFirstPos         ;
    m_nLastPos           = o.m_nLastPos          ;
    m_nAngle             = o.m_nAngle            ;
    m_bEditText          = o.m_bEditText         ;
    m_bEditOptions       = o.m_bEditOptions      ;
    m_bChanged           = o.m_bChanged          ;
    m_bFromFK            = o.m_bFromFK           ;

    return *this;
}

__Field&
__Field::operator = ( const __DBCol__ &o )
{
    m_LogicalName        = o.column_name         ;
    m_PhysicalName       = o.column_name         ;
    m_nPrecission        = o.precision           ;
    m_nScale             = o.scale               ;
    m_bAllowsNulls       = strcmp(o.is_null, "NO")==0;
    m_Remarks            = o.column_comment      ;

    m_nFieldRelationType = strcmp(o.key, "PRI")==0 ? FieldRelationType_t::FieldRelationTypePrimaryKey : FieldRelationType_t::FieldRelationTypeNone;

    if( strcmp( o.datatype, "varchar"    ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeVarchar;
    if( strcmp( o.datatype, "timestamp"  ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeTimestamp;
    if( strcmp( o.datatype, "bigint"     ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "longblob"   ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "int"        ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "tinyint"    ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "longtext"   ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "smallint"   ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "blob"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "double"     ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeDecimal;
    if( strcmp( o.datatype, "mediumtext" ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "text"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "datetime"   ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeTimestamp;
    if( strcmp( o.datatype, "decimal"    ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeDecimal;
    if( strcmp( o.datatype, "enum"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "time"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeTime;
    if( strcmp( o.datatype, "float"      ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeDecimal;
    if( strcmp( o.datatype, "date"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeDate;
    if( strcmp( o.datatype, "char"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeChar;
    if( strcmp( o.datatype, "json"       ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeVarchar;
    if( strcmp( o.datatype, "set"        ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "binary"     ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "varbinary"  ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;
    if( strcmp( o.datatype, "mediumint"  ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeInteger;
    if( strcmp( o.datatype, "mediumblob" ) == 0 ) m_nFieldDataType = FieldDataType_t::FieldDataTypeBlob;

    return *this;
}

void
__Field::init()
{
    m_LogicalName       = "";
    m_PhysicalName      = "";
    m_nFieldRelationType= FieldRelationTypeNone;
    m_nFieldDataType    = FieldDataTypeNone;
    m_nPrecission       = 0;
    m_nScale            = 0;
    m_bAllowsNulls      = false;
    m_bAutoIncrement    = false;
    m_DefaultValue      = "";
    m_Remarks           = "";

    m_bHover            = false;
    m_nFirstPos         = {};
    m_nLastPos          = {};
    m_nAngle            = 0.;
    m_bEditText         = false;
    m_bEditOptions      = false;
    m_bChanged          = false;
    m_bFromFK           = false;
}

bool __Field::contain_title( const QPoint &mousePos, const __Class *pClass )
{
    QRectF       rcf;
                 rcf.setTopLeft    ( pClass->m_nFirstPos );
                 rcf.setBottomRight( pClass->m_nLastPos  );

    QPointF      center( rcf.center() );

    QTransform   transform;
                 transform.translate( center.x(), center.y() );
                 transform.rotate( -qRadiansToDegrees( m_nAngle ) );
                 transform.translate( -center.x(), -center.y() );


                 rcf.setTopLeft    ( m_nFirstPos );
                 rcf.setBottomRight( m_nLastPos  );

    QPainterPath rectanglePath;
                 rectanglePath.addRect( rcf );

    //__Logger.info( APP_LOG_LEVEL, "info: contain_rectangle[%f]", nAngle );

    return transform.map( rectanglePath ).contains( mousePos );
//    return contain_rectangle( mousePos, m_nFirstPos, m_nLastPos, m_nAngle );
}

long
FieldList_t::hover_title( const QPoint &pos, const __Class *pClass )
{
    auto found_reverse = std::find_if( __EXECUTION_POLICY_BUILDER__, rbegin(), rend(), [pos,pClass]( auto pItem ) { return pItem->contain_title( pos, pClass ); } );
    return ( found_reverse != rend() ) ? std::distance( begin(), -- found_reverse.base() ) : -1;
}

long
FieldList_t::find_edit()
{
    auto found = std::find_if( __EXECUTION_POLICY_BUILDER__, begin(), end(), []( auto pItem ) { return pItem->m_bEditText || pItem->m_bEditOptions; } );
    return ( found != end() ) ? std::distance( begin(), found ) : -1;
}

long
FieldList_t::find_fromFK()
{
    auto found = std::find_if( __EXECUTION_POLICY_BUILDER__, begin(), end(), []( auto pItem ) { return pItem->m_bFromFK; } );
    return ( found != end() ) ? std::distance( begin(), found ) : -1;
}

void
FieldList_t::hover_clear()
{
    for_each( begin(), end(), []( auto &pItem ){ pItem->m_bHover = false; } );
}
