/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef ELLIPSE_H
#define ELLIPSE_H

#include "data/general/Class.h"

class CEllipse : public Class_t
{
public:
    CEllipse();
    virtual bool contain( const QPoint &obj ) override;
};

#endif // ELLIPSE_H
