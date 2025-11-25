/*
Project:Software Architect
File: RelationDetails.cpp
Licensee: MIT
Source: https://github.com/pogrebenko/sw_architect
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "RelationDetails.h"

#include "data/general/Relation.h"

CRelationDetails::CRelationDetails( const QString &title, __Relation *pItem, QWidget *parent )
: QDialog( parent )
, m_pItem( pItem  )
{
    Label_Name         = new QLabel   ( "Name:"           );
    Edit_Name          = new QLineEdit();

    Label_PKTableLabel = new QLabel   ( "PK Table Label:" );
    Edit_PKTableLabel  = new QLineEdit();

    Label_FKTableLabel = new QLabel   ( "FK Table Label:" );
    Edit_FKTableLabel  = new QLineEdit();

    Label_nType                      = new QLabel   ( "Type:"                  );
    ComboBox_nType                   = new QComboBox();

    Label_nCardinalityPKTable        = new QLabel   ( "Cardinality PK Table:"  );
    ComboBox_nCardinalityPKTable     = new QComboBox();

    Label_nCardinalityFKTable        = new QLabel   ( "Cardinality FK Table:"  );
    ComboBox_nCardinalityFKTable     = new QComboBox();

    Label_nDeferrability             = new QLabel   ( "Deferrability:"         );
    ComboBox_nDeferrability          = new QComboBox();

    Label_nUpdateRule                = new QLabel   ( "Update Rule:"           );
    ComboBox_nUpdateRule             = new QComboBox();

    Label_nDeleteRule                = new QLabel   ( "Delete Rule:"           );
    ComboBox_nDeleteRule             = new QComboBox();

    buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &CRelationDetails::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CRelationDetails::reject);

    int n = 0;
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget( Label_Name                  , n  , 0 );
    mainLayout->addWidget( Edit_Name                   , n++, 1 );
    mainLayout->addWidget( Label_PKTableLabel          , n  , 0 );
    mainLayout->addWidget( Edit_PKTableLabel           , n++, 1 );
    mainLayout->addWidget( Label_FKTableLabel          , n  , 0 );
    mainLayout->addWidget( Edit_FKTableLabel           , n++, 1 );
    mainLayout->addWidget( Label_nType                 , n  , 0 );
    mainLayout->addWidget( ComboBox_nType              , n++, 1 );
    mainLayout->addWidget( Label_nCardinalityPKTable   , n  , 0 );
    mainLayout->addWidget( ComboBox_nCardinalityPKTable, n++, 1 );
    mainLayout->addWidget( Label_nCardinalityFKTable   , n  , 0 );
    mainLayout->addWidget( ComboBox_nCardinalityFKTable, n++, 1 );
    mainLayout->addWidget( Label_nDeferrability        , n  , 0 );
    mainLayout->addWidget( ComboBox_nDeferrability     , n++, 1 );
    mainLayout->addWidget( Label_nUpdateRule           , n  , 0 );
    mainLayout->addWidget( ComboBox_nUpdateRule        , n++, 1 );
    mainLayout->addWidget( Label_nDeleteRule           , n  , 0 );
    mainLayout->addWidget( ComboBox_nDeleteRule        , n++, 1 );

    mainLayout->addWidget( buttonBox, n, 0, 1, 2, Qt::AlignCenter );
    setLayout(mainLayout);

    ComboBox_nType              ->addItem( "Non-Identifying", RelationPropertyType_t::NonIdentifying   );
    ComboBox_nType              ->addItem( "Identifying    ", RelationPropertyType_t::Identifying      );

    ComboBox_nCardinalityPKTable->addItem( "Zero or More", RelationPropertyCardinality_t::ZeroOrMore );
    ComboBox_nCardinalityPKTable->addItem( "One or More ", RelationPropertyCardinality_t::OneOrMore  );
    ComboBox_nCardinalityPKTable->addItem( "Zero or One ", RelationPropertyCardinality_t::ZeroOrOne  );
    ComboBox_nCardinalityPKTable->addItem( "Exactly One ", RelationPropertyCardinality_t::ExactlyOne );

    ComboBox_nCardinalityFKTable->addItem( "Zero or More", RelationPropertyCardinality_t::ZeroOrMore );
    ComboBox_nCardinalityFKTable->addItem( "One or More ", RelationPropertyCardinality_t::OneOrMore  );
    ComboBox_nCardinalityFKTable->addItem( "Zero or One ", RelationPropertyCardinality_t::ZeroOrOne  );

    ComboBox_nDeferrability     ->addItem( "NotDeferrability            ", RelationPropertyDeferrability_t::NotDeferrability             );
    ComboBox_nDeferrability     ->addItem( "DeferrableInitiallyDeferred ", RelationPropertyDeferrability_t::DeferrableInitiallyDeferred  );
    ComboBox_nDeferrability     ->addItem( "DeferrableInitiallyImmediate", RelationPropertyDeferrability_t::DeferrableInitiallyImmediate );

    ComboBox_nUpdateRule        ->addItem( "Cascade    ", RelationPropertyRule_t::Cascade    );
    ComboBox_nUpdateRule        ->addItem( "Restrict   ", RelationPropertyRule_t::Restrict   );
    ComboBox_nUpdateRule        ->addItem( "No Action  ", RelationPropertyRule_t::NoAction   );
    ComboBox_nUpdateRule        ->addItem( "Set Null   ", RelationPropertyRule_t::SetNull    );
    ComboBox_nUpdateRule        ->addItem( "Set Default", RelationPropertyRule_t::SetDefault );

    ComboBox_nDeleteRule        ->addItem( "Cascade    ", RelationPropertyRule_t::Cascade    );
    ComboBox_nDeleteRule        ->addItem( "Restrict   ", RelationPropertyRule_t::Restrict   );
    ComboBox_nDeleteRule        ->addItem( "No Action  ", RelationPropertyRule_t::NoAction   );
    ComboBox_nDeleteRule        ->addItem( "Set Null   ", RelationPropertyRule_t::SetNull    );
    ComboBox_nDeleteRule        ->addItem( "Set Default", RelationPropertyRule_t::SetDefault );

    setData();

    setWindowTitle(title);
}

void
CRelationDetails::setData()
{
    Edit_Name         ->setText( m_pItem->m_Name        .c_str() );
    Edit_PKTableLabel ->setText( m_pItem->m_PKTableLabel.c_str() );
    Edit_FKTableLabel ->setText( m_pItem->m_FKTableLabel.c_str() );

    ComboBox_nType              ->setCurrentIndex( ComboBox_nType              ->findData( m_pItem->m_nType               ) );
    ComboBox_nCardinalityPKTable->setCurrentIndex( ComboBox_nCardinalityPKTable->findData( m_pItem->m_nCardinalityPKTable ) );
    ComboBox_nCardinalityFKTable->setCurrentIndex( ComboBox_nCardinalityFKTable->findData( m_pItem->m_nCardinalityFKTable ) );
    ComboBox_nDeferrability     ->setCurrentIndex( ComboBox_nDeferrability     ->findData( m_pItem->m_nDeferrability      ) );
    ComboBox_nUpdateRule        ->setCurrentIndex( ComboBox_nUpdateRule        ->findData( m_pItem->m_nUpdateRule         ) );
    ComboBox_nDeleteRule        ->setCurrentIndex( ComboBox_nDeleteRule        ->findData( m_pItem->m_nDeleteRule         ) );
}

void
CRelationDetails::getData()
{
    m_pItem->m_Name         = Edit_Name        ->text().toStdString();
    m_pItem->m_PKTableLabel = Edit_PKTableLabel->text().toStdString();
    m_pItem->m_FKTableLabel = Edit_FKTableLabel->text().toStdString();

    m_pItem->m_nType               = (RelationPropertyType_t         )ComboBox_nType              ->currentData().toInt();
    m_pItem->m_nCardinalityPKTable = (RelationPropertyCardinality_t  )ComboBox_nCardinalityPKTable->currentData().toInt();
    m_pItem->m_nCardinalityFKTable = (RelationPropertyCardinality_t  )ComboBox_nCardinalityFKTable->currentData().toInt();
    m_pItem->m_nDeferrability      = (RelationPropertyDeferrability_t)ComboBox_nDeferrability     ->currentData().toInt();
    m_pItem->m_nUpdateRule         = (RelationPropertyRule_t         )ComboBox_nUpdateRule        ->currentData().toInt();
    m_pItem->m_nDeleteRule         = (RelationPropertyRule_t         )ComboBox_nDeleteRule        ->currentData().toInt();
}
