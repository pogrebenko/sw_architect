/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Triangle.h"

#include <math.h>

#include <QRectF>
#include <QPainterPath>

CTriangle::CTriangle()
: Class_t( FigureTypeTriangle )
{
    m_nAnglesCount = 3;
}
