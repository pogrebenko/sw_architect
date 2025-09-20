/*
Project: Software Architect
File: History.h
Licensee: MIT
Source: https://github.com/pogrebenko/sw_architect
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef HISTORY_H
#define HISTORY_H

#include <string>
#include <vector>

#include <QPoint>

class CHistory : public std::vector< std::string >
{
public:
    inline CHistory() : std::vector< std::string >(), m_nCurrent( 0 ), m_sFunction(__PRETTY_FUNCTION__), m_n(-1), m_m(-1) { Save( __PRETTY_FUNCTION__ ); }

    inline bool Undo () { if( m_nCurrent >          0 && size() > 0 ) { -- m_nCurrent; Load(); return true; } return false; }
    inline bool Redo () { if( m_nCurrent < size() - 1 && size() > 0 ) { ++ m_nCurrent; Load(); return true; } return false; }
    inline void Flush() { m_nCurrent = 0; m_n=-1; m_m=-1; clear(); Save( __PRETTY_FUNCTION__ ); }

    void Save( const char *fn );
    void Save( const char *fn, const QPoint &pt, long n, long m );

protected:
    void Load();

private:
    unsigned long m_nCurrent;
    std::string   m_sFunction;
    QPoint        m_pt;
    long          m_n, m_m;
};

#endif // HISTORY_H
