/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "data/general/Class.h"

class CRectangle : public Class_t
{
public:
    CRectangle();
    virtual bool contain( const QPoint &obj ) override;
};

#endif // RECTANGLE_H
