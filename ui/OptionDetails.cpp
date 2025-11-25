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
    //Label_DatabaseName = new QLabel   ("Database name:");
    Label_TablePrefix  = new QLabel   ("Table prefix:");
    Label_ClassPrefix  = new QLabel   ("Class prefix:");
    Label_nDB          = new QLabel   ("Database:");
    Label_ServerName   = new QLabel   ("Server/IP:");
    Label_Port         = new QLabel   ("Port:");
    Label_BaseName     = new QLabel   ("Database name:");
    Label_UserName     = new QLabel   ("Username:");
    Label_UserPass     = new QLabel   ("Password:");
    Label_SysAdmin     = new QLabel   ("Sysadmin:");

    Edit_ProjectName   = new QLineEdit();
    //Edit_DatabaseName  = new QLineEdit();
    Edit_TablePrefix   = new QLineEdit();
    Edit_ClassPrefix   = new QLineEdit();
    Combo_nDB          = new QComboBox();
    Edit_ServerName    = new QLineEdit();
    Edit_Port          = new QLineEdit();
    Edit_BaseName      = new QLineEdit();
    Edit_UserName      = new QLineEdit();
    Edit_UserPass      = new QLineEdit();
    Edit_SysAdmin      = new QLineEdit();

    buttonBox = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel );

    connect(buttonBox, &QDialogButtonBox::accepted, this, &OptionDetails::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &OptionDetails::reject);

    int n = 0;
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget( Label_ProjectName , n  , 0 );
    mainLayout->addWidget( Edit_ProjectName  , n++, 1 );
    // mainLayout->addWidget( Label_DatabaseName, n  , 0 );
    // mainLayout->addWidget( Edit_DatabaseName , n++, 1 );
    mainLayout->addWidget( Label_TablePrefix , n  , 0 );
    mainLayout->addWidget( Edit_TablePrefix  , n++, 1 );
    mainLayout->addWidget( Label_ClassPrefix , n  , 0 );
    mainLayout->addWidget( Edit_ClassPrefix  , n++, 1 );

    mainLayout->addWidget( Label_nDB        , n  , 0 );
    mainLayout->addWidget( Combo_nDB        , n++, 1 );
    mainLayout->addWidget( Label_ServerName , n  , 0 );
    mainLayout->addWidget( Edit_ServerName  , n++, 1 );
    mainLayout->addWidget( Label_Port       , n  , 0 );
    mainLayout->addWidget( Edit_Port        , n++, 1 );
    mainLayout->addWidget( Label_BaseName   , n  , 0 );
    mainLayout->addWidget( Edit_BaseName    , n++, 1 );
    mainLayout->addWidget( Label_UserName   , n  , 0 );
    mainLayout->addWidget( Edit_UserName    , n++, 1 );
    mainLayout->addWidget( Label_UserPass   , n  , 0 );
    mainLayout->addWidget( Edit_UserPass    , n++, 1 );
    mainLayout->addWidget( Label_SysAdmin   , n  , 0 );
    mainLayout->addWidget( Edit_SysAdmin    , n++, 1 );

    mainLayout->addWidget( buttonBox, n, 0, 1, 2, Qt::AlignCenter );
    setLayout(mainLayout);

    Combo_nDB->addItem( "None"  , PDatabases::ID_PDATABASES_NONE   );
    Combo_nDB->addItem( "SQLite", PDatabases::ID_PDATABASES_SQLITE );
    Combo_nDB->addItem( "MySQL" , PDatabases::ID_PDATABASES_MYSQL  );
    Combo_nDB->addItem( "ODBC"  , PDatabases::ID_PDATABASES_ODBC   );

    setData();

    setWindowTitle(title);
}

void OptionDetails::setData()
{
    Edit_ProjectName ->setText( __AppOptions.getProjectName () );
    //Edit_DatabaseName->setText( __AppOptions.getDatabaseName() );
    Edit_TablePrefix ->setText( __AppOptions.getTablePrefix () );
    Edit_ClassPrefix ->setText( __AppOptions.getClassPrefix () );

    Combo_nDB        ->setCurrentIndex( Combo_nDB->findData(__AppOptions.m_nDB) );
    Edit_ServerName  ->setText( __AppOptions.m_ServerName  );
    Edit_Port        ->setText( __AppOptions.m_Port        );
    Edit_BaseName    ->setText( __AppOptions.m_BaseName    );
    Edit_UserName    ->setText( __AppOptions.m_UserName    );
    Edit_UserPass    ->setText( __AppOptions.m_UserPass    );
    Edit_SysAdmin    ->setText( __AppOptions.m_SysAdmin    );
}

void OptionDetails::getData()
{
    __AppOptions.setProjectName ( Edit_ProjectName ->text() );
    //__AppOptions.setDatabaseName( Edit_DatabaseName->text() );
    __AppOptions.setTablePrefix ( Edit_TablePrefix ->text() );
    __AppOptions.setClassPrefix ( Edit_ClassPrefix ->text() );

    __AppOptions.m_nDB         = (PDatabases)Combo_nDB->currentData().toInt();
    __AppOptions.m_ServerName  = Edit_ServerName  ->text();
    __AppOptions.m_Port        = Edit_Port        ->text();
    __AppOptions.m_BaseName    = Edit_BaseName    ->text();
    __AppOptions.m_UserName    = Edit_UserName    ->text();
    __AppOptions.m_UserPass    = Edit_UserPass    ->text();
    __AppOptions.m_SysAdmin    = Edit_SysAdmin    ->text();
}
