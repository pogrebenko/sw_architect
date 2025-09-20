/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "ClassDetails.h"

#include "data/general/Class.h"

ClassDetails::ClassDetails( const QString &title, __Class *pItem, QWidget *parent )
: QDialog( parent )
, m_pItem( pItem  )
{
    Label_LogicalName  = new QLabel   ("Logical name:");
    Label_PhysicalName = new QLabel   ("Physical name:");
    Edit_LogicalName   = new QLineEdit();
    Edit_PhysicalName  = new QLineEdit();

    buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &ClassDetails::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ClassDetails::reject);

    int n = 0;
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget( Label_LogicalName , n  , 0 );
    mainLayout->addWidget( Edit_LogicalName  , n++, 1 );
    mainLayout->addWidget( Label_PhysicalName, n  , 0 );
    mainLayout->addWidget( Edit_PhysicalName , n++, 1 );

    mainLayout->addWidget( buttonBox, n, 0, 1, 2, Qt::AlignCenter );
    setLayout(mainLayout);

    setData();

    setWindowTitle(title);
}

void ClassDetails::setData()
{
    Edit_LogicalName ->setText( m_pItem->m_LogicalName.c_str() );
    Edit_PhysicalName->setText( m_pItem->m_LogicalName.c_str() );
}

void ClassDetails::getData()
{
    m_pItem->m_LogicalName  = Edit_LogicalName ->text().toStdString();
    m_pItem->m_PhysicalName = Edit_PhysicalName->text().toStdString();
}
