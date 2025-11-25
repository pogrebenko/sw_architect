/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef OPTIONS_H
#define OPTIONS_H

#include <qnamespace.h>
#include <QPoint>
#include <QVariantMap>

#include <math.h>

#include "common/Consts.h"
#include "common/Structs.h"

#include "data/general/Class.h"
#include "data/general/Relation.h"

class CIOStreamer;

class CAppOptions : public __Options__
{
    friend void          Write( CIOStreamer &ios, CAppOptions &obj );
    friend CAppOptions&  Read ( CIOStreamer &ios, CAppOptions &obj, long load_version );

    friend QJsonObject& Write( QJsonObject &ios, CAppOptions &obj );
    friend CAppOptions& Read ( QJsonObject &ios, CAppOptions &obj, long load_version );

public:

    inline virtual ~CAppOptions(){}
    inline          CAppOptions(){ init(); }
    inline          CAppOptions( const CAppOptions &o ) { *this = o; }

    CAppOptions& operator =( const CAppOptions &o );

    void init();
    void clearPos();
    void clearState( bool bWithSelect = true );

    CAppOptions& class_relation_copy();
    void         class_relation_paste( CAppOptions &AppClipboard );
    bool         class_relation_delete( long n );

    static FigureType_t   rotateFigureType  ( FigureType_t   );
    static RelationType_t rotateRelationType( RelationType_t );

    inline void            setProjectName( QString ProjectName ) { m_ProjectName = ProjectName; }
    inline QString         getProjectName() { return m_ProjectName; }

    // inline void            setDatabaseName( QString DatabaseName ) { m_DatabaseName = DatabaseName; }
    // inline QString         getDatabaseName() { return m_DatabaseName; }

    inline void            setTablePrefix( QString TablePrefix ) { m_TablePrefix = TablePrefix; }
    inline QString         getTablePrefix() { return m_TablePrefix; }

    inline void            setClassPrefix( QString ClassPrefix ) { m_ClassPrefix = ClassPrefix; }
    inline QString         getClassPrefix() { return m_ClassPrefix; }

    inline void            setPadding   ( int nPadding ) { m_nPadding = nPadding; }
    inline int             getPadding   () { return m_nPadding    ; }
    inline void            setHeight    ( int nHeight ) { m_nHeight = nHeight; }
    inline int             getHeight    () { return m_nHeight    ; }

    inline float           getWidth     () { return m_nWidth     ; }
    inline float           getWidthHover() { return m_nWidthHover; }
    inline QString         getPenColor  () { return m_PenColor   ; }
    inline QString         getPenColorSelect() { return m_PenColorSelect; }
    inline Qt::GlobalColor getPenColorRotate() { return m_PenColorRotate; }
    inline Qt::GlobalColor getPenColorResize() { return m_PenColorResize; }
    inline Qt::GlobalColor getBrushColor() { return m_BrushColor ; }
    inline Qt::GlobalColor getBkgColor  () { return m_BkgColor   ; }
    inline Qt::PenStyle    getPenStyle  () { return m_PenStyle   ; }
    inline Qt::PenStyle    getPenFKStyle() { if( m_PenFKStyle == Qt::NoPen ) m_PenFKStyle = Qt::DashLine; return m_PenFKStyle ; }
    inline Qt::ArrowType   getArrowType () { return m_ArrowType  ; }
    inline Qt::BrushStyle  getBrushStyle() { return m_BrushStyle ; }
    inline Qt::BrushStyle  getBrushTitleStyle() { return m_BrushTitleStyle ; }
    inline QString         getBrushTitleColor() { return m_BrushTitleColor; }
    inline float           getCPRadius  () { return m_nCPRadius  ; }

    inline FigureType_t    getFigureType() { return m_nFigureType; }
    inline void            setFigureType( FigureType_t nFigureType ) { m_nFigureType = nFigureType; }
    inline RelationType_t  getRelationType() { return m_nRelationType; }
    inline void            setRelationType( RelationType_t nRelationType ) { m_nRelationType = nRelationType; }
    inline ActionType_t    getActionType() { return m_nActionType; }
    inline void            setActionType( ActionType_t nActionType ) { m_nActionType = nActionType; }

    inline QPoint&         getFirstPos() { return m_nFirstPos; }
    inline void            setFirstPos( const QPoint &nFirstPos ) { m_nFirstPos = nFirstPos; }
    inline QPoint&         getLastPos() { return m_nLastPos; }
    inline void            setLastPos( const QPoint &nLastPos ) { m_nLastPos = nLastPos; }

    inline ClassList_t  *    getClassList  () { return &m_ClassList  ; }
    inline RelationList_t*   getRelationList() { return &m_RelationList; }

    inline long            getIndexFrom() { return m_nIndexFrom; }
    inline void            setIndexFrom( long nIndexFrom ) { m_nIndexFrom = nIndexFrom; }
    inline int             getArrowAngle() { return m_nArrowAngle; }
    inline void            setArrowAngle( unsigned int nArrowAngle ) { m_nArrowAngle = nArrowAngle; }
    inline float           getArrowSize() { return m_nArrowSize; }
    inline void            setArrowSize( float nArrowSize ) { m_nArrowSize = nArrowSize; }

    // inline DatabaseType_t  getDatabaseType() { return m_nDatabaseType; }
    // inline void            setDatabaseType( DatabaseType_t nDatabaseType ) { m_nDatabaseType = nDatabaseType; }

    inline float           getRatio() { if( m_nRatio < MINIMUM_RATIO || m_nRatio > MAXIMUM_RATIO ) m_nRatio = DEFAULT_RATIO;
                                        return m_nRatio; }
    inline void            setRatio( float nRatio ) { m_nRatio = std::round( nRatio * PRECISION_RATIO ) / PRECISION_RATIO; }

    inline bool            getSelectGroup() { return m_bSelectGroup; }
    inline void            setSelectGroup( bool bSelectGroup ) { m_bSelectGroup = bSelectGroup; }
    inline Qt::PenStyle    getPenSelectStyle() { if( m_PenSelectStyle == Qt::NoPen ) m_PenSelectStyle = Qt::DashLine; return m_PenSelectStyle; }

    inline RelationNotation_t getRelationNotation() { return m_nRelationNotation; }

    inline HDBPROVIDER     getProvider() { return m_hProvider; }
    inline void            setProvider( HDBPROVIDER hProvider ) { m_hProvider = hProvider; }

private:
    ClassList_t     m_ClassList      ;
    RelationList_t  m_RelationList   ;
};

extern void          Write( CIOStreamer &ios, CAppOptions &obj );
extern CAppOptions&  Read ( CIOStreamer &ios, CAppOptions &obj, long version );

extern QJsonObject& Write( QJsonObject &ios, CAppOptions &obj );
extern CAppOptions& Read ( QJsonObject &ios, CAppOptions &obj, long version );

#endif // OPTIONS_H
