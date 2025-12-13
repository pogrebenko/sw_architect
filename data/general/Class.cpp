/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Class.h"

#include <algorithm>

#include <QRect>
#include <QPainterPath>
#include <QtMath>

#include "common/Utils.h"
#include "common/Logger.h"
#include "common/Compare.h"

#include "data/Options.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
#define                             __AppOptions (*__AppOptions__.get())

/* class __Class */
__Class&
__Class::operator = ( const __Class &o )
{
    m_nFigureType  = o.m_nFigureType ;
    m_nFirstPos    = o.m_nFirstPos   ;
    m_nLastPos     = o.m_nLastPos    ;
    m_nTitlePos    = o.m_nTitlePos   ;
    m_nAnglePos    = o.m_nAnglePos   ;
    m_nResizePos   = o.m_nResizePos  ;
    m_bHover       = o.m_bHover      ;
    m_bHoverTitle  = o.m_bHoverTitle ;
    m_bHoverFirst  = o.m_bHoverFirst ;
    m_bHoverLast   = o.m_bHoverLast  ;
    m_bHoverAngle  = o.m_bHoverAngle ;
    m_bHoverResize = o.m_bHoverResize;
    m_bHoverCenter = o.m_bHoverCenter;
    m_bEditText    = o.m_bEditText   ;
    m_bEditOptions = o.m_bEditOptions;
    m_nAngle       = o.m_nAngle      ;
    m_bSelect      = o.m_bSelect     ;
    m_nAnglesCount = o.m_nAnglesCount;
    m_LogicalName  = o.m_LogicalName ;
    m_PhysicalName = o.m_PhysicalName;
    m_FieldList    = o.m_FieldList   ;
    m_bChanged     = o.m_bChanged    ;

    return *this;
}

void
__Class::init()
{
    m_nFigureType  = FigureTypeNone;
    m_nFirstPos    = {};
    m_nLastPos     = {};
    m_nAnglePos    = {};
    m_nResizePos   = {};
    m_bHover       = false;
    m_bHoverTitle  = false;
    m_bHoverFirst  = false;
    m_bHoverLast   = false;
    m_bHoverAngle  = false;
    m_bHoverResize = false;
    m_bHoverCenter = false;
    m_bEditText    = false;
    m_bEditOptions = false;
    m_bChanged     = false;
    m_bSelect      = false;
    m_nAngle       = 0.;
    m_nAnglesCount = -1;
    m_FieldList.clear();
}

void
__Class::enumFields( FieldList_t &List, FieldRelationType_t Type )
{
    List.clear();
    for_each( __EXECUTION_POLICY_BUILDER__, m_FieldList.begin(), m_FieldList.end(), [&List,Type]( auto &pItem ){ if( pItem->m_nFieldRelationType == Type ) List.push_back( pItem ); } );
}

void
__Class::aggregateFields( FieldList_t &PK, FieldList_t &FK, FieldList_t &NN )
{
    m_FieldList.clear();
    for_each( __EXECUTION_POLICY_BUILDER__, PK.begin(), PK.end(), [this]( auto &pItem ){ m_FieldList.push_back( pItem ); } );
    for_each( __EXECUTION_POLICY_BUILDER__, FK.begin(), FK.end(), [this]( auto &pItem ){ m_FieldList.push_back( pItem ); } );
    for_each( __EXECUTION_POLICY_BUILDER__, NN.begin(), NN.end(), [this]( auto &pItem ){ m_FieldList.push_back( pItem ); } );
}

void
__Class::field_recalc()
{
    for( unsigned long n = 0; n < m_FieldList.size(); ++ n )
    {
        QPoint nFirstPos, nLastPos;
        if( n == 0 )
        {
            nFirstPos.setX( m_nFirstPos.x() );
            nFirstPos.setY( m_nTitlePos.y() );

            nLastPos .setX( m_nTitlePos.x() );
            nLastPos .setY( m_nTitlePos.y() + m_nTitlePos.y() - m_nFirstPos.y() );
        }
        else
        {
            auto &Last = m_FieldList.at( n - 1 );

            nFirstPos.setX( Last->m_nFirstPos.x() );
            nFirstPos.setY( Last->m_nLastPos .y() );

            nLastPos .setX( Last->m_nLastPos .x() );
            nLastPos .setY( Last->m_nLastPos .y() + Last->m_nLastPos.y() - Last->m_nFirstPos.y() );
        }
        auto pField = m_FieldList.at( n );
             pField->m_nFirstPos = nFirstPos;
             pField->m_nLastPos  = nLastPos;
    }
}

bool
__Class::contain_title( const QPoint &mousePos )
{
    QRectF       rcf;
                 rcf.setTopLeft    ( m_nFirstPos );
                 rcf.setBottomRight( m_nLastPos  );
    QPointF      center( rcf.center() );

    QTransform   transform;
                 transform.translate( center.x(), center.y() );
                 transform.rotate( -qRadiansToDegrees( m_nAngle ) );
                 transform.translate( -center.x(), -center.y() );

    rcf.setBottomRight( m_nTitlePos ); // !

    QPainterPath rectanglePath;
                 rectanglePath.addRect( rcf );

    return transform.map( rectanglePath ).contains( mousePos );
//    return contain_rectangle( mousePos, m_nFirstPos, m_nTitlePos, m_nAngle );
}

int
__Class::compare( const QPoint &o )
{
    int     rc = Cmp( this->m_nFirstPos.x(), o.x()   );
    //if(!rc) rc = Cmp( this->m_nLastPos .x(), o.x()+1 );
    if(!rc) rc = Cmp( this->m_nFirstPos.y(), o.y()   );
    //if(!rc) rc = Cmp( this->m_nLastPos .y(), o.y()+1 );
    return rc;
}

int
__Class::compare( const __Class &o )
{
    int     rc = Cmp( this->m_nFirstPos.x(), o.m_nFirstPos.x() );
    //if(!rc) rc = Cmp( this->m_nLastPos .x(), o.m_nLastPos .x() );
    if(!rc) rc = Cmp( this->m_nFirstPos.y(), o.m_nFirstPos.y() );
    //if(!rc) rc = Cmp( this->m_nLastPos .y(), o.m_nLastPos .y() );
    return rc;
}

bool
__Class::contain( const QPoint &mousePos )
{
    QRectF  rcf;
            rcf.setTopLeft    ( this->m_nFirstPos );
            rcf.setBottomRight( this->m_nLastPos  );

    QPointF center( rcf.center() );

    int n = m_nAnglesCount;

    qreal x0 = 0; qreal y0 = 0;

    double width  = abs( rcf.width () );
    double height = abs( rcf.height() );
    double radius = std::sqrt( width * width + height * height ) / 2;

    QPainterPath path;
    for( int i = 0; i < n; ++ i )
    {
        qreal fAngle = ( 2 * M_PI * i ) / n  - M_PI/2 - m_nAngle;
        qreal x = center.x() + cos( fAngle ) * radius;
        qreal y = center.y() + sin( fAngle ) * radius;

        if( i == 0 )
        {
            x0 = x; y0 = y;
            path.moveTo(x, y);
        }
        else
        {
            path.lineTo(x,y);
        }
    }
    path.lineTo( x0, y0 );

    return path.contains( mousePos );
}

void
__Class::calc_angle_resize_point( int nHeight )
{
    QRectF rcf;
    rcf.setTopLeft    ( m_nFirstPos ); // after build
    rcf.setBottomRight( m_nLastPos  ); // after build

    double width  = abs( rcf.width () );
    double height = abs( rcf.height() );
    double radius = std::sqrt( width * width + height * height ) / 2;

    QLineF upward( rcf.center(), m_nFirstPos );
           upward.setLength( radius + DELTA_ANGLE );
    m_nAnglePos.setX( upward.p2().x() );
    m_nAnglePos.setY( upward.p2().y() );

    QLineF downward( rcf.center(), m_nLastPos );
           downward.setLength( radius + DELTA_ANGLE );
    m_nResizePos.setX( downward.p2().x() );
    m_nResizePos.setY( downward.p2().y() );

    m_nTitlePos = { m_nLastPos.x(), m_nFirstPos.y() + nHeight };
}

void
__Class::calc_first_last_point( const QPoint &pt )
{
    m_nResizePos = pt;

    QLineF diagonal( m_nFirstPos, m_nResizePos );
           diagonal.setLength( diagonal.length() - DELTA_ANGLE );
    m_nLastPos.setX( diagonal.p2().x() );
    m_nLastPos.setY( diagonal.p2().y() );

    QRectF rcf_base;
           rcf_base.setTopLeft( m_nFirstPos );
           rcf_base.setBottomRight( m_nLastPos );

    QLineF line_first( rcf_base.center(), m_nFirstPos );
    QLineF line_resize( rcf_base.center(), m_nResizePos );

    QLineF line_from( rcf_base.center(), m_nFirstPos );
           line_from.setLength( line_resize.length() );
           line_from.setAngle( line_first.angle() + qRadiansToDegrees( m_nAngle ) );
    m_nAnglePos.setX( line_from.p2().x() );
    m_nAnglePos.setY( line_from.p2().y() );

    m_nTitlePos.setX( m_nLastPos.x() );
    for_each( m_FieldList.begin(), m_FieldList.end(),
         [this]( auto &pField ){ pField->m_nLastPos.setX( m_nLastPos.x() ); }
    );
}

void
__Class::calc_angle_point( const QPoint &pt )
{
    QRectF rcf;
           rcf.setTopLeft    ( m_nFirstPos );
           rcf.setBottomRight( m_nLastPos  );

    double width  = abs( rcf.width () );
    double height = abs( rcf.height() );
    double radius = std::sqrt( width * width + height * height ) / 2;

    QLineF line_from( rcf.center(), m_nFirstPos );
    QLineF line_to  ( rcf.center(), pt          );

    line_to.setLength( radius + DELTA_ANGLE );
    m_nAnglePos.setX( line_to.p2().x() );
    m_nAnglePos.setY( line_to.p2().y() );
    m_nAngle = qDegreesToRadians( line_from.angleTo( line_to ) );

    m_nTitlePos.setX( m_nLastPos.x() );

    std::for_each( __EXECUTION_POLICY_BUILDER__, m_FieldList.begin(), m_FieldList.end(),
        [ this ]( auto pField ){ pField->m_nAngle = m_nAngle; }
    );
}

bool
__Class::near_points( const QPoint &pt1, const QPoint &pt2 )
{
    QRectF rcf;
    rcf.setTopLeft    ( pt1 );
    rcf.setBottomRight( pt2 );

    double width  = abs( rcf.width () );
    double height = abs( rcf.height() );

    double radius = std::sqrt( width * width + height * height );

    return radius <= MAX_NEAR_SIZE;
}

void
__Class::move( const QPoint &from, const QPoint &to )
{
    assert( m_nFigureType != FigureTypeNone );

    QPoint diff = to - from;

    m_nFirstPos += diff;
    m_nTitlePos += diff;
    m_nLastPos  += diff;
    m_nAnglePos += diff;
    m_nResizePos+= diff;

    std::for_each( __EXECUTION_POLICY_BUILDER__, m_FieldList.begin(), m_FieldList.end(),
        [diff]( auto &pItem ){ pItem->m_nFirstPos += diff; pItem->m_nLastPos += diff; }
    );
}

const QPoint&
__Class::center( QPoint *point )
{
    assert( m_nFigureType != FigureTypeNone );

    QRect rc;
    rc.setTopLeft    ( m_nFirstPos );
    rc.setBottomRight( m_nLastPos  );

    *point = rc.center();
    return *point;
}

bool
__Class::valid()
{
    QRect rc;
    rc.setTopLeft    ( m_nFirstPos );
    rc.setBottomRight( m_nLastPos  );
    if( m_nFigureType == FigureTypeTriangle )
    {
        return
            (abs(rc.width()) > MIN_FIGURE_SIZE && abs(rc.height()) > MIN_FIGURE_SIZE) ||
            (abs(rc.width()) < MAX_FIGURE_SIZE && abs(rc.height()) < MAX_FIGURE_SIZE);
    }
    else
    {
        return
            abs(rc.width()) > MIN_FIGURE_SIZE && abs(rc.height()) > MIN_FIGURE_SIZE &&
            abs(rc.width()) < MAX_FIGURE_SIZE && abs(rc.height()) < MAX_FIGURE_SIZE;
    }
}

void
__Class::oscillation( int dx, int dy )
{
    QPoint dp = QPoint( dx, dy );

    m_nFirstPos += dp;
    m_nLastPos  -= dp;

    m_nTitlePos -= dp;

    m_nAnglePos += dp;
    m_nResizePos-= dp;
}

/* class Figure List */

void
ClassList_t::select_clear()
{
    for_each( begin(), end(), []( auto &pItem ){ pItem->m_bSelect = false; } );
}

void
ClassList_t::hover_clear()
{
    for_each( begin(), end(),
        []( auto &pItem )
        {
            pItem->m_bHover      =
            pItem->m_bHoverFirst =
            pItem->m_bHoverLast  =
            pItem->m_bHoverAngle =
            pItem->m_bHoverResize=
            pItem->m_bHoverTitle =
            pItem->m_bHoverCenter= false;

            pItem->m_FieldList.hover_clear();
        }
    );
}

void
ClassList_t::edit_clear()
{
    std::for_each( __EXECUTION_POLICY_LIST__, begin(), end(),
        []( auto pItem )
        {
            pItem->m_bEditText    =
            pItem->m_bEditOptions = false;
            std::for_each( __EXECUTION_POLICY_LIST__, pItem->m_FieldList.begin(), pItem->m_FieldList.end(),
                []( auto &pField ){
                    pField->m_bEditText    =
                    pField->m_bEditOptions = false;
                }
            );
        }
    );
}

void
ClassList_t::edit_title( const std::string &text )
{
    std::find_if( __EXECUTION_POLICY_LIST__, begin(), end(),
        [text]( auto pItem ) {
            if( pItem->m_bEditText || pItem->m_bEditOptions )
            {
                pItem->m_LogicalName = pItem->m_PhysicalName = text;
                return true;
            }
            else
            {
                auto found = std::find_if( __EXECUTION_POLICY_LIST__, pItem->m_FieldList.begin(), pItem->m_FieldList.end(),
                    [text]( auto &pField ){
                        if( pField->m_bEditText || pField->m_bEditOptions )
                            pField->m_LogicalName = pField->m_PhysicalName = text;
                        return pField->m_bEditText || pField->m_bEditOptions;
                    }
                );
                return found != pItem->m_FieldList.end();
            }
        }
    );
}

std::map<std::string,long>
ClassList_t::find_edit()
{
    std::map<std::string,long> rc = {{"class_index",-1},{"field_index",-1}};
    auto found1 = std::find_if( __EXECUTION_POLICY_LIST__, begin(), end(),
        [&rc]( auto pItem ) {
            if( pItem->m_bEditText || pItem->m_bEditOptions )
            {
                return true;
            }
            else
            {
                auto found2 = std::find_if( __EXECUTION_POLICY_LIST__, pItem->m_FieldList.begin(), pItem->m_FieldList.end(),
                    [&rc]( auto &pField ){
                        return pField->m_bEditText || pField->m_bEditOptions;
                    }
                );
                rc["field_index"] = ( found2 != pItem->m_FieldList.end() ) ? std::distance( pItem->m_FieldList.begin(), found2 ) : -1;
                return found2 != pItem->m_FieldList.end();
            }
        }
    );
    rc["class_index"] = ( found1 != end() ) ? std::distance( begin(), found1 ) : -1;
    return rc;
}

std::map<std::string,long>
ClassList_t::find_fromFK()
{
    std::map<std::string,long> rc = {{"class_index",-1},{"field_index",-1}};
    auto found1 = std::find_if( __EXECUTION_POLICY_LIST__, begin(), end(),
        [&rc]( auto pItem ) {
           auto found2 = std::find_if( __EXECUTION_POLICY_LIST__, pItem->m_FieldList.begin(), pItem->m_FieldList.end(),
              [&rc]( auto &pField ){ return pField->m_bFromFK; }
           );
           rc["field_index"] = ( found2 != pItem->m_FieldList.end() ) ? std::distance( pItem->m_FieldList.begin(), found2 ) : -1;
           return found2 != pItem->m_FieldList.end();
       }
    );
    rc["class_index"] = ( found1 != end() ) ? std::distance( begin(), found1 ) : -1;
    return rc;
}

long
ClassList_t::hover_title( const QPoint &pos )
{
    auto found_reverse = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(), [pos]( auto pItem ) { return pItem->contain_title( pos ); } );
    return ( found_reverse != rend() ) ? std::distance( begin(), -- found_reverse.base() ) : -1;
}

long
ClassList_t::hover_index( const QPoint &pos )
{
    if( false )
    {
        return find( pos );
    }
    else
    {
        auto found_reverse = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(), [pos]( auto pItem ) { return pItem->contain( pos ); } );
        return ( found_reverse != rend() ) ? std::distance( begin(), -- found_reverse.base() ) : -1;
    }
}

long
ClassList_t::hover_center_index( const QPoint &pos )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(),
        [pos]( auto pItem )
        {
            QRect rc;
                  rc.setTopLeft    ( pItem->m_nFirstPos );
                  rc.setBottomRight( pItem->m_nLastPos  );
            return pItem->near_points( rc.center(), pos );
        }
    );
    return ( found != rend() ) ? std::distance( begin(), -- found.base() ) : -1;
}

long
ClassList_t::hover_resize_index( const QPoint &pos )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(), [pos]( auto pItem ) { return pItem->near_points( pItem->m_nResizePos, pos ); } );
    return ( found != rend() ) ? std::distance( begin(), -- found.base() ) : -1;
}

long
ClassList_t::hover_angle_index( const QPoint &pos )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(), [pos]( auto pItem ) { return pItem->near_points( pItem->m_nAnglePos, pos ); } );
    return ( found != rend() ) ? std::distance( begin(), -- found.base() ) : -1;
}

long
ClassList_t::hover_first_index( const QPoint &pos )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(), [pos]( auto pItem ) { return pItem->near_points( pItem->m_nFirstPos, pos ); } );
    return ( found != rend() ) ? std::distance( begin(), -- found.base() ) : -1;
}

long
ClassList_t::hover_last_index( const QPoint &pos )
{
    auto found = std::find_if( __EXECUTION_POLICY_LIST__, rbegin(), rend(), [pos]( auto pItem ) { return pItem->near_points( pItem->m_nLastPos, pos ); } );
    return ( found != rend() ) ? std::distance( begin(), -- found.base() ) : -1;
}


bool SortClass(const std::shared_ptr<__Class>& l, const std::shared_ptr<__Class>& r)
{
    return l.get()->compare( *r.get() ) < 0;
}

long ClassList_t::find( const QPoint &pos )
{
    auto rc = std::lower_bound( begin(),this->end(), pos,
        []( const std::shared_ptr<__Class>& pItem, const QPoint &pos )
        {
        return pItem->compare( pos ) < 0;
        }
    );
    return std::distance( begin(), rc );
}

void ClassList_t::sort()
{
    std::sort( __EXECUTION_POLICY_LIST__, begin(), end(), SortClass );
}
