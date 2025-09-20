/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Ellipse.h"

#include <QtMath>
#include <QRectF>
#include <QPainterPath>

CEllipse::CEllipse()
: Class_t( FigureTypeEllipse )
{
    m_nAnglesCount = 0;
}

bool CEllipse::contain( const QPoint &mousePos )
{
    QRectF       rcf;
                 rcf.setTopLeft    ( m_nFirstPos );
                 rcf.setBottomRight( m_nLastPos  );

    QPointF      center( rcf.center() );

    QPainterPath path;
                 path.addEllipse( rcf );

    //return ellipsePath.contains( mousePos );

    QTransform   transform;
                 transform.translate( center.x(), center.y() );
                 transform.rotate( -qRadiansToDegrees( m_nAngle ) );
                 transform.translate( -center.x(), -center.y() );

    return transform.map( path ).contains( mousePos );
}
