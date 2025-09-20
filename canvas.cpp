/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "canvas.h"

#include <cmath>

#include "data/Options.h"
#include "data/History.h"

#include "ui/FieldDetails.h"
#include "ui/ClassDetails.h"
#include "ui/TableEdit.h"
#include "ui/RelationDetails.h"

#include "mainwindow.h"

#include "templates.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
extern CHistory                     __History;

#define __AppOptions (*__AppOptions__.get())

CCanvas::CCanvas( QWidget *pParent,  CAppOptions *pAppOptions, Logger_t *pLogger )
: QWidget{ pParent }
, CPainter( pAppOptions )
, CBuilder( pAppOptions, pLogger )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    setMouseTracking( true );
    setFocusPolicy( Qt::StrongFocus );

    m_bMouseLButtonMoved = false;
    m_bBreakAddFigure    = false;

    m_nDir = 1; m_nX = 0L; m_nY = 0L;
    m_pOscilationTimer = new QTimer( this );
    connect(m_pOscilationTimer, &QTimer::timeout, this, &CCanvas::oscilationBuilder );

    m_pNameEdit = new CTableEdit( this );
    m_pNameEdit->hide();
    m_pNameEdit->installEventFilter( this );
    m_pNameEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pNameEdit->setVerticalScrollBarPolicy  (Qt::ScrollBarAlwaysOff);
    m_pNameEdit->setLineWrapMode( QTextEdit::NoWrap ); // one line
    connect( m_pNameEdit, &QTextEdit::textChanged, this, &CCanvas::onTextChanged );
    //connect( m_pNameEdit, &QTextEdit::keyPressEvent, this, &CCanvas::onKeyPressEvent );

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, &QWidget::customContextMenuRequested, this, &CCanvas::showContextMenu);

    __AppOptions.setHeight( fontMetrics().height() );
    __AppOptions.setPadding( __AppOptions.getHeight()/HEIGHT_TO_PADDING );
}

void CCanvas::showContextMenu( const QPoint &pos )
{
    QPoint pt( buildPoint( pos ) );

    QMenu contextMenu( this );

    bool toFK = false;
    auto rc = field_hover( pt, false );
    if( rc["field_index"] >= 0 )
    {
        QAction *editFieldOptionsAction = contextMenu.addAction( "edit Options" );
        connect( editFieldOptionsAction, &QAction::triggered, [ this, &pt ](){ onEditFieldOptions( pt );} );

        QAction *editColumnAction = contextMenu.addAction( "edit Field" );
        connect( editColumnAction , &QAction::triggered, [ this, &pt ](){ onEditColumn( pt );} );

        auto pItem = __AppOptions.getClassList()->At( rc["class_index"], rc["field_index"] );

        QAction *pkColumnAction = contextMenu.addAction( "primary" );
                 pkColumnAction->setCheckable( true );
                 pkColumnAction->setChecked( pItem->m_nFieldRelationType == FieldRelationTypePrimaryKey );
        connect( pkColumnAction , &QAction::triggered, [ this, &pt ]( bool checked ){ onPrimaryColumn( pt, checked );} );

        if( pItem->m_nFieldRelationType == FieldRelationTypePrimaryKey )
        {
            __AppOptions.setIndexFrom( rc["class_index"] );

            QAction *FromFKColumnAction = contextMenu.addAction( "delegate [pk]..." );
                     FromFKColumnAction->setCheckable( true );
                     FromFKColumnAction->setChecked( pItem->m_bFromFK );
            connect( FromFKColumnAction , &QAction::triggered, [ this, &pt ]( bool checked ){ onFromFKColumn( pt, checked );} );
        }
        else
        {
            toFK = true;
        }

        QAction *addColumnAction = contextMenu.addAction( "add Field" );
        connect( addColumnAction , &QAction::triggered, [ this, &pt ](){ onAddColumn( pt );} );

        QAction *deleteColumnAction = contextMenu.addAction( "delete Field" );
        connect( deleteColumnAction , &QAction::triggered, [ this, &pt ](){ onDeleteColumn( pt );} );
    }
    else
    if( title_hover( pt, false ) >= 0 )
    {
        QAction *editClassOptionsAction = contextMenu.addAction( "edit Options" );
        connect( editClassOptionsAction, &QAction::triggered, [ this, &pt ](){ onEditClassOptions( pt );} );

        QAction *editTableAction = contextMenu.addAction( "edit Class" );
        connect( editTableAction , &QAction::triggered, [ this, &pt ](){ onEditTable( pt );} );

        QAction *addColumnAction = contextMenu.addAction( "add Field" );
        connect( addColumnAction , &QAction::triggered, [ this, &pt ](){ onAddColumn( pt );} );

        toFK = true;
    }
    else
    if( figure_hover( pt, false ) >= 0 )
    {
        QAction *editTableAction = contextMenu.addAction( "edit Class" );
        connect( editTableAction , &QAction::triggered, [ this, &pt ](){ onEditTable( pt );} );

        QAction *addColumnAction = contextMenu.addAction( "add Field" );
        connect( addColumnAction , &QAction::triggered, [ this, &pt ](){ onAddColumn( pt );} );

        toFK = true;
    }
    else
    if( relation_hover( pt, true ) >= 0 )
    {
        QAction *editRelationAction = contextMenu.addAction( "edit Relation" );
        connect( editRelationAction , &QAction::triggered, [ this, &pt ](){ onEditRelation( pt );} );

        QAction *deleteRelationAction = contextMenu.addAction( "delete Relation" );
        connect( deleteRelationAction , &QAction::triggered, [ this, &pt ](){ onDeleteRelation( pt );} );
    }
    else
    if( rotate_hover( pt, true, false ) >= 0 )
    {
        QAction *addColumnAction = contextMenu.addAction( "reset Rotate" );
        connect( addColumnAction , &QAction::triggered, [ this, &pt ](){ onResetRotate( pt );} );
    }
    else
    if( resize_hover( pt, true, false ) >= 0 )
    {
    }
    else
    {
        QAction *addTableAction = contextMenu.addAction( "add Class" );
        connect( addTableAction, &QAction::triggered, [ this, &pt ](){ onAddTable( pt );} );
        if( __AppOptions.getRatio() != DEFAULT_RATIO )
        {
            QAction *resetRatioAction = contextMenu.addAction( "reset Ratio" );
            connect( resetRatioAction, &QAction::triggered, [ this ](){ onResetRatio(); } );
        }
        QAction *addColumnAction = contextMenu.addAction( "clear Rotate" );
        connect( addColumnAction , &QAction::triggered, [ this ](){ onClearRotate();} );
    }

    if( toFK )
    {
        auto rc0 = __AppOptions.getClassList()->find_fromFK();
        if( rc["class_index"] != rc0["class_index"] && rc0["class_index"] >=0 && rc0["field_index"] >= 0 )
        {
            auto pItem = __AppOptions.getClassList()->At( rc0["class_index"], rc0["field_index"] );
            QAction *ToFKColumnAction = contextMenu.addAction( "accept [fk]" );
            connect( ToFKColumnAction , &QAction::triggered, [ this, &pt, pItem ](){ onToFKColumn( pt, pItem );} );
        }
    }

    contextMenu.exec( mapToGlobal( pos ) );
}

void CCanvas::onAddTable( const QPoint &pt )
{
    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, __AppOptions.getClassList()->size() );
    //figure_begin_add( pt );
    __AppOptions.setFirstPos( pt );
    figure_set_last_pos( pt + QPoint( ADD_DEFAULT_WIDTH, ADD_DEFAULT_HEIGHT ) );
    figure_release_add();
}

void CCanvas::onEditTable( const QPoint &pt )
{
    int n = -1;
    if( (n = figure_hover( pt, false )) >= 0 )
    {
        auto  cl = __AppOptions.getClassList();
        auto &pItem = cl->at( n );
              pItem->m_bEditText = true;

        QRect rc( pItem->m_nFirstPos, pItem->m_nTitlePos );

        //getLogger()->info( APP_LOG_LEVEL, "info: onEditTable:%d", n );

        m_pNameEdit->/*setText*/setPlainText( pItem->m_LogicalName.c_str() );
        showNameEdit( pt, rc );
    }
}

void CCanvas::onAddColumn( const QPoint &pt )
{
    int n = -1;
    if( (n = figure_hover( pt, false )) >= 0 )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, n, __AppOptions.getClassList()->at(n)->m_FieldList.size() );
        field_add( n, "..." ); // how to add predefined pk or name???
        ((QMainWindow*)QApplication::activeWindow())->statusBar()->showMessage( "New field added", DEFAULT_MSG_TIMEOUT );
    }
}

void CCanvas::onEditColumn( const QPoint &pt )
{
    auto rc = field_hover( pt, false );
    if( rc["class_index"] >= 0 && rc["field_index"] >= 0 )
    {
        //CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, rc["class_index"], rc["field_index"], true, false );

        //getLogger()->info( APP_LOG_LEVEL, "info: class_index:%d field_index:%d", rc["class_index"], rc["field_index"] );
        auto pItem = __AppOptions.getClassList()->At( rc["class_index"], rc["field_index"] );
             pItem->m_bEditText = true;

        QRect rc( pItem->m_nFirstPos, pItem->m_nLastPos );

        m_pNameEdit->setPlainText( pItem->m_LogicalName.c_str() );
        showNameEdit( pt, rc );
    }
}

void CCanvas::onEditClassOptions( const QPoint &pt )
{
    long n = figure_hover( pt, false );
    if( n >= 0 )
    {
        __Class *pItem = __AppOptions.getClassList()->at( n ).get();

        ClassDetails dialog( "Options", pItem, this );
        if( dialog.exec() == QDialog::Accepted )
        {
            CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, n );
            dialog.getData();
            //m_pStatusBar->showMessage( "options edited" );
        }
    }
}

void CCanvas::onEditFieldOptions( const QPoint &pt )
{
    auto rc = field_hover( pt, false );
    if( rc["class_index"] >= 0 && rc["field_index"] >= 0 )
    {
        __Field *pItem = __AppOptions.getClassList()->At( rc["class_index"], rc["field_index"] );

        FieldDetails dialog( "Options", pItem, this );
        if( dialog.exec() == QDialog::Accepted )
        {
            CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, rc["class_index"], rc["field_index"] );
            dialog.getData();
            //m_pStatusBar->showMessage( "options edited" );
        }
    }
}

void CCanvas::onFromFKColumn ( const QPoint &pt, bool checked )
{
    auto rc = field_hover( pt, false );
    if( rc["class_index"] >= 0 && rc["field_index"] >= 0 )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, rc["class_index"], rc["field_index"] );
        __AppOptions.setIndexFrom( rc["class_index"] );
        auto pItem = __AppOptions.getClassList()->At( rc["class_index"], rc["field_index"] );
             pItem->m_bFromFK = checked;
    }
}

void CCanvas::onToFKColumn ( const QPoint &pt, __Field *fromFK )
{
    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, __AppOptions.getIndexFrom(), relation_release_add( pt, fromFK ) );
    ((QMainWindow*)QApplication::activeWindow())->statusBar()->showMessage( "New foreign key added", DEFAULT_MSG_TIMEOUT );
}

void CCanvas::onPrimaryColumn( const QPoint &pt, bool checked )
{
    auto rc = field_hover( pt, false );
    if( rc["class_index"] >= 0 && rc["field_index"] >= 0 )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, rc["class_index"], rc["field_index"] );
        //getLogger()->info( APP_LOG_LEVEL, "info: class_index:%d field_index:%d", rc["class_index"], rc["field_index"] );
        auto pItem = __AppOptions.getClassList()->At( rc["class_index"], rc["field_index"] );
             pItem->m_nFieldRelationType =  checked ? FieldRelationTypePrimaryKey : FieldRelationTypeNone;
             pItem->m_nFieldDataType     =  checked ? FieldDataTypeInteger        : FieldDataTypeNone    ;
             pItem->m_bAutoIncrement     =  checked;
             pItem->m_bAllowsNulls       = !checked;

        figure_rebuild( rc["class_index"] );
        ((QMainWindow*)QApplication::activeWindow())->statusBar()->showMessage( "Primary key property added", DEFAULT_MSG_TIMEOUT );
    }
}

void CCanvas::onDeleteColumn( const QPoint &pt )
{
    auto rc = field_hover( pt, false );
    if( field_delete( rc ) )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, rc["class_index"], rc["field_index"] );
        ((QMainWindow*)QApplication::activeWindow())->statusBar()->showMessage( "Field deleted", DEFAULT_MSG_TIMEOUT );
    }
}

void CCanvas::onDeleteRelation( const QPoint &pt )
{
    long n = -1;
    if( relation_delete( (n = relation_hover( pt, false )) ) )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, n );
        ((QMainWindow*)QApplication::activeWindow())->statusBar()->showMessage( "Relation deleted", DEFAULT_MSG_TIMEOUT );
    }
}

void CCanvas::onEditRelation( const QPoint &pt )
{
    long n = relation_hover( pt, false );
    if( n >= 0 )
    {
        __Relation *pItem = __AppOptions.getRelationList()->at( n ).get();

        CRelationDetails dialog( "Relationship properties", pItem, this );
        if( dialog.exec() == QDialog::Accepted )
        {
            CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, n );
            dialog.getData();
            //((QMainWindow*)QApplication::activeWindow())->statusBar()->showMessage( "Relation edited", DEFAULT_MSG_TIMEOUT );
        }
    }
}

bool CCanvas::eventFilter( QObject  *obj, QEvent * event )
{
    if((QTextEdit *)obj == m_pNameEdit && event->type()==QEvent::KeyPress && ((QKeyEvent*)event)->key() == Qt::Key_Escape )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
    }

    return false;
}

void CCanvas::onTextChanged()
{
    __AppOptions.getClassList()->edit_title( m_pNameEdit->toPlainText().toStdString() );
}

CCanvas::~CCanvas()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    delete m_pOscilationTimer;
    delete m_pNameEdit;
}

void
CCanvas::oscilationBuilder()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    //getLogger()->info( APP_LOG_LEVEL, "info: tick updateBuilder" );

    QPoint globalMousePos{ QCursor::pos() };
    QPoint pt = this->mapFromGlobal(globalMousePos);
    int x = m_nDir * ( m_nX % 4 ), y = m_nDir * ( m_nY % 4 );
    if(    figure_oscillation  ( pt, x, y ) >= 0
        || relation_oscillation( pt, x, y ) >= 0 )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt );
        if( x == 0 )
        {
            m_nDir *= -1;
        }
        ++ m_nX; ++ m_nY;
    }
}

void CCanvas::onResetRotate( const QPoint &pt )
{   // for one class
    long n = rotate_hover( pt, false, false );
    if( n >= 0 )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, n );
        int nHeight = __AppOptions.getHeight() + 2 * __AppOptions.getPadding();
        auto &pItem = __AppOptions.getClassList()->at( n );
              pItem->m_nAngle = 0;
              pItem->calc_angle_resize_point( nHeight );
    }
}

void CCanvas::onClearRotate()
{   // for all classes
    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
    int nHeight = __AppOptions.getHeight() + 2 * __AppOptions.getPadding();
    auto CL = __AppOptions.getClassList();
    std::for_each( CL->begin(), CL->end(),
        [nHeight](auto &pItem){
          pItem->m_nAngle = 0;
          pItem->calc_angle_resize_point( nHeight );
        }
    );
}

void CCanvas::onClearDocument()
{
    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
    __AppOptions.init();
    __AppOptions.setHeight( fontMetrics().height() );
    __AppOptions.setPadding( __AppOptions.getHeight()/HEIGHT_TO_PADDING );
}

void CCanvas::onResetRatio()
{
    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
    __AppOptions.setRatio( DEFAULT_RATIO );
    QSize size; setMinimumSize( figure_size( size, parentWidget()->size() ) );
}

void CCanvas::onPlusRatio()
{
    float r  = __AppOptions.getRatio();
          r += DEFAULT_RATIO_STEP;
    if( r < MAXIMUM_RATIO )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
        __AppOptions.setRatio( r );
        QSize size; setMinimumSize( figure_size( size, parentWidget()->size() ) * r );
    }
}

void CCanvas::onMinusRatio()
{
    float r  = __AppOptions.getRatio();
          r -= DEFAULT_RATIO_STEP;
    if( r > MINIMUM_RATIO )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
        __AppOptions.setRatio( r );
        QSize size; setMinimumSize( figure_size( size, parentWidget()->size() ) * r );
    }
}

void
CCanvas::paintEvent( QPaintEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        QPainter painter{ this };
        painter_draw( painter );
        //__Logger.info( APP_LOG_LEVEL, "info: paintEvent" );
        QWidget::paintEvent( pEvent );
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::resizeEvent( QResizeEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        painter_resize( this->rect() );
        //getLogger()->info( APP_LOG_LEVEL, "info: resizeEvent" );
        QWidget::resizeEvent( pEvent );
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::keyPressEvent( QKeyEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        auto key = pEvent->key();
        if( key == Qt::Key_Escape )
        {
            //getLogger()->info( APP_LOG_LEVEL, "info: Key down ESC" );
            m_bBreakAddFigure = true;
            __AppOptions.setIndexFrom( -1 );
        }
        QWidget::keyPressEvent(pEvent);
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::keyReleaseEvent( QKeyEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        auto key = pEvent->key();
        if( key == Qt::Key_Escape )
        {
            CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History );
            //getLogger()->info( APP_LOG_LEVEL, "info: Key up ESC" );
            m_bBreakAddFigure = false;
            __AppOptions.setIndexFrom( -1 );
            __AppOptions.setSelectGroup( false );
            select_reset();
        }
        else
        {
            bool isPlusKey         = ( pEvent->key() == Qt::Key_Plus  || pEvent->key() == Qt::Key_Equal || pEvent->key() == Qt::Key_Up );
            bool isMinusKey        = ( pEvent->key() == Qt::Key_Minus || pEvent->key() == Qt::Key_Down );
            bool isControlModifier = pEvent->modifiers().testFlag( Qt::ControlModifier ); // Qt::ControlModifier | Qt::KeypadModifier
            if( isControlModifier )
            {
                if( isPlusKey )
                {
                    onPlusRatio();
                }
                else
                if( isMinusKey )
                {
                    onMinusRatio();
                }
            }
        }
        QWidget::keyReleaseEvent(pEvent);
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::mousePressEvent( QMouseEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        QPoint pt( buildPoint( pEvent ) );

        m_bMouseLButtonMoved = false;
        if( pEvent->button() == Qt::LeftButton )
        {
            bool isControlModifier = pEvent->modifiers().testFlag( Qt::ControlModifier );
            if( isControlModifier )
            {
                long n = -1;
                if( figure_select( (n = figure_hover( pt, false )) ) )
                {
                    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, n );
                }
                else
                {
                    __AppOptions.setSelectGroup( false );
                    select_reset();

                    __AppOptions.setSelectGroup( true );
                    figure_set_first_pos( -1, pt );
                }
            }
            else
            {
                long n = 0;

                __AppOptions.setSelectGroup( false );

                // reset selection, used only for move selected items
                switch( get_action_type() )
                {
                    case ActionTypeMove        : break;

                    case ActionTypeAddFigure   :
                    case ActionTypeAddRelation :
                    case ActionTypeDelete      :
                    case ActionTypeNone        : select_reset(); break;
                }

                switch( get_action_type() )
                {
                    case ActionTypeMove        : if( (n = figure_hover( pt, false        )) >= 0
                                                  || (n = resize_hover( pt, false, false )) >= 0
                                                  || (n = rotate_hover( pt, false, false )) >= 0 )
                                                 { figure_set_first_pos( n, pt ); }
                                                 else
                                                 { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt ); select_reset(); }  break;

                    case ActionTypeAddFigure   : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, -1, figure_hover(pt,false)<0&&figure_begin_add( pt ) ); } break;
                    case ActionTypeAddRelation : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, -1, !relation_begin_add( figure_hover( pt, false ) ) && relation_release_add( pt, nullptr ) ); } break;
                    case ActionTypeDelete      : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, -1, relation_delete( relation_hover( pt, false ) ) || figure_delete( figure_hover( pt, false ) ) ); } break;

                    case ActionTypeNone        : break;
                }
            }
        }
        // else
        // if( pEvent->button() == Qt::RightButton )
        // {
        //     relation_begin_add( figure_hover( pt, false ) );
        // }
        QWidget::mousePressEvent(pEvent);
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::mouseReleaseEvent( QMouseEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        QPoint pt( buildPoint( pEvent ) );
        if( pEvent->button() == Qt::LeftButton )
        {
            bool isControlModifier = pEvent->modifiers().testFlag( Qt::ControlModifier );
            if( isControlModifier )
            {
                if( m_bMouseLButtonMoved )
                {
                    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt );
                    __AppOptions.setSelectGroup( false );
                }
            }
            else
            {
                if( !m_bBreakAddFigure )
                {
                    if( m_bMouseLButtonMoved )
                    {
                        switch( get_action_type() )
                        {
                            case ActionTypeMove        : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &__AppOptions.getFirstPos(), __AppOptions.getIndexFrom(), /*center_hover( pt, true ) >= 0 ||*/
                                                                 rotate_hover( pt, true, true ) >= 0
                                                              || resize_hover( pt, true, true ) >= 0
                                                              || figure_move( pt )
                                                        ); } break;

                            case ActionTypeAddFigure   : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, figure_hover(pt,false)<0&&figure_release_add() ); } break;
                            case ActionTypeAddRelation : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, relation_release_add( pt, nullptr ) ); } break;

                            case ActionTypeDelete      :
                            case ActionTypeNone        : break;
                        }
                    }
                }
            }
        }
        // else
        // if( pEvent->button() == Qt::RightButton )
        // {
        //     if( relation_release_add( pt ) )
        //     {
        //         refresh();
        //     }
        // }

        QWidget::mouseReleaseEvent( pEvent );
        __AppOptions.setIndexFrom( -1 );
        m_bMouseLButtonMoved = false;
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::mouseMoveEvent( QMouseEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        QPoint pt( buildPoint( pEvent ) );

        hover_reset();

        auto buttons = pEvent->buttons();
        if( buttons == Qt::LeftButton )
        {
            m_bMouseLButtonMoved = true;

            bool isControlModifier = pEvent->modifiers().testFlag( Qt::ControlModifier );
            if( isControlModifier )
            {
                if( __AppOptions.getSelectGroup() )
                {
                    CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1,
                        figure_set_last_pos( pt )
                        && figure_select( QRect( __AppOptions.getFirstPos(), __AppOptions.getLastPos() ) )
                    );
                }
            }
            else
            {
                __AppOptions.setSelectGroup( false );

                switch( get_action_type() )
                {
                    case ActionTypeMove        : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &__AppOptions.getFirstPos(), __AppOptions.getIndexFrom(),/*center_hover( pt, true ) >= 0 ||*/
                                                           rotate_hover( pt, true, true ) >= 0
                                                        || resize_hover( pt, true, true ) >= 0
                                                        || figure_move( pt ) || relation_move( pt )
                                                ); } break;

                    case ActionTypeAddFigure   : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, figure_hover(pt,false)<0&&figure_set_last_pos(pt), true, false, true , false ); } break;
                    case ActionTypeAddRelation : { CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, relation_set_last_pos( pt ), true, false, false, true  ); } break;

                    case ActionTypeDelete      :
                    case ActionTypeNone        : break;
                }

                // Scroll Area Calculate
                QSize size; setMinimumSize( figure_size( size, parentWidget()->size() ) * __AppOptions.getRatio() );
            }
        }
        // else
        // if( buttons == Qt::RightButton )
        // {
        //     if( relation_set_last_pos( pt ) )
        //     {
        //         refresh();
        //     }
        // }
        else
        {
            __AppOptions.setSelectGroup( false );

            long fh = -1, rh = -1;
            this->setCursor( Qt::ArrowCursor );

            if( field_hover( pt, true )["field_index"] >= 0 )
                this->setCursor( Qt::IBeamCursor );
            else
            if( title_hover( pt, true ) >= 0 )
                this->setCursor( Qt::IBeamCursor );
            // else
            // if( center_hover( pt, true ) >= 0 )
            //     this->setCursor( Qt::IBeamCursor );
            else
            if( rotate_hover( pt, true, false ) >= 0 && check_action_type( ActionTypeMove ) )
                this->setCursor( Qt::PointingHandCursor );
            else
            if( resize_hover( pt, true, false ) >= 0 && check_action_type( ActionTypeMove ) )
                this->setCursor( Qt::SizeFDiagCursor );
            //else
            if( (fh = figure_hover( pt, true )) >= 0 && check_action_type( ActionTypeMove ) )
                this->setCursor( Qt::PointingHandCursor );

            rh = relation_hover( pt, true );

            CRefreshCanvas v( __PRETTY_FUNCTION__, this, &__History, &pt, -1, -1, true, false );

            if( false && check_action_type( ActionTypeDelete ) )
            {
                if( fh >= 0 || rh >= 0 )
                {
                    if( !m_pOscilationTimer->isActive() )
                    {
                        m_nDir = 1; m_nX = 0L; m_nY = 0L;
                        m_pOscilationTimer->start(1000/20);
                    }
                }
                else
                {
                    if( m_pOscilationTimer->isActive() )
                    {
                        m_pOscilationTimer->stop();
                        relation_rebuild();
                    }
                }
            }
        }

        QWidget::mouseMoveEvent( pEvent );
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::mouseDoubleClickEvent( QMouseEvent *pEvent )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    try
    {
        QPoint pt( buildPoint( pEvent ) );
        if( pEvent->button() == Qt::LeftButton )
        {
            long n = -1;
            if( /*(n = center_hover( pt, false )) >= 0 ||*/ ( n = title_hover( pt, false ) ) >= 0 )
            {
                onEditTable( pt );
            }
            else
            {
                auto rc = field_hover( pt, false );
                if( rc["class_index"] >= 0 && rc["field_index"] >= 0 )
                {
                    onEditColumn( pt );
                }
                // else
                // if( relation_rotate_type( relation_hover( pt, false ) ) || figure_rotate_type( figure_hover( pt, false ) ) )
                // {
                //     refresh();
                // }
            }
        }
        // else
        // if( pEvent->button() == Qt::RightButton )
        // {
        //     if( relation_delete( relation_hover( pt, false ) ) || figure_delete( figure_hover( pt, false ) ) )
        //     {
        //         refresh();
        //     }
        // }
        QWidget::mouseDoubleClickEvent( pEvent );
    }
    catch( ... )
    {
        getLogger()->error( APP_LOG_LEVEL, "error: unknown exception" );
    }
}

void
CCanvas::hideNameEdit()
{
    auto found = find_edit();
    if( found["class_index"] >= 0/* || found["field_index"] >= 0*/ )
    {
        QString str = "[pk] "; str += m_pNameEdit->toPlainText();
        auto fm = this->fontMetrics();
        int  w1 = fm.horizontalAdvance( str ) + 4*HEIGHT_TO_PADDING;

        auto pClass = __AppOptions.getClassList()->at( found["class_index"] );
        int w0 = abs( pClass->m_nLastPos.x() - pClass->m_nFirstPos.x() );
        if( w0 < w1 )
        {
            QPoint pt = pClass->m_nResizePos + QPoint( w1 - w0, 0 );
            resize_item( pt, pClass.get() );
        }
    }

    edit_reset();
    m_pNameEdit->hide();
}

void
CCanvas::showNameEdit( const QPoint &, const QRect &rc )
{
    // auto fm = this->fontMetrics();
    // int  w = fm.horizontalAdvance( "here we enter the name" );
    // int  h = fm.height() * 1.7;
    // int  x = pt.x();
    // int  y = pt.y();
    // m_pNameEdit->setGeometry( x - w/2, y - h/2, w, h );

    float r = __AppOptions.getRatio();
    QRect rc0( rc.topLeft() * r, rc.bottomRight() * r );
    rc0.adjust( -1, -1, -1, 0 );
    m_pNameEdit->setGeometry( rc0 );
    m_pNameEdit->show();
    m_pNameEdit->setFocus();
    m_pNameEdit->moveCursor( QTextCursor::End );
    m_pNameEdit->selectAll();
}

QPoint
CCanvas::buildPoint( QMouseEvent *pEvent )
{
    return buildPoint( pEvent->pos() );
}

QPoint
CCanvas::buildPoint( const QPoint &pos )
{
    return QPoint( pos / __AppOptions.getRatio() );
}

void CCanvas::refresh( bool hideEdit, bool figureNew, bool relationNew )
{
    if( hideEdit )
    {
        hideNameEdit();
    }

    if( figureNew )
    {
        draw_figure_new();
    }
    else
    if( relationNew )
    {
        draw_relation_new();
    }
    else
    {
        draw_figure_relation();
    }

    update();
}
