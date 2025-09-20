/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QLocale>
#include <QTranslator>

#include "data/Options.h"
#include "data/History.h"

#include "app.h"
#include "mainwindow.h"

CLogStream                   __Logger;
std::unique_ptr<CAppOptions> __AppOptions__( new CAppOptions() );
CHistory                     __History;

#define __AppOptions (*__AppOptions__.get())


int main(int argc, char *argv[])
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    int done = -1;
    try
    {
        CApp app( argc, argv, &__Logger );
             app.setApplicationName( APP_NAME );
             app.setWindowIcon( QIcon(":/images/rocket-launch.svg") );
        // https://heroicons.com/outline

        MainWindow w;
                   w.setWindowTitle( APP_NAME );
                   w.setUnifiedTitleAndToolBarOnMac( true );
                   w.show();

        done = app.exec();
    }
    catch (const std::exception& ex)
    {
        __Logger.error( APP_LOG_LEVEL, "error: %s", ex.what() );
    }
    catch (const std::string& ex)
    {
        __Logger.error( APP_LOG_LEVEL, "error: %s", ex.c_str() );
    }
    catch (...)
    {
        __Logger.error( APP_LOG_LEVEL, "error: %s", "Unknown exception" );
    }
    return done;
}
