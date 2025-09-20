/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Builder.h"

#include <QtMath>
#include <QRectF>
#include <QLineF>

#include "data/shapes/FigureFactory.h"

#include "data/Options.h"

CBuilder::CBuilder( CAppOptions * const pAppOptions, Logger_t * const pLogger )
: CLoggerHolder( pLogger )
, m_pAppOptions( pAppOptions )
{
}

bool
CBuilder::align_horizontaly_left()
{
    int nMinLeft = INT32_MAX;
    auto CL = m_pAppOptions->getClassList();
    for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
        [&nMinLeft]( auto pItem ) {
            if( pItem->m_bSelect )
            {
                if( nMinLeft > pItem->m_nFirstPos.x() )
                {
                    nMinLeft = pItem->m_nFirstPos.x();
                }
            }
        }
    );
    if( nMinLeft < INT32_MAX )
    {
        for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
            [nMinLeft]( auto pItem ) {
                if( pItem->m_bSelect )
                {
                    QPoint nLastPos( nMinLeft, pItem->m_nFirstPos.y() );
                    pItem->move( pItem->m_nFirstPos, nLastPos );
                }
            }
        );
        m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );
        return true;
    }
    return false;
}

bool
CBuilder::align_horizontaly_right()
{
    int nMaxRight = -1;
    auto CL = m_pAppOptions->getClassList();
    for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
        [&nMaxRight]( auto pItem ) {
            if( pItem->m_bSelect )
            {
                if( nMaxRight < pItem->m_nLastPos.x() )
                {
                    nMaxRight = pItem->m_nLastPos.x();
                    QPoint nLastPos( nMaxRight, pItem->m_nFirstPos.y() );
                    pItem->move( pItem->m_nFirstPos, nLastPos );
                }
            }
        }
    );
    if( nMaxRight >= 0 )
    {
        for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
            [nMaxRight]( auto pItem ) {
                if( pItem->m_bSelect )
                {
                    int width = pItem->m_nLastPos.x() - pItem->m_nFirstPos.x();
                    QPoint nLastPos( nMaxRight - width, pItem->m_nFirstPos.y() );
                    pItem->move( pItem->m_nFirstPos, nLastPos );
                }
            }
        );
        m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );
        return true;
    }
    return false;
}

bool
CBuilder::align_vertically_up()
{
    int nMinUp = INT32_MAX;
    auto CL = m_pAppOptions->getClassList();
    for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
        [&nMinUp]( auto pItem ) {
            if( pItem->m_bSelect )
            {
                if( nMinUp > pItem->m_nFirstPos.y() )
                {
                    nMinUp = pItem->m_nFirstPos.y();
                }
            }
        }
    );
    if( nMinUp < INT32_MAX )
    {
        for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
            [nMinUp]( auto pItem ) {
                if( pItem->m_bSelect )
                {
                    QPoint nLastPos( pItem->m_nFirstPos.x(), nMinUp );
                    pItem->move( pItem->m_nFirstPos, nLastPos );
                }
            }
        );
        m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );
        return true;
    }
    return false;
}

bool
CBuilder::align_vertically_down()
{
    int nMaxDown = -1;
    auto CL = m_pAppOptions->getClassList();
    for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
             [&nMaxDown]( auto pItem ) {
                 if( pItem->m_bSelect )
                 {
                     if( nMaxDown < pItem->m_nLastPos.y() )
                     {
                         nMaxDown = pItem->m_nLastPos.y();
                     }
                 }
             }
             );
    if( nMaxDown >= 0 )
    {
        for_each( __EXECUTION_POLICY_BUILDER__, CL->begin(), CL->end(),
            [nMaxDown]( auto pItem ) {
                if( pItem->m_bSelect )
                {
                    int height = pItem->m_nLastPos.y() - pItem->m_nFirstPos.y();
                    QPoint nLastPos( pItem->m_nFirstPos.x(), nMaxDown - height );
                    pItem->move( pItem->m_nFirstPos, nLastPos );
                }
            }
        );
        m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );
        return true;
    }
    return false;
}

ActionType_t
CBuilder::get_action_type()
{
    return m_pAppOptions->getActionType();
}

bool
CBuilder::check_action_type( ActionType_t ActionType )
{
    return ActionType == m_pAppOptions->getActionType();
}

void
CBuilder::hover_reset()
{
    m_pAppOptions->getClassList   ()->hover_clear();
    m_pAppOptions->getRelationList()->hover_clear();
}

void
CBuilder::edit_reset()
{
    m_pAppOptions->getClassList()->edit_clear();
}

void
CBuilder::select_reset()
{
    auto CL = m_pAppOptions->getClassList();
    CL->select_clear();
}

long
CBuilder::figure_hover( const QPoint &pos, bool set_up_hover )
{
    long n = m_pAppOptions->getClassList()->hover_index( pos );
    if( n >= 0 && set_up_hover )
    {
        auto &pItem = m_pAppOptions->getClassList()->at( n );
        //getLogger()->info( APP_LOG_LEVEL, "info: Figure hover[%d]", n );

        pItem->m_bHover       =
        pItem->m_bHoverFirst  =
        pItem->m_bHoverLast   =
        pItem->m_bHoverAngle  =
        pItem->m_bHoverResize = true;
    }
    return n;
}

bool
CBuilder::figure_move( const QPoint &pt )
{
    long n0 = m_pAppOptions->getIndexFrom();
    if( n0 >= 0 )
    {
        auto CL = m_pAppOptions->getClassList();
        long n = CL->hover_index( pt );
        if( n0 == n && n >= 0 )
        {
            //getLogger()->info( APP_LOG_LEVEL, "info: Move figure [%d]", n );

            auto &pItem0 = CL->at( n );

            pItem0->m_bHover = true;

            m_pAppOptions->setLastPos( pt );
            QPoint diff = pItem0->m_nFirstPos + m_pAppOptions->getLastPos() - m_pAppOptions->getFirstPos();
            if( diff.x() < 0 || diff.y() < 0 )
            {
                auto ignore = pItem0.get();
                for_each( CL->begin(), CL->end(),
                         [this,ignore]( auto pItem ) {
                             if( pItem.get() != ignore )
                                 pItem->move( m_pAppOptions->getLastPos(), m_pAppOptions->getFirstPos() );
                         } );
            }
            else
            {
                if( pItem0->m_bSelect )
                {
                    for_each( CL->begin(), CL->end(),
                             [this]( auto pItem ) {
                                 if( pItem->m_bSelect )
                                     pItem->move( m_pAppOptions->getFirstPos(), m_pAppOptions->getLastPos() );
                             } );
                }
                else
                {
                    pItem0->move( m_pAppOptions->getFirstPos(), m_pAppOptions->getLastPos() );
                }
            }
            m_pAppOptions->setFirstPos( pt );

            m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );
            return true;
        }
    }
    return false;
}

__Field*
CBuilder::field_add( long n, const char *str )
{
    auto cl = m_pAppOptions->getClassList();
    auto c  = cl->at( n );
    int  h0 = 0;

    QPoint nFirstPos, nLastPos;
    if( c->m_FieldList.size() == 0 )
    {
        h0 = c->m_nTitlePos.y() - c->m_nFirstPos.y();
        nFirstPos.setX( c->m_nFirstPos.x() );
        nFirstPos.setY( c->m_nTitlePos.y() );

        nLastPos .setX( c->m_nTitlePos.x() );
        nLastPos .setY( c->m_nTitlePos.y() + h0 );
    }
    else
    {
        auto &Last = c->m_FieldList.back();

        h0 = Last->m_nLastPos.y() - Last->m_nFirstPos.y();

        nFirstPos.setX( Last->m_nFirstPos.x() );
        nFirstPos.setY( Last->m_nLastPos .y() );

        nLastPos .setX( Last->m_nLastPos .x() );
        nLastPos .setY( Last->m_nLastPos .y() + h0 );
    }

    __Field *pItem = CFigureFactory::buildField( nFirstPos, nLastPos );
             pItem->m_LogicalName = pItem->m_PhysicalName = str;

    c->m_FieldList.Add( pItem );

    int h = nLastPos.y() - c->m_nLastPos.y();
    if( h >= 0 )
    {
        QPoint pt = c->m_nResizePos + QPoint( 0, h0 + h );
        resize_item( pt, c.get() );
    }

    return pItem;
}

void
CBuilder::figure_set_first_pos( long n, const QPoint &pt )
{
    getLogger()->info( APP_LOG_LEVEL, "info: Press left button for move figure %d:%d", pt.x(), pt.y() );
    m_pAppOptions->setIndexFrom( n );
    m_pAppOptions->setFirstPos( pt );
}

bool
CBuilder::figure_set_last_pos( const QPoint &pt )
{
    m_pAppOptions->setLastPos( pt );
    //getLogger()->info( APP_LOG_LEVEL, "info: Show new figure" );
    return true;
}

bool
CBuilder::figure_begin_add( const QPoint &pos )
{
    getLogger()->info( APP_LOG_LEVEL, "info: Press left button for add figure %d:%d", pos.x(), pos.y() );
    m_pAppOptions->setActionType( ActionTypeAddFigure );
    m_pAppOptions->setFirstPos( pos );
    return true;
}

bool
CBuilder::figure_release_add()
{
    getLogger()->info( APP_LOG_LEVEL, "info: Release left button for add figure" );

    int nHeight = m_pAppOptions->getHeight() + 2 * m_pAppOptions->getPadding();

    auto pFigure = CFigureFactory::buildFigure(
          m_pAppOptions->getFigureType()
        , m_pAppOptions->getFirstPos  ()
        , m_pAppOptions->getLastPos   ()
        , nHeight
        );

    if( pFigure->valid() )
    {
        m_pAppOptions->getClassList()->Add( pFigure );
    }
    else
    {
        delete pFigure;
    }

    return true;
}

bool
CBuilder::figure_rotate_type( long n )
{
    if( n >= 0 )
    {
        if( n < (long)m_pAppOptions->getClassList()->size() )
        {
            getLogger()->info( APP_LOG_LEVEL, "info: Change figure type [%d]", n );
            auto &pFigureItem = m_pAppOptions->getClassList()->at( n );
            pFigureItem->m_nFigureType = CAppOptions::rotateFigureType( pFigureItem->m_nFigureType );

            switch( pFigureItem->m_nFigureType )
            {
                case FigureTypeNone      : break;
                case FigureTypeTriangle  : ++ pFigureItem->m_nAnglesCount; break;
                case FigureTypeEllipse   : pFigureItem->m_nAnglesCount = 0; break;
                case FigureTypeRectangle : pFigureItem->m_nAnglesCount = 4; break;
            }

            m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );
            return true;
        }
    }
    return false;
}

bool
CBuilder::figure_select( const QRect &rc )
{
    auto f = m_pAppOptions->getClassList();
    std::for_each( f->begin(), f->end(),
        [rc]( auto pItem ){ pItem->m_bSelect = QRect( pItem->m_nFirstPos, pItem->m_nLastPos ).QRect::intersects( rc ) ;
    } );
    return true;
}

bool
CBuilder::figure_select( long n )
{
    if( n >= 0 )
    {
        auto f = m_pAppOptions->getClassList();
        if( n < (long)f->size() )
        {
            auto pItem = f->at( n );
            pItem->m_bSelect = !pItem->m_bSelect;
            return true;
        }
    }
    return false;
}

bool
CBuilder::figure_delete( long n )
{
    bool rc = m_pAppOptions->class_relation_delete( n );
    if( rc )
    {
        getLogger()->info( APP_LOG_LEVEL, "info: Delete figure with relations [%d]", n );
    }
    return rc;
}

long
CBuilder::title_hover( const QPoint &pos, bool set_up_hover )
{
    long n = m_pAppOptions->getClassList()->hover_title( pos );
    if( n >= 0 && set_up_hover )
    {
        //getLogger()->info( APP_LOG_LEVEL, "info: title_hover [%d]", n );
        auto &pItem = m_pAppOptions->getClassList()->at( n );
        if( pItem->m_nFigureType == FigureTypeRectangle )
        {
            pItem->m_bHoverTitle = true;
        }
        else
        {
            return -1;
        }
    }
    return n;
}

std::map<std::string,long>
CBuilder::field_hover( const QPoint &pos, bool set_up_hover )
{
    std::map<std::string,long> rc = {{"class_index",-1},{"field_index",-1}};

    rc["class_index"] = m_pAppOptions->getClassList()->hover_index( pos );
    if( rc["class_index"] >= 0 )
    {
        __Class *pClass = m_pAppOptions->getClassList()->at( rc["class_index"] ).get();
        rc["field_index"] = pClass->m_FieldList.hover_title( pos, pClass );
        if( rc["field_index"] >= 0 && set_up_hover )
        {
            auto &pField = pClass->m_FieldList[ rc["field_index"] ];
            pField->m_bHover = true;
        }
    }
    return rc;
}

bool
CBuilder::field_delete( std::map<std::string,long> &rc )
{
    if( rc["class_index"] >= 0 && rc["field_index"] >= 0 )
    {
        auto RL    = m_pAppOptions->getRelationList();
        auto pItem = m_pAppOptions->getClassList()->At( rc["class_index"], rc["field_index"] );
        if( pItem->m_nFieldRelationType == FieldRelationTypePrimaryKey )
        {
            for( long n = RL->size() - 1; n >= 0; n -- )
            {
                auto &pRelation = RL->at( n );
                if( rc["class_index"] == pRelation->m_nFrom )
                {
                    relation_delete_fields( pRelation.get() );
                }
            }
            m_pAppOptions->getClassList()->Delete( rc["class_index"], rc["field_index"] );

            figure_rebuild( rc["class_index"] );
        }
        else
        if( pItem->m_nFieldRelationType == FieldRelationTypeForeignKey )
        {
            for( long n = RL->size() - 1; n >= 0; n -- )
            {
                auto &pRelation = RL->at( n );
                if( rc["class_index"] == pRelation->m_nTo )
                {
                    auto pClassFrom = m_pAppOptions->getClassList()->at( pRelation->m_nFrom );
                    FieldList_t PK; pClassFrom->enumFields( PK, FieldRelationTypePrimaryKey );
                    for( long n0 = PK.size() - 1; n0 >= 0; n0 -- )
                    {
                        auto &pk = PK.at( n0 );
                        if( pItem->m_PhysicalName == pk->m_PhysicalName )
                        {
                            relation_delete_fields( pRelation.get() );
                        }
                    }
                }
            }
        }
        return true;
    }
    return false;
}

std::map<std::string,long>
CBuilder::find_edit()
{
    return m_pAppOptions->getClassList()->find_edit();
}

long
CBuilder::center_hover( const QPoint &pos, bool set_up_hover )
{
    long n = m_pAppOptions->getClassList()->hover_center_index( pos );
    if( n >= 0 && set_up_hover )
    {
        auto &pItem = m_pAppOptions->getClassList()->at( n );
        if( pItem->m_nFigureType == FigureTypeRectangle )
        {
            return -1;
        }
        pItem->m_bHoverCenter = true;
    }
    return n;
}

long
CBuilder::resize_hover( const QPoint &pos, bool set_up_hover, bool set_up_resize )
{
    long n = m_pAppOptions->getClassList()->hover_resize_index( pos );
    if( n >= 0 && set_up_hover )
    {
        //getLogger()->info( APP_LOG_LEVEL, "info: last point hover[%d]", n );
        auto &pItem = m_pAppOptions->getClassList()->at( n );
        pItem->m_bHoverResize = true;
        if( set_up_resize )
        {
            resize_item( pos, pItem.get() );
        }
    }
    return n;
}

void
CBuilder::resize_item( const QPoint &pt, __Class *pItem )
{
    pItem->calc_first_last_point( pt );

    m_pAppOptions->getRelationList()->calculate( m_pAppOptions->getClassList() );

    //getLogger()->info( APP_LOG_LEVEL, "info: Angle[%d]", (int)qRadiansToDegrees( pItem->m_nAngle ) );
}

long
CBuilder::rotate_hover( const QPoint &pos, bool set_up_hover, bool set_up_rotate )
{
    long n = m_pAppOptions->getClassList()->hover_angle_index( pos );
    if( n >= 0 && set_up_hover )
    {
        auto &pItem = m_pAppOptions->getClassList()->at( n );
        pItem->m_bHoverAngle = true;
        if( set_up_rotate )
        {
            pItem->calc_angle_point( pos );
            //getLogger()->info( APP_LOG_LEVEL, "info: Angle[%d]", (int)qRadiansToDegrees( pItem->m_nAngle ) );
        }
    }
    return n;
}

bool
CBuilder::relation_move( const QPoint &pos )
{
    long n = m_pAppOptions->getRelationList()->hover_index( pos );
    if( n >= 0 )
    {
        //getLogger()->info( APP_LOG_LEVEL, "info: Relation move[%d]", n );
        auto &R = m_pAppOptions->getRelationList()->at( n );
        R->m_bHover = true;

        m_pAppOptions->setLastPos( pos );
        R->move( m_pAppOptions->getFirstPos(), m_pAppOptions->getLastPos() );

        m_pAppOptions->setFirstPos( pos );
        return true;
    }
    return false;
}

long
CBuilder::relation_hover( const QPoint &pos, bool set_up_hover )
{
    long n = m_pAppOptions->getRelationList()->hover_index( pos );
    if( n >= 0 && set_up_hover )
    {
        //getLogger()->info( APP_LOG_LEVEL, "info: Relation hover[%d]", n );
        auto &R = m_pAppOptions->getRelationList()->at( n );
        R->m_bHover = true;

        auto CL    = m_pAppOptions->getClassList();
        auto pFrom = CL->at( R->m_nFrom );
        auto pTo   = CL->at( R->m_nTo   );

        FieldList_t PK; pFrom->enumFields( PK, FieldRelationTypePrimaryKey );
        for_each( PK.begin(), PK.end(),
            [ pTo ](auto &pItemFrom)
            {
                pItemFrom->m_bHover = true;

                FieldList_t FK; pTo->enumFields( FK, FieldRelationTypeForeignKey );
                for_each( FK.begin(), FK.end(),
                    [ pItemFrom ](auto &pItemTo)
                    {
                        if( pItemFrom->m_PhysicalName == pItemTo->m_PhysicalName )
                        {
                            pItemTo->m_bHover = true;
                        }
                    }
                );
            }
        );
    }
    return n;
}

bool
CBuilder::relation_begin_add( long n )
{
    if( n >= 0 )
    {
        if( m_pAppOptions->getIndexFrom() == -1 )
        {
            getLogger()->info( APP_LOG_LEVEL, "info: Press left button for relation from figure [%d]", n );
            QPoint from;
            m_pAppOptions->setFirstPos( m_pAppOptions->getClassList()->at( n )->center( &from ) );
            m_pAppOptions->setIndexFrom( n );
            return true;
        }
    }
    return false;
}

bool
CBuilder::relation_release_add( const QPoint &pt, __Field *fromFK )
{
    long nIndexFrom = m_pAppOptions->getIndexFrom();
    if( nIndexFrom >= 0 )
    {
        m_pAppOptions->setIndexFrom( -1L );

        ClassList_t *figures = m_pAppOptions->getClassList();
        long nIndexTo = figures->hover_index( pt );
        if( nIndexTo >= 0 && nIndexFrom != nIndexTo )
        {
            if( m_pAppOptions->getRelationList()->validate( nIndexFrom, nIndexTo ) )
            {
                getLogger()->info( APP_LOG_LEVEL, "info: Press button for relation to figure [%d]->[%d] %d:%d", nIndexFrom, nIndexTo, pt.x(), pt.y() );

                if( fromFK != nullptr )
                {
                    auto &pk = fromFK;
                    pk->m_bFromFK = false;

                    __Field *pItem = field_add( nIndexTo, "..." );
                    pItem->m_LogicalName        = pk->m_LogicalName;
                    pItem->m_PhysicalName       = pk->m_PhysicalName;
                    pItem->m_nFieldDataType     = pk->m_nFieldDataType;
                    pItem->m_nFieldRelationType = FieldRelationTypeForeignKey;
                    pItem->m_bAllowsNulls       = true;

                    QPoint from, to;
                    m_pAppOptions->getRelationList()->Add(
                        CFigureFactory::buildRelation(
                            m_pAppOptions->getRelationType()
                            , nIndexFrom
                            , nIndexTo
                            , figures->at( nIndexFrom )->center( &from )
                            , figures->at( nIndexTo   )->center( &to   )
                            )
                        );

                    figure_rebuild( nIndexTo );
                    return true;
                }
                else
                {
                    auto pClass = figures->at( nIndexFrom );
                    FieldList_t PK; pClass->enumFields( PK, FieldRelationTypePrimaryKey );

                    for( long n = PK.size() - 1; n >= 0; n -- )
                    {
                        auto &pk = PK.at( n );

                        __Field *pItem = field_add( nIndexTo, "..." );
                        pItem->m_LogicalName        = pk->m_LogicalName;
                        pItem->m_PhysicalName       = pk->m_PhysicalName;
                        pItem->m_nFieldDataType     = pk->m_nFieldDataType;
                        pItem->m_nFieldRelationType = FieldRelationTypeForeignKey;
                        pItem->m_bAllowsNulls       = true;

                        QPoint from, to;
                        m_pAppOptions->getRelationList()->Add(
                            CFigureFactory::buildRelation(
                                m_pAppOptions->getRelationType()
                                , nIndexFrom
                                , nIndexTo
                                , figures->at( nIndexFrom )->center( &from )
                                , figures->at( nIndexTo   )->center( &to   )
                                )
                            );

                        figure_rebuild( nIndexTo );

                        return true;
                    }
                }
            }
        }
        getLogger()->info( APP_LOG_LEVEL, "error: Press button for relation to figure [%d]->[%d] %d:%d", nIndexFrom, nIndexTo, pt.x(), pt.y() );
    }
    return false;
}

bool
CBuilder::relation_delete( long n )
{
    if( n >= 0 )
    {
        if( n < (long)m_pAppOptions->getRelationList()->size() )
        {
            auto &pRelation = m_pAppOptions->getRelationList()->at( n );

            getLogger()->info( APP_LOG_LEVEL, "info: Delete relation [%d]->[%d]", pRelation->m_nFrom, pRelation->m_nTo );

            relation_delete_fields( pRelation.get() );
            return true;
        }
    }
    return false;
}

void
CBuilder::relation_delete_fields( __Relation *pRelation )
{
    auto pClassFrom = m_pAppOptions->getClassList()->at( pRelation->m_nFrom );
    auto pClassTo   = m_pAppOptions->getClassList()->at( pRelation->m_nTo   );
    FieldList_t PK; pClassFrom->enumFields( PK, FieldRelationTypePrimaryKey );
    FieldList_t FK; pClassTo  ->enumFields( FK, FieldRelationTypeForeignKey );
    for( long n0 = PK.size() - 1; n0 >= 0; n0 -- )
    {
        auto &pk = PK.at( n0 );
        for( long n1 = FK.size() - 1; n1 >= 0; n1 -- )
        {
            auto &fk = FK.at( n1 );
            if( pk->m_PhysicalName == fk->m_PhysicalName )
            {
                pClassTo->m_FieldList.Delete( fk.get() );
                goto end_loops;
            }
        }
    }
    end_loops:;

    figure_rebuild( pRelation->m_nTo );

    m_pAppOptions->getRelationList()->Delete( pRelation );
}

bool
CBuilder::relation_rotate_type( long n )
{
    if( n >= 0 )
    {
        if( n < (long)m_pAppOptions->getRelationList()->size() )
        {
            auto &pRelation = m_pAppOptions->getRelationList()->at( n );
            getLogger()->info( APP_LOG_LEVEL, "info: Change relation type [%d]->[%d]", pRelation->m_nFrom, pRelation->m_nTo );

            pRelation->m_nRelationType = CAppOptions::rotateRelationType( pRelation->m_nRelationType );
            return true;
        }
    }
    return false;
}

bool
CBuilder::relation_set_last_pos( const QPoint &pt )
{
    long nIndexFrom = m_pAppOptions->getIndexFrom();
    if( nIndexFrom >= 0 )
    {
        m_pAppOptions->setLastPos( pt );
        //getLogger()->info( APP_LOG_LEVEL, "info: Show new relation" );
        return true;
    }
    return false;
}

long
CBuilder::relation_oscillation( QPoint &pt, int dx, int dy )
{
    RelationList_t *rl = m_pAppOptions->getRelationList();
    long n = rl->hover_index( pt );
    if( n >= 0 )
    {
        auto &r = rl->at( n );
        r->oscillation( dx, dy );

        // ClassList_t *fl = m_pAppOption->getClassList();
        // auto c1 = fl->at(r->m_nFrom);
        // auto c2 = fl->at(r->m_nTo  );

        // c1->oscillation( dx, dy );
        // c2->oscillation( dx, dy );
    }
    return n;
}

long
CBuilder::figure_oscillation( QPoint &pt, int dx, int dy )
{
    ClassList_t *fl = m_pAppOptions->getClassList();
    long n = fl->hover_index( pt );
    if( n >= 0 )
    {
        fl->at( n )->oscillation( dx, dy );

        auto rl = m_pAppOptions->getRelationList();

        for( long i = rl->size() - 1; i >= 0; i -- )
        {
            auto &p = rl->at( i );
            if( p->m_nFrom == (long)n || p->m_nTo == (long)n )
            {
                p->oscillation( dx, dy );
            }
        }
    }
    return n;
}

void
CBuilder::figure_rebuild( long n )
{
    auto pClass = m_pAppOptions->getClassList()->at( n );
    FieldList_t PK; pClass->enumFields( PK, FieldRelationTypePrimaryKey );
    FieldList_t FK; pClass->enumFields( FK, FieldRelationTypeForeignKey );
    FieldList_t NN; pClass->enumFields( NN, FieldRelationTypeNone       );
    pClass->aggregateFields( PK, FK, NN );
    pClass->field_recalc();
}

void
CBuilder::figure_area( QPoint &first_pos, QPoint &last_pos )
{
    auto CL = m_pAppOptions->getClassList();
    for_each( CL->begin(), CL->end(),
         [&first_pos,&last_pos]( auto &pItem )
         {
            QPoint first_pos0( pItem->m_nFirstPos.x(), pItem->m_nFirstPos.y() );
            if( first_pos.x() > first_pos0.x() ) first_pos.setX( first_pos0.x() );
            if( first_pos.y() > first_pos0.y() ) first_pos.setY( first_pos0.y() );

            QPoint last_pos0( pItem->m_nLastPos.x(), pItem->m_nLastPos.y() );
            if( last_pos0.x() > last_pos.x() ) last_pos.setX( last_pos0.x() );
            if( last_pos0.y() > last_pos.y() ) last_pos.setY( last_pos0.y() );
         }
    );
}

QSize&
CBuilder::figure_size( QSize &area_size, const QSize &widget_size )
{
    QPoint first_pos, last_pos;
    figure_area( first_pos, last_pos );

    area_size.setWidth ( last_pos.x() + DEFAULT_SCROLL_SIZE );
    area_size.setHeight( last_pos.y() + DEFAULT_SCROLL_SIZE );
    if( widget_size.width () > area_size.width () ) area_size.setWidth ( widget_size.width () );
    if( widget_size.height() > area_size.height() ) area_size.setHeight( widget_size.height() );

    // getLogger()->info( APP_LOG_LEVEL, "info: figure_size area_size [%d]:[%d]", area_size.width(), area_size.height() );
    // getLogger()->info( APP_LOG_LEVEL, "info: figure_size area_pos  [%d]:[%d]", area_pos.x(), area_pos.y() );

    return area_size;
}

void
CBuilder::relation_rebuild()
{
    auto fl = m_pAppOptions->getClassList();
    auto rl = m_pAppOptions->getRelationList();

    for( long i = fl->size() - 1; i >= 0; i -- )
    {
        auto &f = fl->at( i );

        QRect rc;
        rc.setTopLeft    ( f->m_nFirstPos );
        rc.setBottomRight( f->m_nLastPos  );

        for( long ii = rl->size() - 1; ii >= 0; ii -- )
        {
            auto &r = rl->at( ii );
            if( r->m_nFrom == (long)i )
            {
                r->m_nFirstPos = rc.center();
            }
            else
            if( r->m_nTo == (long)i )
            {
                r->m_nLastPos = rc.center();
            }
        }
    }
}
