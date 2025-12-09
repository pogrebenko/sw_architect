/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef BUILDER_H
#define BUILDER_H

#include "common/Consts.h"
#include "common/Logger.h"

#include <map>

class CAppOptions;
class QPoint;
class QSize;
class QRect;

struct __Class;
struct __Field;
struct __Relation;

class CBuilder : public CLoggerHolder
{
public:
             CBuilder( CAppOptions * const pAppOptions, Logger_t * const pLogger );
    virtual ~CBuilder() {}

    ActionType_t get_action_type();
    bool check_action_type( ActionType_t ActionType );

    void hover_reset();
    void edit_reset();
    void select_reset();

    // relation section
    long relation_hover( const QPoint &pos, bool set_up_hover );
    bool relation_delete( long n );
    void relation_delete_fields( __Relation *toDelete );
    bool relation_rotate_type( long n );
    bool relation_begin_add( long n );
    bool relation_release_add( const QPoint &pos, __Field *fromFK );
    bool relation_release_add( long nIndexFrom, long nIndexTo, long i );
    bool relation_set_last_pos( const QPoint &pos );
    void relation_rebuild();
    bool relation_move( const QPoint &pos );

    // figure section
    long figure_hover( const QPoint &pos, bool set_up_hover );
    bool figure_delete( long n );
    bool figure_select( long n );
    bool figure_select( const QRect &rc );
    bool figure_rotate_type( long n );
    bool figure_begin_add( const QPoint &pos );
    bool figure_release_add();
    void figure_set_first_pos( long n, const QPoint &pos );
    bool figure_set_last_pos( const QPoint &pos );
    bool figure_move( const QPoint &pos );
    void figure_rebuild( long n );
    void figure_area( QPoint &first_pos, QPoint &last_pos );
    QSize &figure_size( QSize &area_size, const QSize &widget_size );
    bool figure_intersect( const QRect &rc );

    __Field* field_add( long n, const char* );

    long resize_hover( const QPoint &pos, bool set_up_hover, bool set_up_resize );
    long rotate_hover( const QPoint &pos, bool set_up_hover, bool set_up_resize );
    long center_hover( const QPoint &pos, bool set_up_hover );
    long title_hover ( const QPoint &pos, bool set_up_hover );

    void resize_item( const QPoint &pos, __Class *pItem );

    bool align_horizontaly_left();
    bool align_vertically_up();
    bool align_horizontaly_right();
    bool align_vertically_down();

    std::map<std::string,long> field_hover( const QPoint &pos, bool set_up_hover );
    bool field_delete( std::map<std::string,long> &rc );
    std::map<std::string,long> find_edit();

    long relation_oscillation( QPoint &pt, int dx, int dy );
    long figure_oscillation( QPoint &pt, int dx, int dy );

private:
    CAppOptions *m_pAppOptions;
};

#endif // BUILDER_H
