/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "app.h"

#include <QIcon>

#include "common/Consts.h"

extern CLogStream __Logger;

CApp::CApp( int& argc, char** argv, Logger_t * const pLogger )
: QApplication(argc, argv)
, CLoggerHolder( pLogger )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    setOrganizationName  ( APP_COMPANY );
    setApplicationName   ( APP_NAME    );
    setApplicationVersion( APP_VERSION );
}

bool CApp::notify(QObject* receiver, QEvent* event)
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    bool done = true;
    try
    {
        done = QApplication::notify(receiver, event);
    }
    catch (const std::exception& ex)
    {
        getLogger()->error( APP_LOG_LEVEL, "error: exception %s", ex.what() );
    }
    catch (const std::string& ex)
    {
        getLogger()->error( APP_LOG_LEVEL, "error: string %s", ex.c_str() );
    }
    catch (...)
    {
        getLogger()->error( APP_LOG_LEVEL, "error: %s", "Caught unknown exception" );
    }
    return done;
}

