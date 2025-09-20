/*
Project: Software Architect
File: templates.h
Licensee: MIT
Source: https://github.com/pogrebenko/sw_architect
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <QPoint>

template<class T>
class CRefreshMainWnd
{
public:
    inline CRefreshMainWnd( T *pParent, const char *sMsg = nullptr )
        : m_pParent( pParent ), m_sMsg( sMsg ) { m_pParent->setCursor( Qt::WaitCursor  ); }
    inline virtual ~CRefreshMainWnd() { m_pParent->refresh(); m_pParent->setCursor( Qt::ArrowCursor ); }

private:
    T *m_pParent;
    const char* m_sMsg;
};

template<class T, class H>
class CRefreshCanvas
{
public:
    inline CRefreshCanvas( const char* f, T *pParent, H *pHistory, const QPoint* pt=nullptr, long i=-1, long j=-1, bool p0=true, bool p1=true, bool p2=false, bool p3=false )
        : m_pParent( pParent ), m_pHistory(pHistory), m_f(f), m_pt(pt), m_i(i), m_j(j), m_p0(p0), m_p1(p1), m_p2(p2), m_p3(p3) {}
    inline virtual ~CRefreshCanvas()
    {
        if( m_p0 )
        {
            m_pParent->refresh( m_p1, m_p2, m_p3 );
            QPoint pt; if( m_pt != nullptr ) pt = *m_pt;
            m_pHistory->Save( m_f, pt, m_i, m_j );
        }
    }

private:
    T            *m_pParent;
    H            *m_pHistory;
    const char   *m_f;
    const QPoint *m_pt;
    long          m_i, m_j;
    bool          m_p0, m_p1, m_p2, m_p3;
};

#endif // TEMPLATES_H
