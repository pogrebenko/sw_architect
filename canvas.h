/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef CANVAS_H
#define CANVAS_H

#include <QMouseEvent>
#include <QWidget>
#include <QTextEdit>
#include <QMenu>

//#include "common/Class.h"
//#include "common/Relation.h"

#include "engine/Builder.h"
#include "view/Painter.h"

class CCanvas
: public QWidget
, public CPainter
, public CBuilder
{
    Q_OBJECT
public:

    explicit CCanvas( QWidget*, CAppOptions*, Logger_t* );
    ~CCanvas();

    void hideNameEdit();
    void refresh( bool hideEdit = true, bool figureNew = false, bool relationNew = false );

protected:

    QPoint buildPoint( QMouseEvent *pEvent );
    QPoint buildPoint( const QPoint &pos );

    void paintEvent           ( QPaintEvent * ) override;
    void resizeEvent          ( QResizeEvent* ) override;
    void mousePressEvent      ( QMouseEvent * ) override;
    void mouseReleaseEvent    ( QMouseEvent * ) override;
    void mouseMoveEvent       ( QMouseEvent * ) override;
    void mouseDoubleClickEvent( QMouseEvent * ) override;
    void keyPressEvent        ( QKeyEvent   * ) override;
    void keyReleaseEvent      ( QKeyEvent   * ) override;

    bool eventFilter(QObject  *obj, QEvent * event) override;

    void showNameEdit( const QPoint &pt, const QRect &rc );

    void onAddTable        ( const QPoint &pt );
    void onEditTable       ( const QPoint &pt );
    void onAddColumn       ( const QPoint &pt );
    void onEditColumn      ( const QPoint &pt );
    void onDeleteColumn    ( const QPoint &pt );
    void onPrimaryColumn   ( const QPoint &pt, bool checked );
    void onFromFKColumn    ( const QPoint &pt, bool checked );
    void onToFKColumn      ( const QPoint &pt, __Field *fromFK );
    void onEditRelation    ( const QPoint &pt );
    void onDeleteRelation  ( const QPoint &pt );
    void onResetRotate     ( const QPoint &pt );
    void onEditFieldOptions( const QPoint &pt );
    void onEditClassOptions( const QPoint &pt );

    bool  m_bMouseLButtonMoved;
    bool  m_bBreakAddFigure;
    long  m_nDir, m_nX, m_nY;

    using QWidget::QWidget;

// update/draw tiemrs
    QTimer *m_pOscilationTimer;

    QTextEdit *m_pNameEdit;

public slots:
    void oscilationBuilder();
    void onTextChanged    ();
    void onResetRatio     ();
    void onPlusRatio      ();
    void onMinusRatio     ();
    void onClearRotate    ();
    void onClearDocument  ();

    void showContextMenu(const QPoint &pos);

};

#endif // CANVAS_H
