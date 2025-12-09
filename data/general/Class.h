/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef CLASS_H
#define CLASS_H

#include <memory>
#include <vector>
#include <map>

#include "data/general/Field.h"

typedef struct __Class : public __Class__
{
    FieldList_t   m_FieldList;

    inline          __Class( const FigureType_t nFigureType ) { init(); m_nFigureType = nFigureType; }
    inline          __Class( const __Class &o ) { *this = o; }

    __Class& operator = ( const __Class &o );

    virtual bool  contain( const QPoint& );

    bool          compare( __Class* );
    bool          compare( const QPoint& );

    void          init();
    bool          contain_title( const QPoint& );
    void          move( const QPoint &from, const QPoint &to );
    const QPoint& center( QPoint *point );
    bool          valid();
    bool          near_points( const QPoint &pt1, const QPoint &pt2 );
    void          calc_angle_point( const QPoint &pt );
    void          calc_angle_resize_point( int nHeight );
    void          calc_first_last_point( const QPoint &pt );
    void          oscillation( int dx, int dy );

    void          enumFields( FieldList_t &List, FieldRelationType_t Type );
    void          aggregateFields( FieldList_t &PK, FieldList_t &FK, FieldList_t &NN );
    void          field_recalc();

} Class_t;

class CClassHolder : public std::shared_ptr<Class_t>
{
public :
    inline CClassHolder( Class_t *pItem ) : std::shared_ptr<Class_t>( pItem ) {}
};

class ClassList_t : public std::vector< CClassHolder >
{
public:
    inline ClassList_t() : std::vector< CClassHolder > () {}
    inline ClassList_t( const ClassList_t& o ) : std::vector< CClassHolder > () { *this = o; }

    inline ClassList_t& operator = ( const ClassList_t& o )
    {
        Flush();
        std::for_each( o.begin(), o.end(), [this]( auto &pItem ){ Add( new __Class( *pItem.get() ) ); } );
        return *this;
    }

    void sort();
    long find( const QPoint &pos );

    void hover_clear();
    long hover_title( const QPoint &pos );
    long hover_index( const QPoint &pos );
    long hover_angle_index( const QPoint &pos );
    long hover_resize_index( const QPoint &pos );
    long hover_first_index( const QPoint &pos );
    long hover_last_index( const QPoint &pos );
    long hover_center_index( const QPoint &pos );

    void select_clear();

    void edit_clear();
    void edit_title( const std::string &text );
    std::map<std::string,long> find_edit();
    std::map<std::string,long> find_fromFK();

    inline void Flush(){ clear(); }
    inline void Delete( unsigned long i ) { this->erase( this->begin() + i ); }
    inline void Add( Class_t *pItem ) { this->push_back( CClassHolder( pItem ) ); }

    inline Field_t* At( long n, long m )
    {
        return this->at( n )->m_FieldList[ m ].get();
    }

    inline void Delete( long n, long m )
    {
        this->at( n )->m_FieldList.erase( this->at( n )->m_FieldList.begin() + m );
    }
};


#endif // CLASS_H
