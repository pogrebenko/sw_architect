/*
Project: Software Architect
File: History.cpp
Licensee: MIT
Source: https://github.com/pogrebenko/sw_architect
Copyright (C) 2025, pogrebenko
*/
#include "data/History.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "common/Logger.h"
#include "common/Utils.h"

#include "data/Options.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
#define __AppOptions (*__AppOptions__.get())

void
CHistory::Save( const char *sFunction )
{
    QPoint pt;
    Save( sFunction, pt, -1, -1 );
}

void
CHistory::Save( const char *sFunction, const QPoint &pt, long n, long m )
{
    QJsonObject obj;
    std::string project = QJsonDocument( ::Write( obj, __AppOptions ) ).toJson().toStdString();

    if( empty() )
    {
        push_back( project );
        m_nCurrent = 0;
        __Logger.info( APP_LOG_LEVEL, "history save [%d]", m_nCurrent );
    }
    else
    {
        if( project.compare( at( m_nCurrent ) ) != 0 )
        {
            bool not_found = false;
            if( m_sFunction.compare( sFunction ) == 0 )
            {
                if( n >= 0 )
                {
                    if( m_n == n )
                    {
                        if( m >= 0 )
                        {
                            if( m_m == m )
                            {

                                at( m_nCurrent ) = project;
                            }
                            else
                            {
                                not_found = true;
                            }
                        }
                        else
                        {
                            at( m_nCurrent ) = project;
                        }
                    }
                    else
                    {
                        not_found = true;
                    }
                }
                else
                {
                    if( m_pt == pt )
                    {
                        at( m_nCurrent ) = project;
                    }
                    else
                    {
                        not_found = true;
                    }
                }
            }
            else
            {
                not_found = true;
            }

            if( not_found )
            {
                if( m_nCurrent < size() - 1 ) erase( begin() + m_nCurrent + 1, end() );
                push_back( project );
                m_nCurrent = size() - 1;

                m_sFunction = sFunction;
                m_pt        = pt       ;
                m_n         = n        ;
                m_m         = m        ;

                __Logger.info( APP_LOG_LEVEL, "history save [%d]", m_nCurrent );
            }
        }
    }
}

void
CHistory::Load()
{
    QJsonObject obj = QJsonDocument::fromJson( this->at( m_nCurrent ).c_str() ).object();
    ::Read( obj, __AppOptions, APP_DATA_VERSION );

    __Logger.info( APP_LOG_LEVEL, "history load [%d]", m_nCurrent );
}

