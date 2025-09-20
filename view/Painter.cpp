/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Painter.h"

#include <cmath>

#include <QtMath>

#include "common/Logger.h"

#include "data/Options.h"

#include "data/shapes/FigureFactory.h"

extern CLogStream __Logger;

CPainter::CPainter( CAppOptions *pAppOptions )
: m_pAppOptions( pAppOptions )
{
}

void
CPainter::painter_draw( QPainter &painter )
{
    painter.drawPixmap( 0, 0, m_pixmap );
}

void
CPainter::painter_resize( const QRect &rc )
{
    auto newRect = m_pixmap.rect().united( rc );
    if( newRect == m_pixmap.rect() )
    {
        return;
    }

    QPixmap newPixmap{ newRect.size() };

    QPainter painter{ &newPixmap };
    painter.fillRect( newPixmap.rect(), m_pAppOptions->getBkgColor() );
    painter.drawPixmap( 0, 0, m_pixmap );

    m_pixmap = newPixmap;
}

void
CPainter::draw_figure_relation()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    QPainter painter{ &m_pixmap };
             painter.setRenderHint( QPainter::Antialiasing, true );
             painter.fillRect( m_pixmap.rect(), m_pAppOptions->getBkgColor() );
    draw_figure  ( painter, m_pAppOptions->getClassList  () );
    draw_relation( painter, m_pAppOptions->getRelationList() );
}

void
CPainter::draw_figure_new()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    QPainter painter{ &m_pixmap };
             painter.setRenderHint( QPainter::Antialiasing, true );
             painter.fillRect( m_pixmap.rect(), m_pAppOptions->getBkgColor() );

    draw_figure( painter, m_pAppOptions->getClassList() );

    std::unique_ptr<Class_t> Item( CFigureFactory::buildFigure(
          m_pAppOptions->getFigureType()
        , m_pAppOptions->getFirstPos  ()
        , m_pAppOptions->getLastPos   ()
        , m_pAppOptions->getHeight    ()
        ) );
    draw_figure_item( painter, *Item.get() );

    draw_relation( painter, m_pAppOptions->getRelationList() );
}

void
CPainter::draw_relation_new()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    QPainter painter{ &m_pixmap };
             painter.setRenderHint( QPainter::Antialiasing, true );
             painter.fillRect( m_pixmap.rect(), m_pAppOptions->getBkgColor() );

    draw_figure  ( painter, m_pAppOptions->getClassList  () );
    draw_relation( painter, m_pAppOptions->getRelationList() );

    long nIndexFrom = m_pAppOptions->getIndexFrom();
    if( nIndexFrom >= 0 )
    {
        QPoint from;
        std::unique_ptr<Relation_t> Item( CFigureFactory::buildRelation(
              m_pAppOptions->getRelationType()
            , nIndexFrom
            , -1
            , m_pAppOptions->getClassList()->at( nIndexFrom )->center( &from )
            , m_pAppOptions->getLastPos()
            ) );
        draw_relation_item( painter, *Item.get() );
    }
}

void
CPainter::draw_relation( QPainter &painter, RelationList_t *RelationList )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    for_each( __EXECUTION_POLICY_PAINTER__, RelationList->begin(), RelationList->end(), [&]( auto &pItem ){ draw_relation_item( painter, *pItem ); } );
}

void
CPainter::draw_relation_line( QPainter &painter, QPen &pen, RelationPropertyType_t LineType, QPointF &startPoint, QPointF &endPoint )
{
    switch( LineType )
    {
        case RelationPropertyType_t::NonIdentifying : pen.setStyle( m_pAppOptions->getPenFKStyle() ); break;
        case RelationPropertyType_t::Identifying    : pen.setStyle( m_pAppOptions->getPenStyle  () ); break;
    }
    painter.setPen( pen );
    painter.drawLine( QLineF( startPoint, endPoint ) );
}

void
CPainter::draw_relation_cardinality( QPainter &painter, QPen &pen, RelationPropertyCardinality_t Cardinality, QPointF &startPoint, QPointF &endPoint, __PointType &PointType, float r, int d, qreal arrowSize )
{
    //if( m_pAppOptions->getRelationNotation() == RelationNotation_t::Bachman ){}

    // Bachman Notation
    pen.setStyle( m_pAppOptions->getPenStyle() );
    painter.setPen( pen );

    QLineF  line(startPoint, endPoint);
    double  angle = std::atan2(line.dy(), -line.dx()) + M_PI / 2;

    QPointF arrowP1_end = endPoint + QPointF(std::sin(angle + M_PI / d) * arrowSize,
                                             std::cos(angle + M_PI / d) * arrowSize);
    QPointF arrowP2_end = endPoint + QPointF(std::sin(angle - M_PI / d) * arrowSize,
                                             std::cos(angle - M_PI / d) * arrowSize);

    switch( Cardinality )
    {
        case RelationPropertyCardinality_t::ZeroOrMore : {
            QPolygonF arrowHead; arrowHead << endPoint << arrowP1_end << arrowP2_end;
            painter.drawPolygon(arrowHead);

            QPainterPath path; path.addPolygon(arrowHead);
            painter.fillPath(path, painter.pen().color()); // Fill arrowhead
            break;
        }
        case RelationPropertyCardinality_t::OneOrMore  : {
            QPolygonF arrowHead; arrowHead << endPoint << arrowP1_end << arrowP2_end;
            painter.drawPolygon(arrowHead);

            QPainterPath path1; path1.addPolygon(arrowHead);
            painter.fillPath(path1, painter.pen().color()); // Fill arrowhead

            QPainterPath path2; path2.addEllipse( PointType.m_nPoint, r, r );
            painter.fillPath(path2, painter.pen().color());
            break;
        }
        case RelationPropertyCardinality_t::ZeroOrOne  : {
            painter.drawEllipse( PointType.m_nPoint, r, r );
            break;
        }
        case RelationPropertyCardinality_t::ExactlyOne : {
            QPainterPath path; path.addEllipse( PointType.m_nPoint, r, r );
            painter.fillPath(path, painter.pen().color());
            break;
        }
    }

        // QLineF  line2(startPoint, endPoint);
        // double  angle2 = std::atan2(line2.dy(), -line2.dx()) + M_PI;

        // QPainterPath rectPath;
        // rectPath.addRect( QRectF( pFrom->m_nFirstPos, pFrom->m_nLastPos ) );

        // QPainterPath ellipsePath;
        // ellipsePath.addEllipse( pLineFrom.m_nPoint, 14, 6 );

        // QPointF    pt = pLineFrom.m_nPoint;
        // QTransform transform;
        // transform.translate( pt.x(), pt.y() );
        // transform.rotate( -qRadiansToDegrees( angle2 ) );
        // transform.translate( -pt.x(), -pt.y() );

        // painter.drawPath( ellipsePath.subtracted( rectPath ) );
        // //painter.drawPath( transform.map( ellipsePath.intersected( rectPath ) ) );
        // //painter.drawEllipse( pLineFrom.m_nPoint, 14, 6 );
}

void
CPainter::draw_relation_item( QPainter &painter, Relation_t &pRelation )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    painter.save();

    float nRatio = m_pAppOptions->getRatio();
    painter.scale( nRatio, nRatio );

    ClassList_t *figures = m_pAppOptions->getClassList();
    auto         pFrom   = figures->at( pRelation.m_nFrom );
    auto         pTo     = figures->at( pRelation.m_nTo   );
    QRectF       rc1{ pFrom->m_nFirstPos, pFrom->m_nLastPos };
    QRectF       rc2{ pTo  ->m_nFirstPos, pTo  ->m_nLastPos };
    QPointF      pt1 = pRelation.m_nFirstPos + QPointF(  pRelation.m_dX, -pRelation.m_dY );
    QPointF      pt2 = pRelation.m_nLastPos  + QPointF( -pRelation.m_dY,  pRelation.m_dX );
    QLineF       line( correct_point(pt1,rc1), correct_point(pt2,rc2) );

    float r         = 2.2 * m_pAppOptions->getCPRadius();
    int   d         = m_pAppOptions->getArrowAngle();
    qreal arrowSize = m_pAppOptions->getArrowSize();
    float nWidth    = pRelation.m_bHover ? m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();

    QPen     pen;
             pen.setWidthF( nWidth );
             pen.setColor ( m_pAppOptions->getPenColor() );
             pen.setStyle ( m_pAppOptions->getPenStyle() );
             pen.setCosmetic( true );
    painter.setPen( pen );

    QBrush   brush;
             brush.setColor( m_pAppOptions->getBrushColor() );
             brush.setStyle( m_pAppOptions->getBrushStyle() );
    painter.setBrush( brush );

    std::vector <__PointType> intersectionFrom; draw_relation_intersection( intersectionFrom, pFrom.get(), line );
    std::vector <__PointType> intersectionTo  ; draw_relation_intersection( intersectionTo  , pTo  .get(), line );

    //QPolygonF linePath;
    for_each( intersectionFrom.begin(), intersectionFrom.end(),
        [this,&painter,&intersectionTo,&pen,r,d,arrowSize,pFrom,pTo,&pRelation]( auto &pLineFrom )
        {
            if( pLineFrom.m_nIntersectionType == QLineF::BoundedIntersection )
            {
                for_each( intersectionTo.begin(), intersectionTo.end(),
                    [this,&painter,&pen,r,d,arrowSize,&pLineFrom,pFrom,pTo,&pRelation]( auto &pLineTo )
                    {
                         if( pLineTo.m_nIntersectionType == QLineF::BoundedIntersection )
                         {
                             QPointF startPoint = pLineFrom.m_nPoint, endPoint = pLineTo.m_nPoint;
                             draw_relation_line( painter, pen, pRelation.m_nType, startPoint, endPoint );
                             draw_relation_cardinality( painter, pen, pRelation.m_nCardinalityPKTable, startPoint, endPoint, pLineFrom, r, d, arrowSize );
                             draw_relation_cardinality( painter, pen, pRelation.m_nCardinalityFKTable, startPoint, endPoint, pLineTo  , r, d, arrowSize );
                         }
                    }
                );
            }
        }
    );

    painter.restore();
}

void
CPainter::draw_figure( QPainter &painter, ClassList_t *FigureList )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    for_each( __EXECUTION_POLICY_PAINTER__, FigureList->begin(), FigureList->end(), [&]( auto &pItem ){ draw_figure_item( painter, *pItem ); } );
}

void
CPainter::draw_figure_item( QPainter &painter, Class_t &pItem )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    painter.save();

    float nRatio = m_pAppOptions->getRatio();
    painter.scale( nRatio, nRatio );

    QRectF  rcf;
            rcf.setTopLeft    ( pItem.m_nFirstPos );
            rcf.setBottomRight( pItem.m_nLastPos  );
    QPointF center( rcf.center() );

    // begin figures >>>>>
    // 1. N-angles
    long n = pItem.m_nAnglesCount;
    QPolygonF polygon;
    double width  = abs( rcf.width () );
    double height = abs( rcf.height() );
    double radius = std::sqrt( width * width + height * height ) / 2;
    for( long i = 0; i < n; ++ i )
    {
        qreal fAngle = ( 2 * M_PI * i ) / n  - M_PI/2 - pItem.m_nAngle;
        qreal x = center.x() + cos( fAngle ) * radius;
        qreal y = center.y() + sin( fAngle ) * radius;
        polygon << QPoint( x, y );
    }
    // 2. ellipse
    QPainterPath ellipsePath;
    ellipsePath.addEllipse( rcf );
    // 3. rectangle
    QPainterPath rectPath;
    rectPath.addRect( rcf );
    // >>>>> end figures

    // rotate figures 2.and 3.
    QTransform   transform;
    transform.translate( rcf.center().x(), rcf.center().y() );
    transform.rotate( -qRadiansToDegrees( pItem.m_nAngle ) );
    transform.translate( -rcf.center().x(), -rcf.center().y() );

    float nWidth = (pItem.m_bHover || pItem.m_bSelect) ? m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();

    QPen     pen;
             pen.setWidthF( nWidth );
             pen.setColor ( pItem.m_bSelect ? m_pAppOptions->getPenColorSelect() : m_pAppOptions->getPenColor() );
             pen.setStyle ( m_pAppOptions->getPenStyle  () );
             pen.setCosmetic( true );
             painter.setPen( pen );

    QBrush   brush;
             brush.setColor( m_pAppOptions->getBrushColor() );
             brush.setStyle( m_pAppOptions->getBrushStyle() );
             painter.setBrush( brush );

    // begin paint figures >>>>>>
    switch( pItem.m_nFigureType )
    {
        case FigureTypeTriangle  :
            painter.drawPolygon( polygon );
            break;
        case FigureTypeEllipse   :
            painter.drawPath( transform.map( ellipsePath ) );
            break;
        case FigureTypeRectangle :
            painter.drawPath( transform.map( rectPath ) );
            break;
        case FigureTypeNone :
            break;
    }
    // >>>>>> end paint figures

    float r = m_pAppOptions->getCPRadius();

    // resize points
    float dx = 1.7;
    if( m_pAppOptions->getActionType() == ActionTypeMove )
    {
        draw_points( painter, pItem );
    }

    dx = 2.0;
    pen.setColor ( m_pAppOptions->getPenColor() );
    painter.setPen( pen );

    if( pItem.m_nFigureType == FigureTypeRectangle )
    {
        // without center point
    }
    else
    {
        nWidth = pItem.m_bHoverCenter ? dx*m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();
        pen.setWidthF( nWidth );
        painter.drawEllipse( rcf.center(), r, r );
    }

    if( m_pAppOptions->getSelectGroup() && m_pAppOptions->getFirstPos() != m_pAppOptions->getLastPos() )
    {
        draw_selected_area( painter );
    }

    if( !pItem.m_LogicalName.empty() || pItem.m_FieldList.size() > 0 )
    {
        pen.setWidthF( m_pAppOptions->getWidth() );
        painter.setPen( pen );

        QPointF center = rcf.center();
        painter.translate( center.x(), center.y() );
        painter.rotate( -qRadiansToDegrees( pItem.m_nAngle ) );
        painter.translate( -center.x(), -center.y() );

        draw_title ( painter, pItem );
        draw_fields( painter, pItem );

    }

    painter.restore();
}

void
CPainter::draw_points( QPainter &painter, Class_t &pItem )
{
    float nWidth = (pItem.m_bHover || pItem.m_bSelect) ? m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();

    QPen     pen;
    pen.setWidthF( nWidth );
    pen.setColor ( pItem.m_bSelect ? m_pAppOptions->getPenColorSelect() : m_pAppOptions->getPenColor() );
    pen.setStyle ( m_pAppOptions->getPenStyle  () );
    pen.setCosmetic( true );
    painter.setPen( pen );

    float r = m_pAppOptions->getCPRadius();

    float dx = 1.7;

    nWidth = pItem.m_bHoverFirst ? dx*m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();
    pen.setWidthF( nWidth );
    painter.setPen( pen );
    painter.drawEllipse( QPointF(pItem.m_nFirstPos), r, r );

    nWidth = pItem.m_bHoverLast ? dx*m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();
    pen.setWidthF( nWidth );
    painter.setPen( pen );
    painter.drawEllipse( QPointF(pItem.m_nLastPos), r, r );

    dx = 2.0;
    nWidth = pItem.m_bHoverAngle ? dx*m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();
    pen.setWidthF( nWidth );
    pen.setColor ( m_pAppOptions->getPenColorRotate() );
    painter.setPen( pen );
    painter.drawEllipse( QPointF(pItem.m_nAnglePos ), r, r );

    nWidth = pItem.m_bHoverResize ? dx*m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();
    pen.setWidthF( nWidth );
    pen.setColor ( m_pAppOptions->getPenColorResize() );
    painter.setPen( pen );
    painter.drawEllipse( QPointF(pItem.m_nResizePos ), r, r );
}

void
CPainter::draw_selected_area( QPainter &painter )
{
    QRect  rc_selected;
    rc_selected.setTopLeft    ( m_pAppOptions->getFirstPos() );
    rc_selected.setBottomRight( m_pAppOptions->getLastPos ()  );

    QPen   pen_selected;
    pen_selected.setWidthF( m_pAppOptions->getWidth() );
    pen_selected.setColor ( m_pAppOptions->getPenColorSelect() );
    pen_selected.setStyle ( m_pAppOptions->getPenSelectStyle() );
    pen_selected.setCosmetic( true );

    QBrush brush_selected;

    painter.setPen( pen_selected );
    painter.setBrush( brush_selected );
    painter.drawRect( rc_selected );
}

void
CPainter::draw_title_border( QPainter &painter, Class_t &pClass )
{
    float nWidth = (pClass.m_bHover || pClass.m_bSelect) ? m_pAppOptions->getWidthHover() : m_pAppOptions->getWidth();

    QRectF       rcf;
                 rcf.setTopLeft    ( pClass.m_nFirstPos );
                 rcf.setBottomRight( pClass.m_nTitlePos );
    QBrush       brush;
                 brush.setColor( m_pAppOptions->getBrushTitleColor() ); // m_pAppOptions->getBrushColor()
                 //brush.setColor( hex2color( m_pAppOptions->getBrushTitleColor() ) ); // m_pAppOptions->getBrushColor()
                 brush.setStyle( m_pAppOptions->getBrushTitleStyle() );
    QPen         pen;
                 pen.setWidthF( nWidth );
                 pen.setColor ( pClass.m_bSelect ? m_pAppOptions->getPenColorSelect() : m_pAppOptions->getPenColor() );
                 pen.setStyle ( m_pAppOptions->getPenStyle() );
                 pen.setCosmetic( true );
    QPainterPath rectPath;
                 rectPath.addRect( rcf );

    painter.setBrush( brush );
    painter.setPen( pen );
    painter.drawPath( rectPath );
}

void
CPainter::draw_title( QPainter &painter, Class_t &pClass )
{
    if( pClass.m_nFigureType == FigureTypeRectangle )
    {
        draw_title_border( painter, pClass );
        draw_row( painter, pClass.m_nFirstPos, pClass.m_nTitlePos, pClass.m_LogicalName, FieldRelationTypeNone, true, true );
    }
    else
    {
        auto      fm = painter.fontMetrics();
        int   h = fm.height();
        int   w = fm.horizontalAdvance( pClass.m_LogicalName.c_str() );

        QPoint center    = QRect( pClass.m_nFirstPos, pClass.m_nLastPos ).center();
        QPoint nFirstPos = center + QPoint( -w/2,  h );
        QPoint nLastPos  = center + QPoint(  w/2,  0 );
        draw_row( painter, nFirstPos, nLastPos, pClass.m_LogicalName, FieldRelationTypeNone, false, true );
    }
}

void
CPainter::draw_fields( QPainter &painter, Class_t &pClass )
{
    if( pClass.m_nFigureType == FigureTypeRectangle )
    {
        FieldList_t PK; pClass.enumFields( PK, FieldRelationTypePrimaryKey );
        FieldList_t FK; pClass.enumFields( FK, FieldRelationTypeForeignKey );
        FieldList_t NN; pClass.enumFields( NN, FieldRelationTypeNone       );

        std::for_each( PK.begin(), PK.end(), [&]( auto &pField ) { draw_field( painter, pClass, *pField, pField == PK.back() ); } );
        std::for_each( FK.begin(), FK.end(), [&]( auto &pField ) { draw_field( painter, pClass, *pField, pField == FK.back() ); } );
        std::for_each( NN.begin(), NN.end(), [&]( auto &pField ) { draw_field( painter, pClass, *pField, false ); } );
    }
}

void
CPainter::draw_field( QPainter &painter, Class_t &, Field_t &pField, bool bLastItem )
{
    draw_row( painter, pField.m_nFirstPos, pField.m_nLastPos, pField.m_LogicalName, pField.m_nFieldRelationType, bLastItem, pField.m_bHover );
}

void
CPainter::draw_row( QPainter &painter, QPoint &pt0, QPoint &pt1, std::string &Name, FieldRelationType_t Type, bool bShowLine, bool bFontBold )
{
    std::string buff;
    switch( Type )
    {
        case FieldRelationTypePrimaryKey : buff = "[pk] "; break;
        case FieldRelationTypeForeignKey : buff = "[fk] "; break;
        case FieldRelationTypeNone       : break;
    }
    buff += Name;

    QFont font = painter.font();
    if( bFontBold )
    {
        font.setWeight( QFont::Bold );
    }
    else
    {
        font.setWeight( QFont::Normal );
    }
    painter.setFont( font );

    int padding = DEFAULT_PADDING;
    //auto fm = painter.fontMetrics();
    //int w = fm.horizontalAdvance( Name.c_str() );
    int x1 = pt0.x();
    int y1 = pt1.y();
    int x2 = pt1.x();
    int x0 = x1 + padding;
    int y0 = y1 - padding;
    painter.drawText( x0, y0, buff.c_str() );
    if( bShowLine )
    {
        painter.drawLine( x1, y1, x2, y1 );
    }
}
