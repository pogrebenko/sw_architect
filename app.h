/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef APP_H
#define APP_H

#include <QApplication>

#include "common/Logger.h"

class CApp : public QApplication
           , public CLoggerHolder
{
public :

    CApp( int& argc, char** argv, Logger_t * const pLogger );

    bool notify(QObject* receiver, QEvent* event) override;
};


#endif // APP_H
