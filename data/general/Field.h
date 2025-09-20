/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef FIELD_H
#define FIELD_H

// for release check #define NDEBUG
#include <assert.h>

#include <QLineF>

#include "common/Structs.h"

struct __Class;

typedef struct __Field : public __Field__
{
    inline __Field() { init(); }
    inline __Field( const __Field &o ) { *this = o; }

    __Field& operator = ( const __Field &o );

    void init();

    bool contain_title( const QPoint&, const __Class *pClass );

} Field_t;

class CFieldHolder : public std::shared_ptr<Field_t>
{
public :
    inline CFieldHolder( Field_t *pItem ) : std::shared_ptr<Field_t>( pItem ) {}
};

class FieldList_t : public std::vector< CFieldHolder >
{
public:
    inline FieldList_t() : std::vector< CFieldHolder > () {}
    inline FieldList_t( const FieldList_t &o ) : std::vector< CFieldHolder > () { *this = o; }

    inline FieldList_t& operator = ( const FieldList_t &o )
    {
        std::for_each( o.begin(), o.end(), [this]( auto &pItem ){ Add( new __Field( *pItem.get() ) ); } );
        return *this;
    }

    inline void Flush(){ clear(); }
    inline void Add( Field_t *pItem ) { this->push_back( CFieldHolder( pItem ) ); }
    inline void Delete( Field_t *toDelete ) {
        erase(
            std::remove_if( begin(), end(),
               [toDelete]( auto &Item )
               {
                   return Item.get() == toDelete;
               }
            ), end()
        );
    }

    void hover_clear();

    long hover_title( const QPoint &pos, const __Class *pClass );
    long find_edit();
    long find_fromFK();
};


#endif // FIELD_H
