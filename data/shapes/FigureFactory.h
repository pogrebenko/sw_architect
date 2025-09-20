/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef FIGUREFACTORY_H
#define FIGUREFACTORY_H

#include "common/IOStreamer.h"
#include "data/general/Class.h"
#include "data/general/Relation.h"
#include "common/Utils.h"

#include <QJsonObject>

class CFigureFactory
{
public:
             CFigureFactory();
    virtual ~CFigureFactory();

    static Class_t   * buildFigure  ( FigureType_t );
    static Class_t   * buildFigure  ( FigureType_t, const QPoint&, const QPoint&, int nHeight );

    static Field_t   * buildField   ( const QPoint&, const QPoint& );

    static Relation_t* buildRelation( RelationType_t );
    static Relation_t* buildRelation( RelationType_t, unsigned int, unsigned int, const QPoint&, const QPoint& );


    static QJsonObject    & Write( QJsonObject &ios, Class_t &obj );
    static Class_t        & Read ( QJsonObject &ios, Class_t &obj, long version );

    static QJsonObject    & Write( QJsonObject &ios, ClassList_t &obj );
    static ClassList_t    & Read ( QJsonObject &ios, ClassList_t &obj, long version );


    static void             Write( CIOStreamer &ios, Class_t &obj );
    static Class_t        & Read ( CIOStreamer &ios, Class_t &obj, long version );

    static void             Write( CIOStreamer &ios, ClassList_t &obj );
    static ClassList_t    & Read ( CIOStreamer &ios, ClassList_t &obj, long version );



    static QJsonObject    & Write( QJsonObject &ios, Relation_t &obj );
    static Relation_t     & Read ( QJsonObject &ios, Relation_t &obj, long version );

    static QJsonObject    & Write( QJsonObject &ios, RelationList_t &obj );
    static RelationList_t & Read ( QJsonObject &ios, RelationList_t &obj, long version );


    static void             Write( CIOStreamer &ios, Relation_t &obj );
    static Relation_t     & Read ( CIOStreamer &ios, Relation_t &obj, long version );

    static void             Write( CIOStreamer &ios, RelationList_t &obj );
    static RelationList_t & Read ( CIOStreamer &ios, RelationList_t &obj, long version );
};




inline void Write( CIOStreamer &ios, ClassList_t &obj ){
    CFigureFactory::Write( ios, obj );
}
inline ClassList_t& Read( CIOStreamer &ios, ClassList_t &obj, long version ){
    return CFigureFactory::Read( ios, obj, version );
}

inline void Write( CIOStreamer &ios, RelationList_t &obj ){
    CFigureFactory::Write( ios, obj );
}
inline RelationList_t& Read ( CIOStreamer &ios, RelationList_t &obj, long version ){
    return CFigureFactory::Read( ios, obj, version );
}




inline QJsonObject& Write( QJsonObject &ios, ClassList_t &obj ){
    return CFigureFactory::Write( ios, obj );
}
inline ClassList_t& Read( QJsonObject &ios, ClassList_t &obj, long version ){
    return CFigureFactory::Read( ios, obj, version );
}

inline QJsonObject& Write( QJsonObject &ios, RelationList_t &obj ){
    return CFigureFactory::Write( ios, obj );
}
inline RelationList_t& Read ( QJsonObject &ios, RelationList_t &obj, long version ){
    return CFigureFactory::Read( ios, obj, version );
}

#endif // FIGUREFACTORY_H
