/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Relation.h"

#include <QRect>
#include <QPainterPath>
#include <QtMath>

#include "common/Logger.h"
#include "common/Utils.h"
#include "common/Compare.h"

#include "data/Options.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
#define                             __AppOptions     (*__AppOptions__.get())

/*                */
/* Class Relation */
/*                */

__Relation&
__Relation::operator = ( const __Relation& o )
{
    m_nRelationType = o.m_nRelationType;
    m_nFrom         = o.m_nFrom        ;
    m_nTo           = o.m_nTo          ;
    m_nFirstPos     = o.m_nFirstPos    ;
    m_nLastPos      = o.m_nLastPos     ;
    m_bHover        = o.m_bHover       ;
    m_bChanged      = o.m_bChanged     ;
    m_Name          = o.m_Name         ;

    return *this;
}

void
__Relation::init()
{
    m_nFirstPos           = {};
    m_nLastPos            = {};
    m_nRelationType       = RelationType_t::RelationTypeNone;
    m_nFrom               = -1;
    m_nTo                 = -1;
    m_bHover              = false;
    m_bChanged            = false;
    m_Name                = "";
    m_PKTableLabel        = "";
    m_FKTableLabel        = "";
    m_nType               = RelationPropertyType_t::NonIdentifying;
    m_nCardinalityPKTable = RelationPropertyCardinality_t::ExactlyOne;
    m_nCardinalityFKTable = RelationPropertyCardinality_t::ZeroOrMore;
    m_nDeferrability      = RelationPropertyDeferrability_t::NotDeferrability;
    m_nUpdateRule         = RelationPropertyRule_t::NoAction;
    m_nDeleteRule         = RelationPropertyRule_t::NoAction;
    m_dX                  = DEFAULT_DX;
    m_dY                  = DEFAULT_DY;
}

void
__Relation::calculate( Class_t &pFrom, Class_t &pTo )
{
    assert( m_nRelationType != RelationTypeNone );

    pFrom.center( &m_nFirstPos );
    pTo  .center( &m_nLastPos  );
}

float
__Relation::distancePointToLine( const QPoint &p1, const QPoint &p2, const QPoint &p )
{
    float dx = p2.x() - p1.x();
    float dy = p2.y() - p1.y();

    if( qFuzzyIsNull( dx ) && qFuzzyIsNull( dy ) )
    {
        return QLineF(p1, p).length();
    }

    float numerator   = qAbs(dy * p.x() - dx * p.y() + p2.x() * p1.y() - p2.y() * p1.x());
    float denominator = qSqrt(dx * dx + dy * dy);

    return numerator / denominator;
}

bool
__Relation::isPointNearLine(const QPoint &p1, const QPoint &p2, const QPoint &p, qreal tolerance )
{
    if( distancePointToLine(p1, p2, p) < tolerance )
    {
        qreal minX = qMin(p1.x(), p2.x());
        qreal maxX = qMax(p1.x(), p2.x());
        qreal minY = qMin(p1.y(), p2.y());
        qreal maxY = qMax(p1.y(), p2.y());

        return (p.x() >= minX && p.x() <= maxX &&
                p.y() >= minY && p.y() <= maxY);
    }
    return false;
}

void
__Relation::move( const QPoint &from, const QPoint &to )
{
    QPoint diff = to - from;

    // ClassList_t *figures = __AppOptions.getClassList();
    // auto         pFrom   = figures->at( m_nFrom );
    // auto         pTo     = figures->at( m_nTo   );
    // QRectF       rc_from{ pFrom->m_nFirstPos, pFrom->m_nLastPos };
    // QRectF       rc_to  { pTo  ->m_nFirstPos, pTo  ->m_nLastPos };

    // qreal l_from = QLineF( to, rc_from.center() ).length();
    // qreal l_to   = QLineF( to, rc_to  .center() ).length();

    // if( l_from < l_to )
    // {
    //     // QPointF pt = m_nFirstPos - diff; // - QPointF( m_dX, -m_dY )
    //     // m_nFirstPos = pointf2point( correct_point( pt, rc_from ) );m_nFirstPos

    //     __Logger.debug( APP_LOG_LEVEL, "BeforeFirstPos:%d-%d",m_nFirstPos.x(),m_nFirstPos.y() );
    //     m_nFirstPos += diff;
    //     m_nFirstPos = pointf2point( correct_point( m_nFirstPos, rc_from ) );
    //     __Logger.debug( APP_LOG_LEVEL, "AfterFirstPos:%d-%d",m_nFirstPos.x(),m_nFirstPos.y() );
    // }
    // else
    // {
    //     // QPointF pt = m_nLastPos - diff; // - QPointF( -m_dX, m_dY )
    //     // m_nLastPos = pointf2point( correct_point( pt, rc_from ) );

    //     __Logger.debug( APP_LOG_LEVEL, "BeforeLastPos:%d-%d",m_nLastPos.x(),m_nLastPos.y() );
    //     m_nLastPos += diff;
    //     m_nLastPos = pointf2point( correct_point( m_nLastPos, rc_from ) );
    //     __Logger.debug( APP_LOG_LEVEL, "AfterLastPos:%d-%d",m_nLastPos.x(),m_nLastPos.y() );
    // }




    // QPoint point = to;

    // ClassList_t *figures = __AppOptions.getClassList();
    // auto         pFrom   = figures->at( m_nFrom );
    // auto         pTo     = figures->at( m_nTo   );
    // QRectF       rc_from{ pFrom->m_nFirstPos, pFrom->m_nLastPos };
    // QRectF       rc_to  { pTo  ->m_nFirstPos, pTo  ->m_nLastPos };
    // QLineF       line( correct_point(offset_first(),rc_from), correct_point(offset_last(),rc_to) );

    // std::vector <__PointType> intersectionFrom; draw_relation_intersection( intersectionFrom, pFrom.get(), line );
    // std::vector <__PointType> intersectionTo  ; draw_relation_intersection( intersectionTo  , pTo  .get(), line );

    // std::find_if( __EXECUTION_POLICY_LIST__, intersectionFrom.begin(), intersectionFrom.end(),
    // [this,&intersectionTo,pFrom,pTo,point,diff,rc_from,rc_to]( auto &pLineFrom )
    // {
    //    if( pLineFrom.m_nIntersectionType == QLineF::BoundedIntersection )
    //    {
    //        auto found2 = std::find_if( __EXECUTION_POLICY_LIST__, intersectionTo.begin(), intersectionTo.end(),
    //        [this,&pLineFrom,pFrom,pTo,point,diff,rc_from,rc_to]( auto &pLineTo )
    //        {
    //           if( pLineTo.m_nIntersectionType == QLineF::BoundedIntersection )
    //           {
    //               QPoint from = {int(pLineFrom.m_nPoint.x()),int(pLineFrom.m_nPoint.y())},
    //                      to   = {int(pLineTo  .m_nPoint.x()),int(pLineTo  .m_nPoint.y())};
    //               bool res = isPointNearLine( from, to, point );
    //               if( res )
    //               {
    //                   qreal l_from = QLineF( point, from ).length();
    //                   qreal l_to   = QLineF( point, to   ).length();
    //                   if( l_from < l_to )
    //                   {
    //                       m_nFirstPos += diff;
    //                       m_nFirstPos = pointf2point( correct_point( m_nFirstPos, rc_from ) );
    //                   }
    //                   else
    //                   {
    //                       m_nLastPos += diff;
    //                       m_nLastPos = pointf2point( correct_point( m_nLastPos, rc_from ) );
    //                   }
    //               }
    //               return res;
    //           }
    //           return false;
    //        }
    //        );
    //        return found2 != intersectionTo.end();
    //    }
    //    return false;
    // }
    // );



}

QPoint
__Relation::offset_first()
{
    return m_nFirstPos + QPoint(  m_dX, -m_dY );
}

QPoint
__Relation::offset_last()
{
    return m_nLastPos + QPoint( -m_dX,  m_dY );
}


int
__Relation::compare( const __Relation &o )
{
    int     rc = Cmp( this->m_nFirstPos.y(), o.m_nFirstPos.y() );
    if(!rc) rc = Cmp( this->m_nFirstPos.x(), o.m_nFirstPos.x() );
    if(!rc) rc = Cmp( this->m_nLastPos .y(), o.m_nLastPos .y() );
    if(!rc) rc = Cmp( this->m_nLastPos .x(), o.m_nLastPos .x() );
    return rc;
}

bool
__Relation::contain( const QPoint &point )
{
    //assert( m_nRelationType != RelationTypeNone );
    //return isPointNearLine( m_nFirstPos, m_nLastPos, point );

    ClassList_t *figures = __AppOptions.getClassList();
    auto         pFrom   = figures->at( m_nFrom );
    auto         pTo     = figures->at( m_nTo   );
    QRectF       rc_from{ pFrom->m_nFirstPos, pFrom->m_nLastPos };
    QRectF       rc_to  { pTo  ->m_nFirstPos, pTo  ->m_nLastPos };
    QLineF       line( correct_point(offset_first(),rc_from), correct_point(offset_last(),rc_to) );

    std::vector <__PointType> intersectionFrom; draw_relation_intersection( intersectionFrom, pFrom.get(), line );
    std::vector <__PointType> intersectionTo  ; draw_relation_intersection( intersectionTo  , pTo  .get(), line );

    auto found1 = std::find_if( __EXECUTION_POLICY_LIST__, intersectionFrom.begin(), intersectionFrom.end(),
        [this,&intersectionTo,pFrom,pTo,point]( auto &pLineFrom )
        {
            if( pLineFrom.m_nIntersectionType == QLineF::BoundedIntersection )
            {
                auto found2 = std::find_if( __EXECUTION_POLICY_LIST__, intersectionTo.begin(), intersectionTo.end(),
                    [this,&pLineFrom,pFrom,pTo,point]( auto &pLineTo )
                    {
                        if( pLineTo.m_nIntersectionType == QLineF::BoundedIntersection )
                        {
                            QPoint from = {int(pLineFrom.m_nPoint.x()),int(pLineFrom.m_nPoint.y())},
                                   to   = {int(pLineTo  .m_nPoint.x()),int(pLineTo  .m_nPoint.y())};
                            return isPointNearLine( from, to, point );
                        }
                        return false;
                    }
                );
                return found2 != intersectionTo.end();
            }
            return false;
        }
    );
    return found1 != intersectionFrom.end();
}

void
__Relation::oscillation( int dx, int dy )
{
    QPoint dp = QPoint( dx, dy );
    m_nFirstPos -= dp;
    m_nLastPos  += dp;
}

/*                     */
/* Class Relation List */
/*                     */

bool
RelationList_t::validate( long nFrom, long nTo )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(),
        [ nFrom, nTo ]( auto &pItem ) { return pItem->m_nFrom == nFrom && pItem->m_nTo == nTo; } );
    return found == rend();
}

void
RelationList_t::calculate( ClassList_t *pFigureList )
{
    std::for_each( __EXECUTION_POLICY_LIST__, begin(), end(),
        [ pFigureList ]( auto &pItem ) { pItem->calculate( *pFigureList->at( pItem->m_nFrom ), *pFigureList->at( pItem->m_nTo ) ); } );
}

void
RelationList_t::hover_clear() { for( auto &p : *this ) { p->m_bHover = false; } }

long
RelationList_t::hover_index( const QPoint &pos )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(),
        [ pos ]( auto &pItem ) { return pItem->contain( pos ); } );
    return ( found != rend() ) ? std::distance( begin(), -- found.base() ) : -1;
}

bool SortRelation(const std::shared_ptr<__Relation>& l, const std::shared_ptr<__Relation>& r)
{
    return l.get()->compare( *r.get() ) < 0;
}

void RelationList_t::sort()
{
    std::sort( __EXECUTION_POLICY_LIST__, begin(), end(), SortRelation );
}
