/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include "Options.h"

#include <QJsonObject>
#include <QJsonDocument>

#include "common/Logger.h"
#include "common/IOStreamer.h"

#include "data/general/Class.h"

#include "data/shapes/FigureFactory.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
#define __AppOptions (*__AppOptions__.get())

void CAppOptions::init()
{
    m_nAppVariant       = APP_VARIANT;
    m_ProjectName       = "";
    //m_DatabaseName      = "";
    m_TablePrefix       = "";
    m_ClassPrefix       = "";
    m_nWidth            = 1.0;
    m_nHeight           = 12;
    m_nPadding          = m_nHeight / HEIGHT_TO_PADDING;
    m_nWidthHover       = 3.0;
    m_PenColor          = "#000000";
    m_PenColorSelect    = "#A72920";
    m_PenColorRotate    = Qt::darkGreen;
    m_PenColorResize    = Qt::darkBlue;
    m_BrushColor        = Qt::green;
    m_BkgColor          = Qt::white;
    m_PenStyle          = Qt::SolidLine;
    m_PenFKStyle        = Qt::DashLine;
    m_PenSelectStyle    = Qt::DotLine;
    m_ArrowType         = Qt::NoArrow;
    m_BrushStyle        = Qt::NoBrush;  //FDiagPattern, CrossPattern, DiagCrossPattern
    m_BrushTitleStyle   = Qt::SolidPattern;
    m_BrushTitleColor   = "#D0F0C0"; // #D0F0C0 (lt.green) #FDE6E4 (rose) #7C9D46 (olive) #F8CE1C, #A7BCD6 #4F868E #A84B9C #A72920
    m_nCPRadius         = 2.0;
    m_nFigureType       = FigureType_t::FigureTypeRectangle;
    m_nActionType       = ActionType_t::ActionTypeAddFigure;
    m_nIndexFrom        = -1L;
    m_nRelationType     = RelationType_t::RelationTypeLineNondirect;
    m_nArrowAngle       = 10;
    m_nArrowSize        = 15;
    m_bChanged          = false;
    m_bSelectGroup      = false;
    m_nRatio            = DEFAULT_RATIO;
    m_nRelationNotation = RelationNotation_t::Bachman;

    //m_nDatabaseType     = DatabaseType_t::DatabaseTypeMySQL;
    m_nDB               = PDatabases::ID_PDATABASES_NONE;
    m_ServerName        = "";
    m_Port              = "";
    m_BaseName          = "";
    m_UserName          = "";
    m_UserPass          = "";
    m_SysAdmin          = "";
    m_hProvider         = 0;//SQL_NULL_HANDLE;

    clearPos();

    m_ClassList   .Flush();
    m_RelationList.Flush();
}

CAppOptions&
CAppOptions::operator = ( const CAppOptions &o )
{
    m_nAppVariant       = o.m_nAppVariant      ;
    m_ProjectName       = o.m_ProjectName      ;
    //m_DatabaseName      = o.m_DatabaseName     ;
    m_TablePrefix       = o.m_TablePrefix      ;
    m_ClassPrefix       = o.m_ClassPrefix      ;
    m_nWidth            = o.m_nWidth           ;
    m_nHeight           = o.m_nHeight          ;
    m_nPadding          = o.m_nPadding         ;
    m_nWidthHover       = o.m_nWidthHover      ;
    m_PenColor          = o.m_PenColor         ;
    m_PenColorRotate    = o.m_PenColorRotate   ;
    m_PenColorResize    = o.m_PenColorResize   ;
    m_PenColorSelect    = o.m_PenColorSelect   ;
    m_BrushColor        = o.m_BrushColor       ;
    m_BkgColor          = o.m_BkgColor         ;
    m_PenStyle          = o.m_PenStyle         ;
    m_PenFKStyle        = o.m_PenFKStyle       ;
    m_PenSelectStyle    = o.m_PenSelectStyle   ;
    m_ArrowType         = o.m_ArrowType        ;
    m_BrushStyle        = o.m_BrushStyle       ;
    m_BrushTitleStyle   = o.m_BrushTitleStyle  ;
    m_BrushTitleColor   = o.m_BrushTitleColor  ;
    m_nCPRadius         = o.m_nCPRadius        ;
    m_nFigureType       = o.m_nFigureType      ;
    m_nActionType       = o.m_nActionType      ;
    m_nIndexFrom        = o.m_nIndexFrom       ;
    m_nRelationType     = o.m_nRelationType    ;
    m_nArrowAngle       = o.m_nArrowAngle      ;
    m_nArrowSize        = o.m_nArrowSize       ;
    m_bChanged          = o.m_bChanged         ;
    m_bSelectGroup      = o.m_bSelectGroup     ;
    m_nRatio            = o.m_nRatio           ;
    m_nRelationNotation = o.m_nRelationNotation;
    m_nFirstPos         = o.m_nFirstPos        ;
    m_nLastPos          = o.m_nLastPos         ;
    m_ClassList         = o.m_ClassList        ;
    m_RelationList      = o.m_RelationList     ;

    //m_nDatabaseType     = o.m_nDatabaseType    ;
    m_nDB               = o.m_nDB              ;
    m_ServerName        = o.m_ServerName       ;
    m_Port              = o.m_Port             ;
    m_BaseName          = o.m_BaseName         ;
    m_UserName          = o.m_UserName         ;
    m_UserPass          = o.m_UserPass         ;
    m_SysAdmin          = o.m_SysAdmin         ;
    m_hProvider         = o.m_hProvider        ;
    return *this;
}

CAppOptions&
CAppOptions::class_relation_copy()
{
    for( long it = (long)getClassList()->size() - 1; it >= 0; -- it  )
    {
        auto pItem = getClassList()->at( it );
        if( !pItem->m_bSelect )
        {
            class_relation_delete( it );
        }
    }
    return *this;
}

void
CAppOptions::class_relation_paste( CAppOptions &from )
{
    unsigned long delta = m_ClassList.size();

    for( unsigned long it = 0u, size = from.getClassList()->size(); it < size; ++ it )
    {
        auto &pItem = from.getClassList()->at( it );
        m_ClassList.push_back( pItem );
    }

    for( unsigned long it = 0u, size = from.getRelationList()->size(); it < size; ++ it )
    {
        auto &pItem = from.getRelationList()->at( it );
        pItem->m_nFrom += delta;
        pItem->m_nTo   += delta;
        m_RelationList.push_back( pItem );
    }
}

bool
CAppOptions::class_relation_delete( long n )
{
    if( n >= 0 )
    {
        auto CL = getClassList();
        if( n < (long)CL->size() )
        {
            auto RL = getRelationList();

            for( long i = RL->size() - 1; i >= 0; i -- )
            {
                auto &p = RL->at( i );
                if( p->m_nFrom == (long)n || p->m_nTo == (long)n )
                {
                    RL->Delete( i );
                }
            }

            for( unsigned long j = n + 1; j < CL->size(); ++ j )
                for( long jj = RL->size() - 1; jj >= 0; -- jj )
                {
                    auto &p = RL->at( jj );
                    if( p->m_nFrom == (long)j ) -- p->m_nFrom;
                    if( p->m_nTo   == (long)j ) -- p->m_nTo  ;
                }

            CL->Delete( n );
            return true;
        }
    }
    return false;
}

void
CAppOptions::clearState( bool bWithSelect )
{
    m_nIndexFrom = -1L;
    m_bChanged   = false;
    if( bWithSelect )
    {
        m_ClassList.select_clear();
    }
    m_bSelectGroup = false;
    clearPos();
}

void
CAppOptions::clearPos()
{
    m_nFirstPos.setX( 0 ); m_nFirstPos.setY( 0 );
    m_nLastPos .setX( 0 ); m_nLastPos .setY( 0 );
}

FigureType_t
CAppOptions::rotateFigureType( FigureType_t nFigureType )
{
    switch( nFigureType )
    {
        case FigureTypeNone      : break;
        case FigureTypeTriangle  : return FigureTypeTriangle ;
        case FigureTypeEllipse   : return FigureTypeRectangle;
        case FigureTypeRectangle : return FigureTypeEllipse  ;
    }
    return FigureTypeNone;
}


RelationType_t
CAppOptions::rotateRelationType( RelationType_t nRelationType )
{
    switch( nRelationType )
    {
        case RelationTypeNone            : break;
        case RelationTypeLineNondirect   : return RelationTypeLineBidirect   ;
        case RelationTypeLineBidirect    : return RelationTypeLineDirectLeft ;
        case RelationTypeLineDirectLeft  : return RelationTypeLineDirectRight;
        case RelationTypeLineDirectRight : return RelationTypeLineNondirect  ;
    }
    return RelationTypeNone;
}

QJsonObject& Write( QJsonObject &ios, CAppOptions &obj )
{
    //QJsonObject m_nFirstPos   ; ::Write( m_nFirstPos   , obj.m_nFirstPos    );
    //QJsonObject m_nLastPos    ; ::Write( m_nLastPos    , obj.m_nLastPos     );
    QJsonObject m_ClassList   ; ::Write( m_ClassList   , obj.m_ClassList    );
    QJsonObject m_RelationList; ::Write( m_RelationList, obj.m_RelationList );

    ios[ "m_nAppVariant"      ] =      obj.m_nAppVariant      ;
    ios[ "m_nWidth"           ] =      obj.m_nWidth           ;
    ios[ "m_nWidthHover"      ] =      obj.m_nWidthHover      ;
    ios[ "m_PenColor"         ] =      obj.m_PenColor         ;
    ios[ "m_PenColorSelect"   ] =      obj.m_PenColorSelect   ;
    ios[ "m_PenColorRotate"   ] = (int)obj.m_PenColorRotate   ;
    ios[ "m_PenColorResize"   ] = (int)obj.m_PenColorResize   ;
    ios[ "m_BrushColor"       ] = (int)obj.m_BrushColor       ;
    ios[ "m_BkgColor"         ] = (int)obj.m_BkgColor         ;
    ios[ "m_PenStyle"         ] = (int)obj.m_PenStyle         ;
    ios[ "m_PenFKStyle"       ] = (int)obj.m_PenFKStyle       ;
    ios[ "m_ArrowType"        ] =      obj.m_ArrowType        ;
    ios[ "m_BrushStyle"       ] = (int)obj.m_BrushStyle       ;
    ios[ "m_BrushTitleStyle"  ] = (int)obj.m_BrushTitleStyle  ;
    ios[ "m_BrushTitleColor"  ] =      obj.m_BrushTitleColor  ;
    ios[ "m_nCPRadius"        ] =      obj.m_nCPRadius        ;
    ios[ "m_nFigureType"      ] =      obj.m_nFigureType      ;
    ios[ "m_nActionType"      ] =      obj.m_nActionType      ;
    ios[ "m_nRelationType"    ] =      obj.m_nRelationType    ;
    //ios[ "m_nIndexFrom"      ] = (int)obj.m_nIndexFrom       ;
    //ios[ "m_nArrowAngle"     ] =      obj.m_nArrowAngle      ;
    //ios[ "m_nArrowSize"      ] =      obj.m_nArrowSize       ;
    //ios[ "m_nDatabaseType"    ] =      obj.m_nDatabaseType    ;
    //ios[ "m_nRatio"          ] =      obj.m_nRatio           ;
    ios[ "m_ProjectName"      ] =      obj.m_ProjectName      ;
    //ios[ "m_DatabaseName"     ] =      obj.m_DatabaseName     ;
    ios[ "m_TablePrefix"      ] =      obj.m_TablePrefix      ;
    ios[ "m_ClassPrefix"      ] =      obj.m_ClassPrefix      ;
    ios[ "m_nRelationNotation"] =      obj.m_nRelationNotation;
    //ios[ "m_nFirstPos"       ] =          m_nFirstPos        ;
    //ios[ "m_nLastPos"        ] =          m_nLastPos         ;

    ios[ "m_nDB"              ] = obj.m_nDB                   ;
    ios[ "m_ServerName"       ] = obj.m_ServerName            ;
    ios[ "m_Port"             ] = obj.m_Port                  ;
    ios[ "m_BaseName"         ] = obj.m_BaseName              ;
    ios[ "m_UserName"         ] = obj.m_UserName              ;
    ios[ "m_UserPass"         ] = obj.m_UserPass              ;
    ios[ "m_SysAdmin"         ] = obj.m_SysAdmin              ;

    ios[ "m_ClassList"        ] =          m_ClassList        ;
    ios[ "m_RelationList"     ] =          m_RelationList     ;

    return ios;
}

CAppOptions& Read( QJsonObject &ios, CAppOptions &obj, long version )
{
    obj.        m_nAppVariant      =                     ios[ "m_nAppVariant"      ].toInt();
    obj.        m_nWidth           =                     ios[ "m_nWidth"           ].toInt();
    obj.        m_nWidthHover      =                     ios[ "m_nWidthHover"      ].toInt();
    obj.        m_PenColor         =                     ios[ "m_PenColor"         ].toString();
    obj.        m_PenColorSelect   =                     ios[ "m_PenColorSelect"   ].toString();
    obj.        m_PenColorRotate   = (Qt::GlobalColor)   ios[ "m_PenColorRotate"   ].toInt();
    obj.        m_PenColorResize   = (Qt::GlobalColor)   ios[ "m_PenColorResize"   ].toInt();
    obj.        m_BrushColor       = (Qt::GlobalColor)   ios[ "m_BrushColor"       ].toInt();
    obj.        m_BkgColor         = (Qt::GlobalColor)   ios[ "m_BkgColor"         ].toInt();
    obj.        m_PenStyle         = (Qt::PenStyle   )   ios[ "m_PenStyle"         ].toInt();
    obj.        m_PenFKStyle       = (Qt::PenStyle   )   ios[ "m_PenFKStyle"       ].toInt();
    obj.        m_ArrowType        = (Qt::ArrowType  )   ios[ "m_ArrowType"        ].toInt();
    obj.        m_BrushStyle       = (Qt::BrushStyle )   ios[ "m_BrushStyle"       ].toInt();
    obj.        m_BrushTitleStyle  = (Qt::BrushStyle )   ios[ "m_BrushTitleStyle"  ].toInt();
    obj.        m_BrushTitleColor  =                     ios[ "m_BrushTitleColor"  ].toString();
    obj.        m_nCPRadius        =                     ios[ "m_nCPRadius"        ].toInt();
    obj.        m_nFigureType      = (FigureType_t   )   ios[ "m_nFigureType"      ].toInt();
    obj.        m_nActionType      = (ActionType_t   )   ios[ "m_nActionType"      ].toInt();
    obj.        m_nRelationType    = (RelationType_t )   ios[ "m_nRelationType"    ].toInt();
    //obj.        m_nIndexFrom       =                     ios[ "m_nIndexFrom"       ].toInt();
    //obj.        m_nArrowAngle      =                     ios[ "m_nArrowAngle"      ].toInt();
    //obj.        m_nArrowSize       =                     ios[ "m_nArrowSize"       ].toInt();
    //obj.        m_nDatabaseType    = (DatabaseType_t )   ios[ "m_nDatabaseType"    ].toInt();
    //obj.        m_nRatio           =                     ios[ "m_nRatio"           ].toDouble();
    obj.        m_ProjectName      =                     ios[ "m_ProjectName"      ].toString();
    //obj.        m_DatabaseName     =                     ios[ "m_DatabaseName"     ].toString();
    obj.        m_TablePrefix      =                     ios[ "m_TablePrefix"      ].toString();
    obj.        m_ClassPrefix      =                     ios[ "m_ClassPrefix"      ].toString();
    obj.        m_nRelationNotation= (RelationNotation_t)ios[ "m_nRelationNotation"].toInt();
    //QJsonObject m_nFirstPos        =                     ios[ "m_nFirstPos"        ].toObject();
    //QJsonObject m_nLastPos         =                     ios[ "m_nLastPos"         ].toObject();

    obj.        m_nDB              = (PDatabases)        ios[ "m_nDB"        ].toInt();
    obj.        m_ServerName       =                     ios[ "m_ServerName" ].toString();
    obj.        m_Port             =                     ios[ "m_Port"       ].toString();
    obj.        m_BaseName         =                     ios[ "m_BaseName"   ].toString();
    obj.        m_UserName         =                     ios[ "m_UserName"   ].toString();
    obj.        m_UserPass         =                     ios[ "m_UserPass"   ].toString();
    obj.        m_SysAdmin         =                     ios[ "m_SysAdmin"   ].toString();

    //::Read( m_nFirstPos   , obj.m_nFirstPos   , version );
    //::Read( m_nLastPos    , obj.m_nLastPos    , version );

    QJsonObject m_ClassList        =                     ios[ "m_ClassList"        ].toObject();
    QJsonObject m_RelationList     =                     ios[ "m_RelationList"     ].toObject();
    ::Read( m_ClassList   , obj.m_ClassList   , version );
    ::Read( m_RelationList, obj.m_RelationList, version );
    return obj;
}

void Write( CIOStreamer &ios, CAppOptions &obj )
{
    ::Write   ( ios, obj.m_nAppVariant       );
    ::Write   ( ios, obj.m_nWidth            );
    ::Write   ( ios, obj.m_nWidthHover       );
    ::Write   ( ios, obj.m_PenColor          );
    ::Write   ( ios, obj.m_PenColorSelect    );
    ::WriteInt( ios, obj.m_PenColorRotate    );
    ::WriteInt( ios, obj.m_PenColorResize    );
    ::WriteInt( ios, obj.m_BrushColor        );
    ::WriteInt( ios, obj.m_BkgColor          );
    ::WriteInt( ios, obj.m_PenStyle          );
    ::WriteInt( ios, obj.m_PenFKStyle        );
    ::WriteInt( ios, obj.m_ArrowType         );
    ::WriteInt( ios, obj.m_BrushStyle        );
    ::WriteInt( ios, obj.m_BrushTitleStyle   );
    ::Write   ( ios, obj.m_BrushTitleColor   );
    ::Write   ( ios, obj.m_nCPRadius         );
    ::WriteInt( ios, obj.m_nFigureType       );
    //::Write   ( ios, obj.m_nFirstPos         );
    //::Write   ( ios, obj.m_nLastPos          );
    ::WriteInt( ios, obj.m_nActionType       );
    ::WriteInt( ios, obj.m_nRelationType     );
    ::Write   ( ios, obj.m_nIndexFrom        );
    ::Write   ( ios, obj.m_nArrowAngle       );
    ::Write   ( ios, obj.m_nArrowSize        );
    //::Write   ( ios, obj.m_nDatabaseType     );
    ::Write   ( ios, obj.m_nRatio            );
    ::Write   ( ios, obj.m_ProjectName       );
    //::Write   ( ios, obj.m_DatabaseName      );
    ::Write   ( ios, obj.m_TablePrefix       );
    ::Write   ( ios, obj.m_ClassPrefix       );
    ::Write   ( ios, obj.m_nRelationNotation );
    ::WriteInt( ios, obj.m_nDB               );
    ::Write   ( ios, obj.m_ServerName        );
    ::Write   ( ios, obj.m_Port              );
    ::Write   ( ios, obj.m_BaseName          );
    ::Write   ( ios, obj.m_UserName          );
    ::Write   ( ios, obj.m_UserPass          );
    ::Write   ( ios, obj.m_SysAdmin          );

    ::Write   ( ios, obj.m_ClassList         );
    ::Write   ( ios, obj.m_RelationList      );
}


CAppOptions& Read( CIOStreamer &ios, CAppOptions &obj, long version )
{
    ::Read   ( ios, obj.m_nAppVariant             );
    ::Read   ( ios, obj.m_nWidth                  );
    ::Read   ( ios, obj.m_nWidthHover             );
    ::Read   ( ios, obj.m_PenColor                );
    ::Read   ( ios, obj.m_PenColorSelect          );
    ::ReadInt( ios, obj.m_PenColorRotate          );
    ::ReadInt( ios, obj.m_PenColorResize          );
    ::ReadInt( ios, obj.m_BrushColor              );
    ::ReadInt( ios, obj.m_BkgColor                );
    ::ReadInt( ios, obj.m_PenStyle                );
    ::ReadInt( ios, obj.m_PenFKStyle              );
    ::ReadInt( ios, obj.m_ArrowType               );
    ::ReadInt( ios, obj.m_BrushStyle              );
    ::ReadInt( ios, obj.m_BrushTitleStyle         );
    ::Read   ( ios, obj.m_BrushTitleColor         );
    ::Read   ( ios, obj.m_nCPRadius               );
    ::ReadInt( ios, obj.m_nFigureType             );
    //::Read   ( ios, obj.m_nFirstPos     , version );
    //::Read   ( ios, obj.m_nLastPos      , version );
    ::ReadInt( ios, obj.m_nActionType             );
    ::ReadInt( ios, obj.m_nRelationType           );
    ::Read   ( ios, obj.m_nIndexFrom              );
    ::Read   ( ios, obj.m_nArrowAngle             );
    ::Read   ( ios, obj.m_nArrowSize              );
    //::Read   ( ios, obj.m_nDatabaseType           );
    ::Read   ( ios, obj.m_nRatio                  );
    ::Read   ( ios, obj.m_ProjectName             );
    //::Read   ( ios, obj.m_DatabaseName            );
    ::Read   ( ios, obj.m_TablePrefix             );
    ::Read   ( ios, obj.m_ClassPrefix             );
    ::Read   ( ios, obj.m_nRelationNotation       );
    ::ReadInt( ios, obj.m_nDB                     );
    ::Read   ( ios, obj.m_ServerName              );
    ::Read   ( ios, obj.m_Port                    );
    ::Read   ( ios, obj.m_BaseName                );
    ::Read   ( ios, obj.m_UserName                );
    ::Read   ( ios, obj.m_UserPass                );
    ::Read   ( ios, obj.m_SysAdmin                );

    ::Read   ( ios, obj.m_ClassList     , version );
    ::Read   ( ios, obj.m_RelationList  , version );
    return obj;
}
