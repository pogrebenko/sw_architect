/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef CONSTS_H
#define CONSTS_H

#define APP_COMPANY            "pogrebenko"
#define APP_NAME               "Software Architect"
#define APP_VERSION            "1.0.0"

#define APP_DATA_VERSION       0

#define APP_VARIANT_FULL       1
#define APP_VARIANT_DEMO       2
#define APP_VARIANT            APP_VARIANT_DEMO

#define MAX_NEAR_SIZE         10
#define MIN_FIGURE_SIZE       10
#define MAX_FIGURE_SIZE      500
#define DEFAULT_TOLERANCE     10.
#define DELTA_ANGLE           10.
#define DELTA_RESIZE          10.
#define HEIGHT_TO_PADDING      4
#define DEFAULT_PADDING        7
#define DEFAULT_SCROLL_SIZE   16
#define DEFAULT_OFFSET_DX     30
#define DEFAULT_OFFSET_DY     30

#define DEFAULT_RATIO_STEP     0.1
#define PRECISION_RATIO       10
#define MAXIMUM_RATIO          1.6
#define MINIMUM_RATIO          0.4
#define DEFAULT_RATIO          1.
#define MINIMUM_RELATION_WIDTH 8

#define MAXRECENTFILE          5

#define ADD_DEFAULT_WIDTH    100
#define ADD_DEFAULT_HEIGHT   120

#define DEFAULT_PRINTER_DPI  300

#define DEFAULT_MSG_TIMEOUT 3000

#define __SMART__

#include <execution>

#define __EXECUTION_POLICY_BUILDER__ std::execution::par
#define __EXECUTION_POLICY_PAINTER__ std::execution::seq

typedef enum {
    ExportTypeNone              = 0,
    ExportTypeBin               = 1,
    ExportTypeJSON              = 2,
} FormatFileType_t;

#define __EXPORT_FORMAT__ ExportTypeJSON

typedef enum {
    ActionTypeNone              = 0,
    ActionTypeAddFigure         = 1,
    ActionTypeAddRelation       = 2,
    ActionTypeMove              = 3,
    ActionTypeDelete            = 4,
} ActionType_t;

typedef enum {
    DatabaseTypeNone            = 0,
    DatabaseTypeMySQL           = 1,
} DatabaseType_t;

typedef enum {
    FigureTypeNone              = 0,
    FigureTypeTriangle          = 1,
    FigureTypeEllipse           = 2,
    FigureTypeRectangle         = 3,
} FigureType_t;

typedef enum {
    FieldRelationTypeNone       = 0,
    FieldRelationTypePrimaryKey = 1,
    FieldRelationTypeForeignKey = 2,
} FieldRelationType_t;

typedef enum {
    FieldDataTypeNone           = 0,
    FieldDataTypeInteger        = 1,
    FieldDataTypeDecimal        = 2,
    FieldDataTypeTimestamp      = 3,
    FieldDataTypeTime           = 4,
    FieldDataTypeDate           = 5,
    FieldDataTypeBlob           = 6,
    FieldDataTypeVarchar        = 7,
    FieldDataTypeChar           = 8,
} FieldDataType_t;

typedef enum {
    RelationTypeNone            = 0,
    RelationTypeLineNondirect   = 1,
    RelationTypeLineBidirect    = 2,
    RelationTypeLineDirectLeft  = 3,
    RelationTypeLineDirectRight = 4,
} RelationType_t;

typedef enum {
    Cascade                     = 0,
    Restrict                    = 1,
    NoAction                    = 2,
    SetNull                     = 3,
    SetDefault                  = 4
} RelationPropertyRule_t;

typedef enum {
    NotDeferrability             = 0,
    DeferrableInitiallyDeferred  = 1,
    DeferrableInitiallyImmediate = 2
} RelationPropertyDeferrability_t;

typedef enum {
    ZeroOrMore                   = 0,
    OneOrMore                    = 1,
    ZeroOrOne                    = 2,
    ExactlyOne                   = 3,
} RelationPropertyCardinality_t;

typedef enum {
    NonIdentifying               = 0,
    Identifying                  = 1
} RelationPropertyType_t;

typedef enum {
    Bachman                      = 0,
    MinMax                       = 1,
    CrowsFoot                    = 2,
} RelationNotation_t;


#endif // CONSTS_H
