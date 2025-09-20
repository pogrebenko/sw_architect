/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Rectangle.h"

#include <QtMath>
#include <QRectF>
#include <QPainterPath>

#include "common/Utils.h"

CRectangle::CRectangle()
: Class_t( FigureTypeRectangle )
{
    m_nAnglesCount = 4;
}

bool CRectangle::contain( const QPoint &mousePos )
{
    return contain_rectangle( mousePos, m_nFirstPos, m_nLastPos, m_nAngle );
}
