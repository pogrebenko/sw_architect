/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "FieldDetails.h"

#include "data/general/Field.h"

FieldDetails::FieldDetails( const QString &title, __Field *pItem, QWidget *parent )
: QDialog( parent )
, m_pItem( pItem  )
{
    Label_LogicalName       = new QLabel   ( "Logical name:"  );
    Edit_LogicalName        = new QLineEdit();
    Label_PhysicalName      = new QLabel   ( "Physical name:" );
    Edit_PhysicalName       = new QLineEdit();
    CheckBox_bPrimaryKey    = new QCheckBox( "Primary key" );
    CheckBox_bForeignKey    = new QCheckBox( "Foreign key" );
    Label_nFieldDataType    = new QLabel   ( "Data type :" );
    ComboBox_nFieldDataType = new QComboBox();
    Label_nPrecission       = new QLabel   ( "Precission :" );
    Edit_nPrecission        = new QLineEdit();
    Label_nScale            = new QLabel   ( "Scale :" );
    Edit_nScale             = new QLineEdit();
    CheckBox_bAllowsNulls   = new QCheckBox( "Allows nulls" );
    CheckBox_bAutoIncrement = new QCheckBox( "Auto increment" );
    Label_DefaultValue      = new QLabel   ( "Default value :" );
    Edit_DefaultValue       = new QLineEdit();
    Label_Remarks           = new QLabel   ( "Remarks :" );
    Edit_Remarks            = new QTextEdit();
    Edit_Remarks->setFixedHeight( 3 * QFontMetrics( Edit_Remarks->font() ).lineSpacing() );

    buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &FieldDetails::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &FieldDetails::reject);

    int n = 0;
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget( Label_LogicalName      , n  , 0 );
    mainLayout->addWidget( Edit_LogicalName       , n++, 1 );
    mainLayout->addWidget( Label_PhysicalName     , n  , 0 );
    mainLayout->addWidget( Edit_PhysicalName      , n++, 1 );
    mainLayout->addWidget( CheckBox_bPrimaryKey   , n++, 0, 1, 2 );
    mainLayout->addWidget( CheckBox_bForeignKey   , n++, 0, 1, 2 );
    mainLayout->addWidget( Label_nFieldDataType   , n  , 0 );
    mainLayout->addWidget( ComboBox_nFieldDataType, n++, 1 );
    mainLayout->addWidget( Label_nPrecission      , n  , 0 );
    mainLayout->addWidget( Edit_nPrecission       , n++, 1 );
    mainLayout->addWidget( Label_nScale           , n  , 0 );
    mainLayout->addWidget( Edit_nScale            , n++, 1 );
    mainLayout->addWidget( CheckBox_bAllowsNulls  , n++, 0, 1, 2 );
    mainLayout->addWidget( CheckBox_bAutoIncrement, n++, 0, 1, 2 );
    mainLayout->addWidget( Label_DefaultValue     , n  , 0 );
    mainLayout->addWidget( Edit_DefaultValue      , n++, 1 );
    mainLayout->addWidget( Label_Remarks          , n  , 0 );
    mainLayout->addWidget( Edit_Remarks           , n++, 1 );

    mainLayout->addWidget( buttonBox, n, 0, 1, 2, Qt::AlignCenter );
    setLayout(mainLayout);

    ComboBox_nFieldDataType->addItem( "None     ", FieldDataTypeNone      );
    ComboBox_nFieldDataType->addItem( "Integer  ", FieldDataTypeInteger   );
    ComboBox_nFieldDataType->addItem( "Decimal  ", FieldDataTypeDecimal   );
    ComboBox_nFieldDataType->addItem( "Timestamp", FieldDataTypeTimestamp );
    ComboBox_nFieldDataType->addItem( "Time     ", FieldDataTypeTime      );
    ComboBox_nFieldDataType->addItem( "Date     ", FieldDataTypeDate      );
    ComboBox_nFieldDataType->addItem( "Blob     ", FieldDataTypeBlob      );
    ComboBox_nFieldDataType->addItem( "Varchar  ", FieldDataTypeVarchar   );
    ComboBox_nFieldDataType->addItem( "Char     ", FieldDataTypeChar      );

    setData();

    setWindowTitle(title);
}

void FieldDetails::setData()
{
    Edit_LogicalName ->setText( m_pItem->m_LogicalName.c_str() );
    Edit_PhysicalName->setText( m_pItem->m_LogicalName.c_str() );

    CheckBox_bPrimaryKey   ->setChecked     (                  m_pItem->m_nFieldRelationType == FieldRelationTypePrimaryKey );
    CheckBox_bForeignKey   ->setChecked     (                  m_pItem->m_nFieldRelationType == FieldRelationTypeForeignKey );

    ComboBox_nFieldDataType->setCurrentIndex( ComboBox_nFieldDataType->findData(m_pItem->m_nFieldDataType) );

    Edit_nPrecission       ->setText        ( QString::number( m_pItem->m_nPrecission          ) );
    Edit_nScale            ->setText        ( QString::number( m_pItem->m_nScale               ) );
    CheckBox_bAllowsNulls  ->setChecked     (                  m_pItem->m_bAllowsNulls           );
    CheckBox_bAutoIncrement->setChecked     (                  m_pItem->m_bAutoIncrement         );
    Edit_DefaultValue      ->setText        (                  m_pItem->m_DefaultValue.c_str()   );
    Edit_Remarks           ->setText        (                  m_pItem->m_Remarks     .c_str()   );
}

void FieldDetails::getData()
{
    m_pItem->m_LogicalName        = Edit_LogicalName       ->text       ().toStdString();
    m_pItem->m_PhysicalName       = Edit_PhysicalName      ->text       ().toStdString();
    m_pItem->m_nFieldRelationType = CheckBox_bPrimaryKey   ->isChecked  () ? FieldRelationTypePrimaryKey : FieldRelationTypeNone;
    if( !CheckBox_bPrimaryKey->isChecked() )
    m_pItem->m_nFieldRelationType = CheckBox_bForeignKey   ->isChecked  () ? FieldRelationTypeForeignKey : FieldRelationTypeNone;

    m_pItem->m_nFieldDataType     = (FieldDataType_t)ComboBox_nFieldDataType->currentData().toInt();

    m_pItem->m_nPrecission        = Edit_nPrecission       ->text       ().toInt      ();
    m_pItem->m_nScale             = Edit_nScale            ->text       ().toInt      ();
    m_pItem->m_bAllowsNulls       = CheckBox_bAllowsNulls  ->isChecked  ()              ;
    m_pItem->m_bAutoIncrement     = CheckBox_bAutoIncrement->isChecked  ()              ;
    m_pItem->m_DefaultValue       = Edit_DefaultValue      ->text       ().toStdString();
    m_pItem->m_Remarks            = Edit_Remarks           ->toPlainText().toStdString();
}
