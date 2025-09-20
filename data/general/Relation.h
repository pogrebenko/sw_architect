/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef RELATION_H
#define RELATION_H

#include "common/Structs.h"

#include "data/general/Class.h"

typedef struct __Relation : public __Relation__
{
    inline __Relation( RelationType_t nRelationType ) { init();  m_nRelationType = nRelationType; }
    inline __Relation( const __Relation& o ) { *this = o; }

    __Relation& operator = ( const __Relation& o );

    void  init();

    void  calculate( Class_t &pFrom, Class_t &pTo );

    float distancePointToLine( const QPoint &p1, const QPoint &p2, const QPoint &p );
    bool  isPointNearLine(const QPoint &p1, const QPoint &p2, const QPoint &p, qreal tolerance = DEFAULT_TOLERANCE );
    bool  contain( const QPoint &point );
    void  move( const QPoint &from, const QPoint &to );
    void  oscillation( int dx, int dy );

} Relation_t;

class CRelationHolder : public std::shared_ptr<Relation_t>
{
public :
    inline CRelationHolder( Relation_t *pItem ) : std::shared_ptr<Relation_t>( pItem ) {}
};

class RelationList_t : public std::vector< CRelationHolder >
{
public:
    inline RelationList_t() : std::vector< CRelationHolder > () {}
    inline RelationList_t( const RelationList_t &o ) : std::vector< CRelationHolder > () { *this = o; }

    inline RelationList_t& operator = ( const RelationList_t &o )
    {
        Flush();
        std::for_each( o.begin(), o.end(), [this]( auto &pItem ){ Add( new __Relation( *pItem.get() ) ); } );
        return *this;
    }

    bool validate( long nFrom, long nTo );
    void calculate( ClassList_t *pFigureList );

    void hover_clear();
    long hover_index( const QPoint &pos );

    inline void Flush(){ clear(); }
    inline void Add( Relation_t *pItem ) { this->push_back( CRelationHolder( pItem ) ); }
    inline void Delete( unsigned long i ) { this->erase( this->begin() + i ); }
    inline void Delete( Relation_t *toDelete ) {
        erase(
            std::remove_if( begin(), end(),
               [toDelete]( auto &Item )
               {
                   return Item.get() == toDelete;
               }
            ), end()
        );
    }

};

#endif // RELATION_H
