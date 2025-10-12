/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef UTILS_H
#define UTILS_H

#include "data/general/Relation.h"

#include <QVariantMap>

class CIOStreamer;
class QPoint;

using PointType_t = struct __PointType
{
    QPointF                  m_nPoint           ;
    QLineF::IntersectionType m_nIntersectionType;
};

extern QPoint  pointf2point( const QPointF &pt );
extern QPointF point2pointf( const QPoint  &pt );

extern QPointF correct_point( const QPointF &pt, const QRectF &item );

extern bool hex2rgb( const QString &sHex, int &r, int &g, int &b );
extern void rgb2hex( QString &sHex, int  r, int  g, int  b );

extern bool hex2rgb( const std::string &sHex, int &r, int &g, int &b );
extern void rgb2hex( std::string &sHex, int  r, int  g, int  b );

extern QColor hex2color( const QString &sHex );

extern void draw_relation_intersection( std::vector <__PointType>&, const __Class*, const QLineF& );

extern void        Write( CIOStreamer &ios, QPoint &obj );
extern QPoint&     Read ( CIOStreamer &ios, QPoint &obj, long );

extern void        Write( CIOStreamer &ios, Field_t &obj );
extern Field_t&    Read ( CIOStreamer &ios, Field_t &obj, long version );

extern void        Write( CIOStreamer &ios, Class_t &obj );
extern Class_t&    Read ( CIOStreamer &ios, Class_t &obj, long version );

extern void        Write( CIOStreamer &ios, Relation_t &obj );
extern Relation_t& Read ( CIOStreamer &ios, Relation_t &obj, long version );


extern QJsonObject&Write( QJsonObject &ios, QPoint &obj );
extern QPoint&     Read ( QJsonObject &ios, QPoint &obj, long );

extern QJsonObject&Write( QJsonObject &ios, Field_t &obj );
extern Field_t&    Read ( QJsonObject &ios, Field_t &obj, long version );

extern QJsonObject&Write( QJsonObject &ios, Class_t &obj );
extern Class_t&    Read ( QJsonObject &ios, Class_t &obj, long version );

extern QJsonObject&Write( QJsonObject &ios, Relation_t &obj );
extern Relation_t& Read ( QJsonObject &ios, Relation_t &obj, long version );

extern bool contain_rectangle( const QPoint &mousePos, const QPoint &nFirstPos, const QPoint &nLastPos, float nAngle );

#endif // UTILS_H
