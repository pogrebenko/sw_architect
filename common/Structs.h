/*
Project: Software Architect
File: Structs.h
Licensee: MIT
Source: https://github.com/pogrebenko/sw_architect
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

#include <QPoint>
#include <QString>

#include "common/export.h"
#include "common/Consts.h"
#include "psql/sqltype.h"

struct __Field__
{
    std::string         m_LogicalName       ;
    std::string         m_PhysicalName      ;
    FieldRelationType_t m_nFieldRelationType;
    FieldDataType_t     m_nFieldDataType    ;
    int                 m_nPrecission       ;
    int                 m_nScale            ;
    bool                m_bAllowsNulls      ;
    bool                m_bAutoIncrement    ;
    std::string         m_DefaultValue      ;
    std::string         m_Remarks           ;

    bool                m_bHover            ;
    QPoint              m_nFirstPos         ;
    QPoint              m_nLastPos          ;
    float               m_nAngle            ;
    bool                m_bEditText         ;
    bool                m_bEditOptions      ;
    bool                m_bChanged          ;
    bool                m_bFromFK           ;

    inline virtual ~__Field__() {}
};

struct __Class__
{
    FigureType_t        m_nFigureType ;
    QPoint              m_nFirstPos   ;
    QPoint              m_nLastPos    ;
    QPoint              m_nTitlePos   ;
    QPoint              m_nAnglePos   ;
    QPoint              m_nResizePos  ;
    bool                m_bHover      ;
    bool                m_bHoverTitle ;
    bool                m_bHoverFirst ;
    bool                m_bHoverLast  ;
    bool                m_bHoverAngle ;
    bool                m_bHoverResize;
    bool                m_bHoverCenter;
    bool                m_bEditText   ;
    bool                m_bEditOptions;
    float               m_nAngle      ;
    bool                m_bSelect     ;
    long                m_nAnglesCount;
    std::string         m_LogicalName ;
    std::string         m_PhysicalName;
    bool                m_bChanged    ;

    inline virtual ~__Class__() {}

};

struct __Relation__
{
    RelationType_t                  m_nRelationType      ;
    long                            m_nFrom              ;
    long                            m_nTo                ;
    QPoint                          m_nFirstPos          ;
    QPoint                          m_nLastPos           ;
    bool                            m_bHover             ;
    bool                            m_bChanged           ;
    std::string                     m_Name               ;
    std::string                     m_PKTableLabel       ;
    std::string                     m_FKTableLabel       ;
    RelationPropertyType_t          m_nType              ;
    RelationPropertyCardinality_t   m_nCardinalityPKTable;
    RelationPropertyCardinality_t   m_nCardinalityFKTable;
    RelationPropertyDeferrability_t m_nDeferrability     ;
    RelationPropertyRule_t          m_nUpdateRule        ;
    RelationPropertyRule_t          m_nDeleteRule        ;
    long                            m_dX                 ;
    long                            m_dY                 ;

    inline virtual ~__Relation__() {}

};

struct __Options__
{
    int                 m_nAppVariant      ;
    QString             m_ProjectName      ;
    //QString             m_DatabaseName     ;
    QString             m_TablePrefix      ;
    QString             m_ClassPrefix      ;
    float               m_nWidth           ;
    float               m_nWidthHover      ;
    int                 m_nHeight          ;
    int                 m_nPadding         ;
    QString             m_PenColor         ;
    QString             m_PenColorSelect   ;
    Qt::GlobalColor     m_PenColorRotate   ;
    Qt::GlobalColor     m_PenColorResize   ;
    Qt::GlobalColor     m_BrushColor       ;
    Qt::GlobalColor     m_BkgColor         ;
    Qt::PenStyle        m_PenStyle         ;
    Qt::PenStyle        m_PenFKStyle       ;
    Qt::PenStyle        m_PenSelectStyle   ;
    Qt::ArrowType       m_ArrowType        ;
    Qt::BrushStyle      m_BrushStyle       ;
    Qt::BrushStyle      m_BrushTitleStyle  ;
    QString             m_BrushTitleColor  ;
    float               m_nCPRadius        ;
    FigureType_t        m_nFigureType      ;
    ActionType_t        m_nActionType      ;
    RelationType_t      m_nRelationType    ;
    long                m_nIndexFrom       ;
    int                 m_nArrowAngle      ;
    float               m_nArrowSize       ;
    QPoint              m_nFirstPos        ;
    QPoint              m_nLastPos         ;
    bool                m_bChanged         ;
    bool                m_bSelectGroup     ;
    float               m_nRatio           ;
    RelationNotation_t  m_nRelationNotation;

    //DatabaseType_t      m_nDatabaseType    ;
    PDatabases          m_nDB              ;
    QString             m_ServerName       ;
    QString             m_Port             ;
    QString             m_BaseName         ;
    QString             m_UserName         ;
    QString             m_UserPass         ;
    QString             m_SysAdmin         ;
    PHDBPROVIDER        m_hProvider        ;

    inline virtual ~__Options__() {}

};

struct __DBCol__ {
    TCHAR table_name    [ 256 ],
          column_name   [ 256 ],
          datatype      [  64 ],
          key           [  64 ],
          is_null       [  36 ],
          column_comment[ 256 ];
    long  length, precision, scale, dt_precision;
};

#endif // STRUCTS_H
