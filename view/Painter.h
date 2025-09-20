/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef PAINTER_H
#define PAINTER_H

#include <QPainterPath>
#include <QPainter>
#include <QPixmap>

#include "common/Utils.h"
#include "data/general//Class.h"
#include "data/general/Relation.h"

class CAppOptions;

class CPainter
{
    friend class CPaintWidget;

public:
             CPainter( CAppOptions *pAppOptions );
    virtual ~CPainter() {}

    void painter_draw( QPainter &painter );
    void painter_resize( const QRect &r );

    void draw_figure_relation();

    void draw_figure_new   ();
    void draw_figure       ( QPainter &painter, ClassList_t   *FigureList   );
    void draw_figure_item  ( QPainter &painter, Class_t       &Figure       );

    void draw_relation_new ();
    void draw_relation     ( QPainter &painter, RelationList_t *RelationList );
    void draw_relation_item( QPainter &painter, Relation_t     &Relation     );
    void draw_relation_line( QPainter &painter, QPen &pen, RelationPropertyType_t LineType, QPointF &startPoint, QPointF &endPoint );
    void draw_relation_cardinality( QPainter &painter, QPen &pen, RelationPropertyCardinality_t Cardinality, QPointF &startPoint, QPointF &endPoint, __PointType &line, float r, int d, qreal arrowSize );

    void draw_title_border( QPainter &painter, Class_t &pClass );
    void draw_title ( QPainter &painter, Class_t &pClass );
    void draw_fields( QPainter &painter, Class_t &pClass );
    void draw_field ( QPainter &painter, Class_t &pClass, Field_t &pItem, bool bLastItem );
    void draw_row   ( QPainter &painter, QPoint &p1, QPoint &p2, std::string &Name, FieldRelationType_t Type, bool bShowLine, bool bFontBold );
    void draw_selected_area( QPainter &painter );
    void draw_points( QPainter &painter, Class_t &pItem );

private:
    CAppOptions *m_pAppOptions;
    QPixmap      m_pixmap;
};

#endif // PAINTER_H
