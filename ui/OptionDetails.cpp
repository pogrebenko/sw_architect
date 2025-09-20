/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "OptionDetails.h"

#include "common/Logger.h"

#include "data/Options.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
#define                             __AppOptions (*__AppOptions__.get())

OptionDetails::OptionDetails(const QString &title, QWidget *parent)
: QDialog(parent)
{
    Label_ProjectName  = new QLabel   ("Project name:");
    Label_DatabaseName = new QLabel   ("Database name:");
    Label_TablePrefix  = new QLabel   ("Table prefix:");
    Label_ClassPrefix  = new QLabel   ("Class prefix:");
    Edit_ProjectName   = new QLineEdit();
    Edit_DatabaseName  = new QLineEdit();
    Edit_TablePrefix   = new QLineEdit();
    Edit_ClassPrefix   = new QLineEdit();

    buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &OptionDetails::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &OptionDetails::reject);

    int n = 0;
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget( Label_ProjectName , n  , 0 );
    mainLayout->addWidget( Edit_ProjectName  , n++, 1 );
    mainLayout->addWidget( Label_DatabaseName, n  , 0 );
    mainLayout->addWidget( Edit_DatabaseName , n++, 1 );
    mainLayout->addWidget( Label_TablePrefix , n  , 0 );
    mainLayout->addWidget( Edit_TablePrefix  , n++, 1 );
    mainLayout->addWidget( Label_ClassPrefix , n  , 0 );
    mainLayout->addWidget( Edit_ClassPrefix  , n++, 1 );

    mainLayout->addWidget( buttonBox, n, 0, 1, 2, Qt::AlignCenter );
    setLayout(mainLayout);

    setData();

    setWindowTitle(title);
}

void OptionDetails::setData()
{
    Edit_ProjectName ->setText( __AppOptions.getProjectName () );
    Edit_DatabaseName->setText( __AppOptions.getDatabaseName() );
    Edit_TablePrefix ->setText( __AppOptions.getTablePrefix () );
    Edit_ClassPrefix ->setText( __AppOptions.getClassPrefix () );
}

void OptionDetails::getData()
{
    __AppOptions.setProjectName ( Edit_ProjectName ->text() );
    __AppOptions.setDatabaseName( Edit_DatabaseName->text() );
    __AppOptions.setTablePrefix ( Edit_TablePrefix ->text() );
    __AppOptions.setClassPrefix ( Edit_ClassPrefix ->text() );
}
