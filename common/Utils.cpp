/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Utils.h"

#include <QtMath>
#include <QPoint>
#include <QVariantMap>
#include <QJsonArray>
#include <QRectF>
#include <QPainterPath>
#include <QJsonObject>
#include <QColor>

#include <regex>

#include "common/IOStreamer.h"
#include "common/Logger.h"

#include "data/general/Class.h"
#include "data/general/Relation.h"
#include "data/Options.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
#define                             __AppOptions (*__AppOptions__.get())

QPoint
pointf2point( const QPointF &pt )
{
    return QPoint( pt.x(), pt.y() );
}

QPointF
point2pointf( const QPoint  &pt )
{
    return QPointF( pt.x(), pt.y() );
}

QPointF
correct_point( const QPointF &pt, const QRectF &item )
{
    float x = pt.x(), y = pt.y();
    if( pt.x() < item.left() )
    {
        x = item.left();
    }
    else
    if( pt.x() > item.right() )
    {
        x = item.right();
    }
    if( pt.y() < item.top() )
    {
        y = item.top();
    }
    else
    if( pt.y() > item.bottom() )
    {
        y = item.bottom();
    }
    return QPointF( x, y );
}


void
rgb2hex( QString &sHex, int r, int g, int b )
{
    std::string sBuff;
    rgb2hex( sBuff, r, g, b );
    sHex.fromStdString( sBuff );
}

void
rgb2hex( std::string &sHex, int r, int g, int b )
{
    char buff[ 8 ]; memset( buff, 0, sizeof( buff ) );
    snprintf( buff, sizeof( buff ), "#%2x%2x%2x", r, g, b );
    sHex = buff;
}

QColor
hex2color( const QString &sHex )
{
    int r = 0, g = 0, b = 0;
    if( hex2rgb( sHex, r, g, b ) )
    {
        return QColor( qRgb( r, g, b ) );
    }
    return QColor( Qt::color0 );
}

bool
hex2rgb( const QString &sHex, int &r, int &g, int &b )
{
    std::string sBuff = sHex.toStdString();
    return hex2rgb( sBuff, r, g, b );
}

bool
hex2rgb( const std::string &sHex, int &r, int &g, int &b )
{
    std::regex  pattern( "#([0-9a-fA-F]{6})" );
    std::smatch match;
    if( std::regex_match( sHex, match, pattern ) )
    {
        int rc = sscanf( match.str( 1 ).c_str(), "%2x%2x%2x", &r, &g, &b );
        return rc == 3;
    }
    return false;
}

void
draw_relation_intersection( std::vector <__PointType> &intersection, const __Class *pClass, const QLineF &line )
{
    QRectF       rc;
                 rc.setTopLeft    ( pClass->m_nFirstPos );
                 rc.setBottomRight( pClass->m_nLastPos  );
    QPointF      center = rc.center();
    QPainterPath rectPath;
                 rectPath.addRect( rc );
    QTransform   transform;
                 transform.translate(  center.x(),  center.y() );
                 transform.rotate( -qRadiansToDegrees( pClass->m_nAngle ) );
                 transform.translate( -center.x(), -center.y() );

    QPainterPath painterPath = transform.map( rectPath );
    QPointF      currentPoint;
    for( int i = 0; i < painterPath.elementCount(); ++ i )
    {
        const QPainterPath::Element& element = painterPath.elementAt(i);
        if( element.type == QPainterPath::MoveToElement )
        {
            currentPoint = element;
        }
        else
        if( element.type == QPainterPath::LineToElement )
        {
            __PointType pt;
            pt.m_nIntersectionType = line.intersects( QLineF( currentPoint, element ), &pt.m_nPoint );
            intersection.push_back( pt );
            currentPoint = element;
        }
        else
        if( element.type == QPainterPath::CurveToElement ||
            element.type == QPainterPath::CurveToDataElement )
        {
            __PointType pt;
            pt.m_nIntersectionType = line.intersects( QLineF( currentPoint, element ), &pt.m_nPoint );
            intersection.push_back( pt );
            currentPoint = element;
        }
    }
}

bool contain_rectangle( const QPoint &mousePos, const QPoint &nFirstPos, const QPoint &nLastPos, float nAngle )
{
    QRectF       rcf;
                 rcf.setTopLeft    ( nFirstPos );
                 rcf.setBottomRight( nLastPos  );

    QPointF      center( rcf.center() );

    QPainterPath rectanglePath;
                 rectanglePath.addRect( rcf );

    QTransform   transform;
                 transform.translate( center.x(), center.y() );
                 transform.rotate( -qRadiansToDegrees( nAngle ) );
                 transform.translate( -center.x(), -center.y() );

    //__Logger.info( APP_LOG_LEVEL, "info: contain_rectangle[%f]", nAngle );

    return transform.map( rectanglePath ).contains( mousePos );
}


QJsonObject& Write( QJsonObject &ios, QPoint &obj )
{
    ios[ "x" ] = obj.rx();
    ios[ "y" ] = obj.ry();
    return ios;
}

QPoint& Read( QJsonObject &ios, QPoint &obj, long )
{
    obj.setX( ios[ "x" ].toInt() );
    obj.setY( ios[ "y" ].toInt() );
    return obj;
}


void Write( CIOStreamer &ios, QPoint &obj )
{
    int x = obj.rx(); ::Write( ios, x );
    int y = obj.ry(); ::Write( ios, y );
}

QPoint& Read( CIOStreamer &ios, QPoint &obj, long )
{
    int x = 0; ::Read( ios, x ); obj.setX( x );
    int y = 0; ::Read( ios, y ); obj.setY( y );
    return obj;
}

void Write( CIOStreamer &ios, Class_t &obj )
{
    ::WriteInt( ios, obj.m_nFigureType );
    ::Write   ( ios, obj.m_nFirstPos   );
    ::Write   ( ios, obj.m_nLastPos    );
    ::Write   ( ios, obj.m_nTitlePos   );
    ::Write   ( ios, obj.m_nAnglePos   );
    ::Write   ( ios, obj.m_nResizePos  );
    ::Write   ( ios, obj.m_nAngle      );
    ::Write   ( ios, obj.m_nAnglesCount);
    //::Write   ( ios, obj.m_bHover      );
    //::Write   ( ios, obj.m_bSelect     );
    //::Write   ( ios, obj.m_bHoverCenter);
    //::Write   ( ios, obj.m_bEditText   );
    //::Write   ( ios, obj.m_bEditOptions);
    ::Write   ( ios, obj.m_LogicalName );
    ::Write   ( ios, obj.m_PhysicalName);
    ::Write   ( ios, obj.m_FieldList   );
}

void Write( CIOStreamer &ios, Field_t &obj )
{
    ::Write( ios, obj.m_nFirstPos          );
    ::Write( ios, obj.m_nLastPos           );
    ::Write( ios, obj.m_nAngle             );
    //::Write( ios, obj.m_bHover             );
    //::Write( ios, obj.m_bEditText          );
    //::Write( ios, obj.m_bEditOptions       );
    ::Write( ios, obj.m_LogicalName        );
    ::Write( ios, obj.m_PhysicalName       );

    ::Write( ios, obj.m_nFieldRelationType );
    ::Write( ios, obj.m_nFieldDataType     );
    ::Write( ios, obj.m_nPrecission        );
    ::Write( ios, obj.m_nScale             );
    ::Write( ios, obj.m_bAllowsNulls       );
    ::Write( ios, obj.m_bAutoIncrement     );
    ::Write( ios, obj.m_DefaultValue       );
    ::Write( ios, obj.m_Remarks            );
}

Field_t& Read( CIOStreamer &ios, Field_t &obj, long version )
{
    ::Read( ios, obj.m_nFirstPos  , version );
    ::Read( ios, obj.m_nLastPos   , version );
    ::Read( ios, obj.m_nAngle               );
    //::Read( ios, obj.m_bHover               );
    //::Read( ios, obj.m_bEditText            );
    //::Read( ios, obj.m_bEditOptions         );
    ::Read( ios, obj.m_LogicalName          );
    ::Read( ios, obj.m_PhysicalName         );

    ::Read( ios, obj.m_nFieldRelationType   );
    ::Read( ios, obj.m_nFieldDataType       );
    ::Read( ios, obj.m_nPrecission          );
    ::Read( ios, obj.m_nScale               );
    ::Read( ios, obj.m_bAllowsNulls         );
    ::Read( ios, obj.m_bAutoIncrement       );
    ::Read( ios, obj.m_DefaultValue         );
    ::Read( ios, obj.m_Remarks              );

    return obj;
}


QJsonObject& Write( QJsonObject &ios, Field_t &obj )
{
    QJsonObject m_nFirstPos ; ::Write( m_nFirstPos , obj.m_nFirstPos  );
    QJsonObject m_nLastPos  ; ::Write( m_nLastPos  , obj.m_nLastPos   );

    //ios[ "m_bHover"             ] =      obj.m_bHover      ;
    ios[ "m_nAngle"             ] =      obj.m_nAngle      ;
    //ios[ "m_bEditText"          ] =      obj.m_bEditText   ;
    //ios[ "m_bEditOptions"       ] =      obj.m_bEditOptions;
    ios[ "m_LogicalName"        ] = QString::fromStdString( obj.m_LogicalName );
    ios[ "m_PhysicalName"       ] = QString::fromStdString( obj.m_PhysicalName);

    ios[ "m_nFieldRelationType" ] =       obj.m_nFieldRelationType;
    ios[ "m_nFieldDataType"     ] =       obj.m_nFieldDataType    ;
    ios[ "m_nPrecission"        ] =       obj.m_nPrecission       ;
    ios[ "m_nScale"             ] =       obj.m_nScale            ;
    ios[ "m_bAllowsNulls"       ] =       obj.m_bAllowsNulls      ;
    ios[ "m_bAutoIncrement"     ] =       obj.m_bAutoIncrement    ;
    ios[ "m_DefaultValue"       ] = QString::fromStdString( obj.m_DefaultValue );
    ios[ "m_Remarks"            ] = QString::fromStdString( obj.m_Remarks      );

    ios[ "m_nFirstPos"          ] =          m_nFirstPos   ;
    ios[ "m_nLastPos"           ] =          m_nLastPos    ;

    return ios;
}

Field_t& Read( QJsonObject &ios, Field_t &obj, long version )
{
    //obj .m_bHover            = ios[ "m_bHover"             ].toBool  ();
    obj .m_nAngle            = ios[ "m_nAngle"             ].toDouble();
    //obj. m_bEditText         = ios[ "m_bEditText"          ].toBool  ();
    //obj. m_bEditOptions      = ios[ "m_bEditOptions"       ].toBool  ();
    obj. m_LogicalName       = ios[ "m_LogicalName"        ].toString().toStdString();
    obj. m_PhysicalName      = ios[ "m_PhysicalName"       ].toString().toStdString();

    obj.m_nFieldRelationType = (FieldRelationType_t)ios[ "m_nFieldRelationType" ].toInt();
    obj.m_nFieldDataType     = (FieldDataType_t    )ios[ "m_nFieldDataType"     ].toInt();
    obj.m_nPrecission        =                      ios[ "m_nPrecission"        ].toInt();
    obj.m_nScale             =                      ios[ "m_nScale"             ].toInt();
    obj.m_bAllowsNulls       =                      ios[ "m_bAllowsNulls"       ].toBool();
    obj.m_bAutoIncrement     =                      ios[ "m_bAutoIncrement"     ].toBool();
    obj.m_DefaultValue       =                      ios[ "m_DefaultValue"       ].toString().toStdString();
    obj.m_Remarks            =                      ios[ "m_Remarks"            ].toString().toStdString();

    auto m_nFirstPos         = ios[ "m_nFirstPos"          ].toObject();
    auto m_nLastPos          = ios[ "m_nLastPos"           ].toObject();

    ::Read( m_nFirstPos , obj.m_nFirstPos , version );
    ::Read( m_nLastPos  , obj.m_nLastPos  , version );
    return obj;
}



QJsonObject& Write( QJsonObject &ios, FieldList_t &obj )
{
    QJsonArray FigureItems;
    for( unsigned long n = 0; n < obj.size(); ++ n )
    {
        QJsonObject Item;
        FigureItems.append( ::Write( Item, *obj.at( n ) ) );
    }
    ios["FieldList"] = FigureItems;

    return ios;
}

FieldList_t& Read( QJsonObject &ios, FieldList_t &obj, long version )
{
    obj.Flush();

    QJsonArray FigureItems = ios["FieldList"].toArray();
    for( unsigned int i = 0; i < (unsigned int)FigureItems.size(); ++ i )
    {
        QJsonObject FigureItem = FigureItems[ i ].toObject();
        obj.Add( &::Read( FigureItem, *new Field_t(), version ) );
    }

    return obj;
}




QJsonObject& Write( QJsonObject &ios, Class_t &obj )
{
    QJsonObject m_nFirstPos ; ::Write( m_nFirstPos , obj.m_nFirstPos  );
    QJsonObject m_nLastPos  ; ::Write( m_nLastPos  , obj.m_nLastPos   );
    QJsonObject m_nTitlePos ; ::Write( m_nTitlePos , obj.m_nTitlePos  );
    QJsonObject m_nAnglePos ; ::Write( m_nAnglePos , obj.m_nAnglePos  );
    QJsonObject m_nResizePos; ::Write( m_nResizePos, obj.m_nResizePos );
    QJsonObject m_FieldList ; ::Write( m_FieldList , obj.m_FieldList  );

    ios[ "m_nFigureType" ] =      obj.m_nFigureType ;
    //ios[ "m_bHover"      ] =      obj.m_bHover      ;
    //ios[ "m_bSelect"     ] =      obj.m_bSelect     ;
    ios[ "m_nAnglesCount"] = (int)obj.m_nAnglesCount;
    ios[ "m_nAngle"      ] =      obj.m_nAngle      ;
    //ios[ "m_bHoverCenter"] =      obj.m_bHoverCenter;
    //ios[ "m_bEditText"   ] =      obj.m_bEditText   ;
    //ios[ "m_bEditOptions"] =      obj.m_bEditOptions;
    ios[ "m_LogicalName" ] =      QString::fromStdString( obj.m_LogicalName );
    ios[ "m_PhysicalName"] =      QString::fromStdString( obj.m_PhysicalName);

    ios[ "m_nFirstPos"   ] =          m_nFirstPos   ;
    ios[ "m_nLastPos"    ] =          m_nLastPos    ;
    ios[ "m_nTitlePos"   ] =          m_nTitlePos   ;
    ios[ "m_nAnglePos"   ] =          m_nAnglePos   ;
    ios[ "m_nResizePos"  ] =          m_nResizePos  ;
    ios[ "m_FieldList"   ] =          m_FieldList   ;

    return ios;
}

Class_t& Read( QJsonObject &ios, Class_t &obj, long version )
{
    obj .m_nFigureType = (FigureType_t)ios[ "m_nFigureType" ].toInt   ();
    //obj .m_bHover      =               ios[ "m_bHover"      ].toBool  ();
    //obj .m_bSelect     =               ios[ "m_bSelect"     ].toBool  ();
    obj .m_nAnglesCount=               ios[ "m_nAnglesCount"].toInt   ();
    obj .m_nAngle      =               ios[ "m_nAngle"      ].toDouble();
    //obj. m_bHoverCenter=               ios[ "m_bHoverCenter"].toBool  ();
    //obj. m_bEditText   =               ios[ "m_bEditText"   ].toBool  ();
    //obj. m_bEditOptions=               ios[ "m_bEditOptions"].toBool  ();
    obj. m_LogicalName =               ios[ "m_LogicalName" ].toString().toStdString();
    obj. m_PhysicalName=               ios[ "m_PhysicalName"].toString().toStdString();

    auto m_nFirstPos   =               ios[ "m_nFirstPos"   ].toObject();
    auto m_nLastPos    =               ios[ "m_nLastPos"    ].toObject();
    auto m_nTitlePos   =               ios[ "m_nTitlePos"   ].toObject();
    auto m_nAnglePos   =               ios[ "m_nAnglePos"   ].toObject();
    auto m_nResizePos  =               ios[ "m_nResizePos"  ].toObject();
    auto m_FieldList   =               ios[ "m_FieldList"   ].toObject();

    ::Read( m_nFirstPos , obj.m_nFirstPos , version );
    ::Read( m_nLastPos  , obj.m_nLastPos  , version );
    ::Read( m_nTitlePos , obj.m_nTitlePos , version );
    ::Read( m_nAnglePos , obj.m_nAnglePos , version );
    ::Read( m_nResizePos, obj.m_nResizePos, version );
    ::Read( m_FieldList , obj.m_FieldList , version );

    return obj;
}






QJsonObject& Write( QJsonObject &ios, Relation_t &obj )
{
    QJsonObject m_nFirstPos;  ::Write( m_nFirstPos, obj.m_nFirstPos );
    QJsonObject m_nLastPos ;  ::Write( m_nLastPos , obj.m_nLastPos  );

    ios[ "m_nRelationType"       ] =                         obj.m_nRelationType       ;
    ios[ "m_nFrom"               ] = (int)                   obj.m_nFrom               ;
    ios[ "m_nTo"                 ] = (int)                   obj.m_nTo                 ;
    ios[ "m_Name"                ] = QString::fromStdString( obj.m_Name               );
    ios[ "m_PKTableLabel"        ] = QString::fromStdString( obj.m_PKTableLabel       );
    ios[ "m_FKTableLabel"        ] = QString::fromStdString( obj.m_FKTableLabel       );
    ios[ "m_nType"               ] = (int)                   obj.m_nType               ;
    ios[ "m_nCardinalityPKTable" ] = (int)                   obj.m_nCardinalityPKTable ;
    ios[ "m_nCardinalityFKTable" ] = (int)                   obj.m_nCardinalityFKTable ;
    ios[ "m_nDeferrability"      ] = (int)                   obj.m_nDeferrability      ;
    ios[ "m_nUpdateRule"         ] = (int)                   obj.m_nUpdateRule         ;
    ios[ "m_nDeleteRule"         ] = (int)                   obj.m_nDeleteRule         ;

    ios[ "m_nFirstPos"           ] =                             m_nFirstPos           ;
    ios[ "m_nLastPos"            ] =                             m_nLastPos            ;
    //ios[ "m_bHover"        ] =      obj.m_bHover       ;
    return ios;
}

Relation_t& Read( QJsonObject &ios, Relation_t &obj, long version )
{
    obj.m_nRelationType       = (RelationType_t                 )ios[ "m_nRelationType"       ].toInt   ();
    obj.m_nFrom               =                                  ios[ "m_nFrom"               ].toInt   ();
    obj.m_nTo                 =                                  ios[ "m_nTo"                 ].toInt   ();
    obj.m_Name                =                                  ios[ "m_Name"                ].toString().toStdString();
    obj.m_PKTableLabel        =                                  ios[ "m_PKTableLabel"        ].toString().toStdString();
    obj.m_FKTableLabel        =                                  ios[ "m_FKTableLabel"        ].toString().toStdString();
    obj.m_nType               = (RelationPropertyType_t         )ios[ "m_nType"               ].toInt   ();
    obj.m_nCardinalityPKTable = (RelationPropertyCardinality_t  )ios[ "m_nCardinalityPKTable" ].toInt   ();
    obj.m_nCardinalityFKTable = (RelationPropertyCardinality_t  )ios[ "m_nCardinalityFKTable" ].toInt   ();
    obj.m_nDeferrability      = (RelationPropertyDeferrability_t)ios[ "m_nDeferrability"      ].toInt   ();
    obj.m_nUpdateRule         = (RelationPropertyRule_t         )ios[ "m_nUpdateRule"         ].toInt   ();
    obj.m_nDeleteRule         = (RelationPropertyRule_t         )ios[ "m_nDeleteRule"         ].toInt   ();

    auto m_nFirstPos          =                                  ios[ "m_nFirstPos"           ].toObject();
    auto m_nLastPos           =                                  ios[ "m_nLastPos"            ].toObject();
  //obj.m_bHover              =                                  ios[ "m_bHover"              ].toBool  ();

    ::Read( m_nFirstPos, obj.m_nFirstPos, version );
    ::Read( m_nLastPos , obj.m_nLastPos , version );

    return obj;
}

void Write( CIOStreamer &ios, Relation_t &obj )
{
    ::WriteInt( ios, obj.m_nRelationType       );
    ::Write   ( ios, obj.m_nFrom               );
    ::Write   ( ios, obj.m_nTo                 );
    ::Write   ( ios, obj.m_nFirstPos           );
    ::Write   ( ios, obj.m_nLastPos            );
    ::Write   ( ios, obj.m_Name                );
    ::Write   ( ios, obj.m_PKTableLabel        );
    ::Write   ( ios, obj.m_FKTableLabel        );
    ::WriteInt( ios, obj.m_nType               );
    ::WriteInt( ios, obj.m_nCardinalityPKTable );
    ::WriteInt( ios, obj.m_nCardinalityFKTable );
    ::WriteInt( ios, obj.m_nDeferrability      );
    ::WriteInt( ios, obj.m_nUpdateRule         );
    ::WriteInt( ios, obj.m_nDeleteRule         );
    //::Write   ( ios, obj.m_bHover        );
}

Relation_t& Read( CIOStreamer &ios, Relation_t &obj, long version )
{
    ::ReadInt( ios, obj.m_nRelationType          );
    ::Read   ( ios, obj.m_nFrom                  );
    ::Read   ( ios, obj.m_nTo                    );
    ::Read   ( ios, obj.m_nFirstPos    , version );
    ::Read   ( ios, obj.m_nLastPos     , version );
    ::Read   ( ios, obj.m_Name                   );
    ::Read   ( ios, obj.m_PKTableLabel           );
    ::Read   ( ios, obj.m_FKTableLabel           );
    ::ReadInt( ios, obj.m_nType                  );
    ::ReadInt( ios, obj.m_nCardinalityPKTable    );
    ::ReadInt( ios, obj.m_nCardinalityFKTable    );
    ::ReadInt( ios, obj.m_nDeferrability         );
    ::ReadInt( ios, obj.m_nUpdateRule            );
    ::ReadInt( ios, obj.m_nDeleteRule            );
    //::Read   ( ios, obj.m_bHover                 );
    return obj;
}


void Write( CIOStreamer &ios, FieldList_t &obj )
{
    unsigned long n = 0, count = 0;
    count = obj.size(); ::Write( ios, count );
    for( n = 0; n < count; ++ n )
    {
        auto &pItem = obj.at( n );
        ::Write( ios, *pItem );
    }
}

FieldList_t& Read( CIOStreamer &ios, FieldList_t &obj, long version )
{
    unsigned long n = 0, count = 0;
    ::Read( ios, count );

    obj.Flush();
    for( n = 0; n < count; ++ n )
    {
        obj.Add( &::Read( ios, *new Field_t(), version ) );
    }

    return obj;
}




Class_t& Read( CIOStreamer &ios, Class_t &obj, long version )
{
    ::ReadInt( ios, obj.m_nFigureType          );
    ::Read   ( ios, obj.m_nFirstPos  , version );
    ::Read   ( ios, obj.m_nLastPos   , version );
    ::Read   ( ios, obj.m_nTitlePos  , version );
    ::Read   ( ios, obj.m_nAnglePos  , version );
    ::Read   ( ios, obj.m_nResizePos , version );
    ::Read   ( ios, obj.m_nAngle               );
    ::Read   ( ios, obj.m_nAnglesCount         );
    //::Read   ( ios, obj.m_bHover               );
    //::Read   ( ios, obj.m_bSelect              );
    //::Read   ( ios, obj.m_bHoverCenter         );
    //::Read   ( ios, obj.m_bEditText            );
    //::Read   ( ios, obj.m_bEditOptions         );
    ::Read   ( ios, obj.m_LogicalName          );
    ::Read   ( ios, obj.m_PhysicalName         );
    ::Read   ( ios, obj.m_FieldList  , version );

    return obj;
}
