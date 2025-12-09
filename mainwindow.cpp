/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "mainwindow.h"

#include "ui_mainwindow.h"

#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printdialog)
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#endif
#endif

#include "common/IOStreamer.h"
#include "data/shapes/Rectangle.h"

#include "data/Options.h"
#include "data/History.h"

#include "data/databases/SQLStreamer.h"

#include "ui/OptionDetails.h"

#include "templates.h"

#include "psql/sqlinfo.h"
#include "psql/sqlbridge.h"
#include "psql/sqlpipe.h"

extern CLogStream                   __Logger;
extern std::unique_ptr<CAppOptions> __AppOptions__;
extern CHistory                     __History;
extern CSQLBridge                   g_SQLBridge;

#define __AppOptions (*__AppOptions__.get())

MainWindow::MainWindow( QWidget *parent )
: QMainWindow( parent )
, ui( new Ui::MainWindow )
, actionSave            ( nullptr )
, actionSaveAs          ( nullptr )
, actionLoad            ( nullptr )
, actionPrint           ( nullptr )
, actionPrintPreview    ( nullptr )
, actionExit            ( nullptr )
, actionClear           ( nullptr )
, actionUndo            ( nullptr )
, actionRedo            ( nullptr )
, actionHorizontalyLeft ( nullptr )
, actionVerticallyUp    ( nullptr )
, actionHorizontalyRight( nullptr )
, actionVerticallyDown  ( nullptr )
, actionCopy            ( nullptr )
, actionPaste           ( nullptr )
, actionSettings        ( nullptr )
, actionExpotToSQL      ( nullptr )
, actionImportFromDB    ( nullptr )
, actionAbout           ( nullptr )
, actionAboutQt         ( nullptr )
, actionZoomIn          ( nullptr )
, actionZoomOut         ( nullptr )
, actionActual          ( nullptr )
, m_pRecentFilesMenu    ( nullptr )
, m_pZoomMenu           ( nullptr )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    // build menu and init toolbar
    setup_ui();
    init_ui( &__AppOptions );

    // create canvas
    QHBoxLayout *pLayout = new QHBoxLayout();
    m_pCanvas = new CCanvas( this, &__AppOptions, &__Logger );

    // create and initizlize scrollarea with canvas
    QScrollArea *scrollArea = new QScrollArea( this );
                 scrollArea->setWidget( m_pCanvas );
                 scrollArea->setWidgetResizable( true );
    pLayout->addWidget( scrollArea );
    centralWidget()->setLayout( pLayout );

    m_pCanvas->setMinimumSize( m_pCanvas->width(), m_pCanvas->height() );
}

MainWindow::~MainWindow()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    delete_ui();
}

void
MainWindow::delete_ui()
{
    delete ui;
    delete actionSave;
    delete actionSaveAs;
    delete actionLoad;
    delete actionPrint;
    delete actionPrintPreview;
    delete actionExit;
    delete actionClear;
    delete actionUndo;
    delete actionRedo;
    delete actionCopy;
    delete actionHorizontalyLeft;
    delete actionVerticallyUp;
    delete actionHorizontalyRight;
    delete actionVerticallyDown;
    delete actionPaste;
    delete actionSettings;
    delete actionExpotToSQL;
    delete actionImportFromDB;
    delete actionAbout;
    delete actionAboutQt;
    delete actionZoomIn;
    delete actionZoomOut;
    delete actionActual;

    delete m_pRecentFilesMenu;
    delete m_pZoomMenu;
    delete m_pAlignMenu;

    std::for_each( m_pRecentFileActionList.begin(), m_pRecentFileActionList.end(), []( auto pItem ){ delete pItem; } );
}

void
MainWindow::init_ui( CAppOptions *pAppOption )
{
    switch( pAppOption->getActionType() )
    {
        case ActionTypeAddFigure :
        {
            switch( pAppOption->getFigureType() )
            {
                //                case FigureTypeTriangle  : ui->actionTriangle ->setChecked( true ); break;
                //                case FigureTypeEllipse   : ui->actionEllipce  ->setChecked( true ); break;
                case FigureTypeRectangle : ui->actionClass->setChecked( true ); break;
                case FigureTypeNone      : assert( false && "error: figure type none" ); break;
                default                  : assert( false && "error: unknown figure type" ); break;
            }
        } break;
        //        case ActionTypeAddRelation : ui->actionRelation->setChecked( true ); break;
        case ActionTypeDelete      : ui->actionDelete  ->setChecked( true ); break;
        case ActionTypeMove        : ui->actionMove    ->setChecked( true ); break;
        default                    : throw "error: unknown figure type"; //assert( false && "error: unknown figure type" ); break;
    }
}

void
MainWindow::setup_ui()
{
    ui->setupUi( this );

    for( int i = 0; i < MAXRECENTFILE; ++ i )
    {
        QAction *recentFileAction = new QAction( this );
                 recentFileAction->setVisible(false);

        QObject::connect( recentFileAction, &QAction::triggered, this, &MainWindow::on_OpenRecentFile );

        m_pRecentFileActionList.append( recentFileAction );
    }

    actionLoad         = ui->menuFile->addAction( "Open...", this, &MainWindow::on_Load );
    m_pRecentFilesMenu = ui->menuFile->addMenu( "Open Recent" );
    for( int i = 0; i < MAXRECENTFILE; ++ i )
    {
        m_pRecentFilesMenu->addAction( m_pRecentFileActionList.at( i ) );
    }
    m_pRecentFilesMenu->addSeparator();
    actionClear        = m_pRecentFilesMenu->addAction( "Clear recent files", this, &MainWindow::on_ClearRecentFiles );
    actionSave         = ui->menuFile   ->addAction( "Save"         , this, &MainWindow::on_Save         );
    actionSaveAs       = ui->menuFile   ->addAction( "Save as..."   , this, &MainWindow::on_SaveAs       );
    actionPrint        = ui->menuFile   ->addAction( "Print..."     , this, &MainWindow::on_Print        );
    actionPrintPreview = ui->menuFile   ->addAction( "Print preview", this, &MainWindow::on_PrintPreview );
    actionExit         = ui->menuFile   ->addAction( "Exit"         , this, &MainWindow::on_Exit         );

    updateRecentActionList();

    actionUndo         = ui->menuEdit->addAction( "Undo"    , this, &MainWindow::on_Undo        );
    actionRedo         = ui->menuEdit->addAction( "Redo"    , this, &MainWindow::on_Redo        );

    m_pAlignMenu          = ui->menuEdit->addMenu( "Align" );
    actionHorizontalyLeft = m_pAlignMenu->addAction( "Horizontaly left" , this, &MainWindow::on_AlignHorizontalyLeft  );
    actionHorizontalyRight= m_pAlignMenu->addAction( "Horizontaly right", this, &MainWindow::on_AlignHorizontalyRight );
    actionVerticallyUp    = m_pAlignMenu->addAction( "Vertically up"    , this, &MainWindow::on_AlignVerticallyUp     );
    actionVerticallyDown  = m_pAlignMenu->addAction( "Vertically down"  , this, &MainWindow::on_AlignVerticallyDown   );

    actionCopy         = ui->menuEdit->addAction( "Copy"    , this, &MainWindow::on_Copy        );
    actionPaste        = ui->menuEdit->addAction( "Paste"   , this, &MainWindow::on_Paste       );
    actionSettings     = ui->menuEdit->addAction( "Settings", this, &MainWindow::on_EditOptions );

    m_pZoomMenu        = ui->menuView->addMenu( "Zoom" );
    actionZoomIn       = m_pZoomMenu->addAction( "Zoom In"    , this, &MainWindow::on_ScaleMinus );
    actionZoomOut      = m_pZoomMenu->addAction( "Zoom Out"   , this, &MainWindow::on_ScalePlus  );
    m_pZoomMenu->addSeparator();
    actionActual       = m_pZoomMenu->addAction( "Actual Size", this, &MainWindow::on_ScaleReset );

    actionExpotToSQL   = ui->menuTools->addAction( "Export to SQL"       , this, &MainWindow::on_ExpotToSQL );
    actionImportFromDB = ui->menuTools->addAction( "Import from Database", this, &MainWindow::on_ImportFromDB );

    actionAbout        = ui->menuHelp ->addAction( "About..."     , this, &MainWindow::on_About      );
    actionAboutQt      = ui->menuHelp ->addAction( "About Qt..."  , this, &MainWindow::on_AboutQt    );

    // heroicons.com/outline
    //auto icons = {actionLoad };

    actionLoad            ->setIcon( QIcon(":/images/arrow-up-tray.svg"                 ) );
    actionSave            ->setIcon( QIcon(":/images/arrow-down-tray.svg"               ) );
    actionPrint           ->setIcon( QIcon(":/images/printer.svg"                       ) );
    actionExit            ->setIcon( QIcon(":/images/arrow-right-start-on-rectangle.svg") );
    actionUndo            ->setIcon( QIcon(":/images/arrow-uturn-left.svg"              ) );
    actionRedo            ->setIcon( QIcon(":/images/arrow-uturn-right.svg"             ) );
    actionCopy            ->setIcon( QIcon(":/images/clipboard-16x16.svg"               ) );
    actionPaste           ->setIcon( QIcon(":/images/clipboard-document-16x16.svg"      ) );
    actionSettings        ->setIcon( QIcon(":/images/cog-6-tooth-16x16.svg"             ) );
    actionZoomIn          ->setIcon( QIcon(":/images/magnifying-glass-minus-16x16.svg"  ) );
    actionZoomOut         ->setIcon( QIcon(":/images/magnifying-glass-plus-16x16.svg"   ) );
    actionExpotToSQL      ->setIcon( QIcon(":/images/circle-stack.svg"                  ) );
    actionHorizontalyLeft ->setIcon( QIcon(":/images/bars-3-center-left-16x16.svg"      ) );
    actionHorizontalyRight->setIcon( QIcon(":/images/bars-3-center-right-16x16.svg"     ) );
    actionVerticallyUp    ->setIcon( QIcon(":/images/bars-3-center-up-16x16.svg"        ) );
    actionVerticallyDown  ->setIcon( QIcon(":/images/bars-3-center-down-16x16.svg"      ) );

    connect( ui->actionClass, &QAction::triggered, this, &MainWindow::on_Rectangle );
    //connect( ui->actionEllipce  , &QAction::triggered, this, &MainWindow::on_Ellipse   );
    //connect( ui->actionTriangle , &QAction::triggered, this, &MainWindow::on_Triangle  );
    //connect( ui->actionRelation , &QAction::triggered, this, &MainWindow::on_Relation  );
    connect( ui->actionDelete   , &QAction::triggered, this, &MainWindow::on_Delete    );
    connect( ui->actionMove     , &QAction::triggered, this, &MainWindow::on_Move      );

    connect( ui->actionScaleMinus, &QAction::triggered, this, &MainWindow::on_ScaleMinus );
    connect( ui->actionScalePlus , &QAction::triggered, this, &MainWindow::on_ScalePlus  );
    connect( ui->actionScaleReset, &QAction::triggered, this, &MainWindow::on_ScaleReset );

    connect( ui->actionResetRotate, &QAction::triggered, this, &MainWindow::on_ResetRotate );

    connect( ui->actionEditOptions, &QAction::triggered, this, &MainWindow::on_EditOptions );

    connect( ui->actionSaveFile   , &QAction::triggered, this, &MainWindow::on_Save        );
    connect( ui->actionClear      , &QAction::triggered, this, &MainWindow::on_Clear       );

    QActionGroup* alignGroup = new QActionGroup( this );
    alignGroup->setExclusive( true );
    alignGroup->addAction( ui->actionClass );
//    alignGroup->addAction( ui->actionEllipce   );
//    alignGroup->addAction( ui->actionTriangle  );
//    alignGroup->addAction( ui->actionRelation  );
    alignGroup->addAction( ui->actionDelete    );
    alignGroup->addAction( ui->actionMove      );

    statusBar()->showMessage( "ready" );
}

void
MainWindow::on_AlignHorizontalyLeft()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    if( m_pCanvas->align_horizontaly_left() )
    {
        CRefreshMainWnd w( this, "Align horizontaly left" );
        CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
        QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );
    }
}

void
MainWindow::on_AlignHorizontalyRight()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    if( m_pCanvas->align_horizontaly_right() )
    {
        CRefreshMainWnd w( this, "Align horizontaly right" );
        CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
        QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );
    }
}

void
MainWindow::on_AlignVerticallyUp()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    if( m_pCanvas->align_vertically_up() )
    {
        CRefreshMainWnd w( this, "Align vertically up" );
        CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
        QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );
    }
}

void
MainWindow::on_AlignVerticallyDown()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    if( m_pCanvas->align_vertically_down() )
    {
        CRefreshMainWnd w( this, "Align vertically down" );
        CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
        QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );
    }
}

void
MainWindow::on_ExpotToSQL()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    QString sFile = QFileDialog::getSaveFileName(
        this,
        "Export project to SQL file",
        QDir::homePath(),
        "SQL Files (*.sql);;All Files (*)"
    );
    if( !sFile.isEmpty() )
    {
        QString sError = "";
        this->setCursor( Qt::WaitCursor );
        try
        {
            QFile file( sFile );
            if( file.open( QFile::WriteOnly ) )
            {
                QByteArray   Buffer;
                CSQLStreamer ios( CSQLFactory( __AppOptions.m_nDB ).buildDatabase(), &__AppOptions );
                ::Write( ios, __AppOptions );
                Buffer.append( ios.m_Buffer.c_str(), ios.m_Buffer.size() );
                file.write( Buffer );

                file.flush();
                file.close();

                __Logger.info( APP_LOG_LEVEL, "info: File saved [%s]", sFile.toLatin1().constData() );
                statusBar()->showMessage( "project '" + __AppOptions.getProjectName() + "' saved to file " + sFile , DEFAULT_MSG_TIMEOUT );
            }
            else
            {
                sError = "Error: Could not open file: %s" + file.errorString();
            }

        }
        catch( ... )
        {
            sError = "Exception error when trying to open a file.";
        }
        this->setCursor( Qt::ArrowCursor );

        if( !sError.isEmpty() )
        {
            QMessageBox::critical( this, "Exception", sError );
        }
    }
}

void
MainWindow::on_ImportFromDB()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    CRefreshMainWnd w( this );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );

    this->setCursor( Qt::ArrowCursor );

    if( !__AppOptions.m_hProvider )
    {
        CSqlConnectInfo Info;
        Info.m_nDB = __AppOptions.m_nDB;
        _tcscpy( (TCHAR*)Info.m_ServerName, (const TCHAR*)__AppOptions.m_ServerName.toStdString().c_str() );
        _tcscpy( (TCHAR*)Info.m_BaseName  , (const TCHAR*)__AppOptions.m_BaseName  .toStdString().c_str() );
        _tcscpy( (TCHAR*)Info.m_UserName  , (const TCHAR*)__AppOptions.m_UserName  .toStdString().c_str() );
        _tcscpy( (TCHAR*)Info.m_UserPass  , (const TCHAR*)__AppOptions.m_UserPass  .toStdString().c_str() );
        _tcscpy( (TCHAR*)Info.m_SysAdmin  , (const TCHAR*)__AppOptions.m_SysAdmin  .toStdString().c_str() );
        Info.m_Port = __AppOptions.m_Port.toInt();
        ::g_SQLBridge.Connect( Info, &__AppOptions.m_hProvider );
    }

    if( __AppOptions.m_hProvider )
    {
        auto class_list = __AppOptions.getClassList();
        class_list->Flush();

        int nHeight = __AppOptions.getHeight() + 2 * __AppOptions.getPadding();

        CRectangle t;

        std::vector<__DBCol__> dbcolumns;

        PDatabases db = ::g_SQLBridge.GetDatabase( __AppOptions.m_hProvider );

        TCHAR table_name[ 256 ];

        std::string
            table_prefix = __AppOptions.m_TablePrefix.toStdString(),
            table_type   = "BASE TABLE",
            table_schema = __AppOptions.m_BaseName.toStdString();

        CSQLPipe sql( __AppOptions.m_hProvider );
        switch( db )
        {
            case PDatabases::ID_PDATABASES_MYSQL :
            {
                sql = _T( "SELECT" )
                    + Buff( _T( " table_name" ), table_name, sizeof( table_name ) )
                    + _T( " FROM INFORMATION_SCHEMA.TABLES" )
                    + _T( " WHERE " ) + Bind( "table_type"  , table_type   )
                    + _T( " AND "   ) + Bind( "table_schema", table_schema )
                    + _T( " ORDER BY table_name ASC");
                break;
            }
            default : break;
        }

        if( sql.Execute() )
            while( sql.Fetch() )
            {
                t.m_LogicalName = t.m_PhysicalName = table_name;
                if( !t.m_PhysicalName.empty() && t.m_PhysicalName.find( table_prefix ) == std::string::npos )
                {
                    continue;
                }
                class_list->Add( new CRectangle( t ) );
            }
        sql.Flush();

        __DBCol__ c; memset( &c, 0, sizeof(c) );
        switch( db )
        {
            case PDatabases::ID_PDATABASES_MYSQL :
            {
                sql = _T( "SELECT" )
                    + Buff( _T( " COLUMN_NAME"             ),  c.column_name   , sizeof( c.column_name    ) )
                    + Buff( _T( ",DATA_TYPE"               ),  c.datatype      , sizeof( c.datatype       ) )
                    + Buff( _T( ",COLUMN_KEY"              ),  c.key           , sizeof( c.key            ) )
                    + Buff( _T( ",IS_NULLABLE"             ),  c.is_null       , sizeof( c.is_null        ) )
                    + Buff( _T( ",COLUMN_COMMENT"          ),  c.column_comment, sizeof( c.column_comment ) )
                    + Buff( _T( ",CHARACTER_MAXIMUM_LENGTH"), &c.length                                     )
                    + Buff( _T( ",NUMERIC_PRECISION"       ), &c.precision                                  )
                    + Buff( _T( ",NUMERIC_SCALE"           ), &c.scale                                      )
                    + Buff( _T( ",DATETIME_PRECISION"      ), &c.dt_precision                               )
                    + _T( " FROM INFORMATION_SCHEMA.COLUMNS" )
                    + _T( " WHERE " ) + Bind( "TABLE_NAME", table_name, sizeof( table_name ) )
                    + _T( " ORDER BY ORDINAL_POSITION ASC");
                break;
            }
            default : break;
        }

        std::for_each( class_list->begin(), class_list->end(),
            [ table_prefix, &table_name, &sql, &c ]( auto &p )
            {
                strcpy(table_name,p->m_PhysicalName.c_str());
                if( sql.Execute() ) while( sql.Fetch() ) p->m_FieldList.Add( new Field_t( c ) );
            }
        );

        auto fm = this->fontMetrics();

        long row = -1, col = 0; int pos_x = 0, pos_y = 0;
        for( unsigned long n = 0; n < class_list->size(); ++ n )
        {
            auto &t = class_list->at( n );

            if( n % 20 == 0 )
            {
                col = 0;
                row ++;
            }
            else
            {
                col ++;
            }

            // вычисляем первичное смещение по горизонтали и вертикали
            pos_x = DEFAULT_OFFSET_DX + col * ( DEFAULT_OFFSET_DX + ADD_DEFAULT_WIDTH  ),
            pos_y = DEFAULT_OFFSET_DY + row * ( DEFAULT_OFFSET_DY + ADD_DEFAULT_HEIGHT );
            if( row > 0 && col > 0 )
            {   // если предыдущая таблица широкая, то сдвигаем ее правее
                auto &prev = class_list->at( n - 1 );
                pos_x = prev->m_nLastPos.x() + DEFAULT_OFFSET_DX;
            }

            // самый длинный текст используется для вычисления ширины таблицы
            std::string max_text;
            std::for_each( t->m_FieldList.begin(), t->m_FieldList.end(),
              [ &max_text ]( auto pItem )
              {
                  if( max_text.size() < pItem->m_PhysicalName.size() ) max_text = pItem->m_PhysicalName;
              }
            );
            if( max_text.size() < t->m_PhysicalName.size() ) max_text = t->m_PhysicalName;


            // step 1 устанавливаем первичные размеры таблицы и ее полей
            t->m_nFirstPos = QPoint( pos_x, pos_y );
            t->m_nLastPos  = t->m_nFirstPos + QPoint( ADD_DEFAULT_WIDTH, ADD_DEFAULT_HEIGHT );

            t->calc_angle_resize_point( nHeight );
            FieldList_t PK; t->enumFields( PK, FieldRelationTypePrimaryKey );
            FieldList_t FK; t->enumFields( FK, FieldRelationTypeForeignKey );
            FieldList_t NN; t->enumFields( NN, FieldRelationTypeNone       );
            t->aggregateFields( PK, FK, NN );
            t->field_recalc();


            // step 2 оптимизируем размеры таблицы и полей по ширине и высоте
            int h1 = t->m_nFirstPos.y() + nHeight * (t->m_FieldList.size() + 2);
            int h0 = t->m_nLastPos.y();

            int w1 = fm.horizontalAdvance( max_text.c_str() ) + 6*HEIGHT_TO_PADDING;
            int w0 = abs( t->m_nLastPos.x() - t->m_nFirstPos.x() );

            QPoint pt = t->m_nResizePos + QPoint( w1 - w0, h1 - h0 );
            t->calc_first_last_point( pt );


            // step 3 смещаем таблицу с полями правее если она пересекается с предыдущими таблицами
            for( unsigned long ri = 0; ri < n; ri ++ )
            {
                auto &prev = class_list->at( ri );
                if( QRect( prev->m_nFirstPos, prev->m_nLastPos ).intersects( QRect( t->m_nFirstPos, t->m_nLastPos ) ) )
                {
                    t->move( t->m_nFirstPos, QPoint( prev->m_nLastPos.x() + DEFAULT_OFFSET_DX, t->m_nFirstPos.y() ) );
                }
            }
        }

        // add relations
        auto relations_list = __AppOptions.getRelationList();
        relations_list->Flush();
        for( unsigned long i = 0; i < class_list->size(); ++ i )
        {
            auto &ti = class_list->at( i );

            FieldList_t PK; ti->enumFields( PK, FieldRelationTypePrimaryKey );

            if( PK.size() > 1 ) continue;
            if( PK.end() != std::find_if( PK.begin(), PK.end(), [](auto pItem){
                return std::string::npos == pItem->m_PhysicalName.find( "_id" )
                    && std::string::npos == pItem->m_PhysicalName.find( "uid" )
                    && std::string::npos == pItem->m_PhysicalName.find( "dbno" )
                    && std::string::npos == pItem->m_PhysicalName.find( "nox" )
                    && std::string::npos == pItem->m_PhysicalName.find( "sox" );
                } ) )
                continue;

            for( unsigned long j = 0; j < class_list->size(); ++ j )
            {
                if( i != j )
                {
                    auto &tj = class_list->at( j );
                    for( unsigned long k = 0; k < tj->m_FieldList.size(); ++ k )
                    {
                        auto f = tj->m_FieldList[ k ];
                        auto it = std::find_if( PK.begin(), PK.end(), [f](auto pItem){ return 0 == pItem->m_PhysicalName.compare( f->m_PhysicalName ); } );
                        if( it != PK.end() )
                        {
                            m_pCanvas->relation_release_add( i, j, k );
                        }
                    }
                }
            }
        }

        // class_list->sort();
        // relations_list->sort();
    }
}

void
MainWindow::on_About()
{
    QString Title   = "About " + QString( APP_NAME );
#ifdef Q_OS_WIN
    QString Content =
        "Product: "  + QString( APP_NAME  ) + " " + QString( APP_VERSION ) + "\n" \
        "Source: https://github.com/pogrebenko/sw_architect\n" \
        "Icons from: https://heroicons.com\n" \
        "\n" \
        "Copyright (C) 2025, pogrebenko";
#endif
#ifdef Q_OS_LINUX
    QString Content =
        "Product: "  + QString( APP_NAME  ) + " " + QString( APP_VERSION ) + "\n" \
        "Built on: " + QString( BUILDDATE ).replace( QChar('_'), QChar(' ') ) + "\n" \
        "Source: https://github.com/pogrebenko/sw_architect\n" \
        "Icons from: https://heroicons.com\n" \
        "\n" \
        "Copyright (C) 2025, pogrebenko";
#endif

    QMessageBox::about( this, Title, Content );
}

void
MainWindow::on_AboutQt()
{
    QMessageBox::aboutQt( this, "About QT" );
}

QPrinter*
MainWindow::initPrinter( QPrinter &Printer )
{
    QMarginsF margins( 12, 16, 12, 20 ); // in Millimeter

    Printer.setPageSize( QPageSize::PageSizeId::A4 );
    Printer.setPageOrientation( QPageLayout::Orientation::Portrait );
    Printer.setPageMargins( margins, QPageLayout::Unit::Millimeter );
    //Printer.setResolution( DEFAULT_PRINTER_DPI );
    Printer.setFullPage( false );
    //Printer.setResolution(QPrinter::HighResolution);
    return &Printer;
}

void
MainWindow::on_Print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
    QPrinter     Printer;
    QPrintDialog PrintDlg( initPrinter( Printer ) );
                 PrintDlg.setMinMax( 1, 1 );
                 PrintDlg.setWindowTitle( "Print project" );
    if( PrintDlg.exec() == QDialog::Accepted )
    {
        printCanvas( &Printer );
    }
#endif
}

void
MainWindow::on_PrintPreview()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printdialog)
    QPrinter            Printer;
    QPrintPreviewDialog PreviewDlg( initPrinter( Printer ) );
                        PreviewDlg.setWindowTitle( "Print preview" );
                        PreviewDlg.setWindowFlags( Qt::WindowTitleHint );
    // QList<QToolBar *> toolbarlist = PreviewDlg.findChildren<QToolBar *>();
    // foreach( auto tool, toolbarlist )
    // {
    //     tool->setDisabled(true);
    // }
    // toolbarlist.first()->addAction( QIcon("icon.png"), "New Action" );

    connect( &PreviewDlg, &QPrintPreviewDialog::paintRequested, this, &MainWindow::printCanvas );

    PreviewDlg.exec();
#endif
}

void
MainWindow::printCanvas( QPrinter *pPrinter )
{
    QPainter painter;

    painter.begin( pPrinter );

    painter.setRenderHints(QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::SmoothPixmapTransform, true);

    // begin center the widget on the page and scale it so that it fits the page >>>>>
#ifdef Q_OS_WIN
    qreal w = pPrinter->pageLayout().paintRectPixels( DEFAULT_PRINTER_DPI ).width ();
    qreal h = pPrinter->pageLayout().paintRectPixels( DEFAULT_PRINTER_DPI ).height();
    double xscale = w/double(m_pCanvas->width ());
    double yscale = h/double(m_pCanvas->height());
    double scale  = qMin(xscale, yscale);
    int x = pPrinter->pageLayout().fullRectPixels( DEFAULT_PRINTER_DPI ).x();
    int y = pPrinter->pageLayout().fullRectPixels( DEFAULT_PRINTER_DPI ).y();
    painter.translate( x + w/2, y + h/2 );
    painter.scale( scale, scale );
    painter.translate( -width()/2, -height()/2 );
#endif
#ifdef Q_OS_LINUX
    double xscale = pPrinter->pageRect().width ()/double(m_pCanvas->width ());
    double yscale = pPrinter->pageRect().height()/double(m_pCanvas->height());
    double scale = qMin(xscale, yscale);
    painter.translate(pPrinter->paperRect().x() + pPrinter->pageRect().width ()/2,
                      pPrinter->paperRect().y() + pPrinter->pageRect().height()/2);
    painter.scale(scale, scale);
    painter.translate(-width()/2, -height()/2);
#endif
    // end >>>>>

    m_pCanvas->draw_figure  ( painter, __AppOptions.getClassList   () );
    m_pCanvas->draw_relation( painter, __AppOptions.getRelationList() );

    painter.resetTransform();

    painter.end();

    // for (int page = 0; page < numberOfPages; ++ page)
    // {
    //     // Use the painter to draw on the page.
    //     if (page != lastPage)
    //         pPrinter->newPage();
    // }

    //#include <QEventLoop>
    //QEventLoop m_waitForResult;
    //m_waitForResult.exec(QEventLoop::ExcludeUserInputEvents);
}

void
MainWindow::adjustForCurrentFile( const QString &filePath )
{
    m_File = filePath;

    QSettings   settings( APP_COMPANY, APP_NAME );
    QStringList recentFilePaths = settings.value( "recentFiles" ).toStringList();

    recentFilePaths.removeAll( filePath );
    recentFilePaths.prepend( filePath );
    while( recentFilePaths.size() > MAXRECENTFILE )
    {
        recentFilePaths.removeLast();
    }
    settings.setValue( "recentFiles", recentFilePaths );

    updateRecentActionList();
}

void
MainWindow::updateRecentActionList()
{
    QSettings   settings( APP_COMPANY, APP_NAME );
    QStringList recentFilePaths = settings.value( "recentFiles" ).toStringList();

    auto itEnd = 0u;
    if( recentFilePaths.size() <= MAXRECENTFILE )
        itEnd = recentFilePaths.size();
    else
        itEnd = MAXRECENTFILE;

    for( auto i = 0u; i < itEnd; ++ i )
    {
        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( recentFilePaths[ i ] ) );

        auto pItem = m_pRecentFileActionList.at( i );
             pItem->setText   ( text );
             pItem->setData   ( recentFilePaths.at( i ) );
             pItem->setVisible( true );
    }

    for( auto i = itEnd; i < MAXRECENTFILE; ++ i )
    {
        m_pRecentFileActionList.at( i )->setVisible(false);
    }
}

QString
MainWindow::strippedName( const QString &fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}

void MainWindow::on_OpenRecentFile()
{
    CRefreshMainWnd w( this );

    QAction *action = qobject_cast<QAction *>( sender() );
    if( action )
    {
        QString sFileName( action->data().toString() );
        LoadFile( sFileName );
    }
}

void
MainWindow::on_ClearRecentFiles()
{
    QStringList recentFilePaths;

    QSettings settings( APP_COMPANY, APP_NAME );
              settings.setValue( "recentFiles", recentFilePaths );

    updateRecentActionList();
}

void MainWindow::on_Undo()
{
    CRefreshMainWnd w( this, "Undo" );
    if( __History.Undo() )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
        QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );
    }
}

void MainWindow::on_Redo()
{
    CRefreshMainWnd w( this, "Redo" );
    if( __History.Redo() )
    {
        CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
        QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );
    }
}

void MainWindow::on_Copy()
{
    CRefreshMainWnd w( this );
    CAppOptions to_copy( __AppOptions );
                to_copy.class_relation_copy();
    QJsonObject obj; ::Write( obj, to_copy );
    QApplication::clipboard()->setText( QString::fromStdString( QJsonDocument( obj ).toJson().toStdString() ) );

    statusBar()->showMessage(
        QString::number( to_copy.getClassList()->size() + to_copy.getRelationList()->size() ) +
            " items copied to clipboard", DEFAULT_MSG_TIMEOUT );
}

void MainWindow::on_Paste()
{
    CRefreshMainWnd w( this );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );

    CAppOptions to_paste;
    QJsonObject obj = QJsonDocument::fromJson( QApplication::clipboard()->text().toUtf8() ).object();
    ::Read( obj, to_paste, APP_DATA_VERSION );

    __History.Save( __PRETTY_FUNCTION__ );

    __AppOptions.clearState( false );
    __AppOptions.class_relation_paste( to_paste );

    QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );

    statusBar()->showMessage(
        QString::number( to_paste.getClassList()->size() + to_paste.getRelationList()->size() )
            + " items loaded from clipboard", DEFAULT_MSG_TIMEOUT );
}

void MainWindow::on_ScaleMinus()
{
    CRefreshMainWnd w( this );
    m_pCanvas->onMinusRatio();
    statusBar()->showMessage( "scale to " + QString::number(__AppOptions.getRatio()*100) + " %" );
}

void MainWindow::on_ScalePlus()
{
    CRefreshMainWnd w( this );
    m_pCanvas->onPlusRatio();
    statusBar()->showMessage( "scale to " + QString::number(__AppOptions.getRatio()*100) + " %" );
}

void MainWindow::on_ScaleReset()
{
    CRefreshMainWnd w( this );
    m_pCanvas->onResetRatio();
    statusBar()->showMessage( "scale reset", DEFAULT_MSG_TIMEOUT );
}

void MainWindow::on_ResetRotate()
{
    CRefreshMainWnd w( this );
    m_pCanvas->onClearRotate();
    statusBar()->showMessage( "rotation reset", DEFAULT_MSG_TIMEOUT );
}

void MainWindow::on_Clear()
{
    CRefreshMainWnd w( this, "clear document" );
    m_pCanvas->onClearDocument();
    m_File.clear();
    __History.Flush();
    setWindowTitle( QString( APP_NAME ) );
}

void MainWindow::on_EditOptions()
{
    OptionDetails dialog( "Options", this );
    if( dialog.exec() == QDialog::Accepted )
    {
        dialog.getData();
        statusBar()->showMessage( "options edited", DEFAULT_MSG_TIMEOUT );
    }
}

void
MainWindow::on_SaveAs()
{
    QString sFile;
    SaveFile( sFile );
}

void
MainWindow::on_Save()
{
    SaveFile( m_File );
}

void
MainWindow::on_Load()
{
    QString sFile;
    LoadFile( sFile );
}

void
MainWindow::SaveFile( QString &sFile )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    if( sFile.isEmpty() )
    {
        sFile = QFileDialog::getSaveFileName(
            this,
            "Save Moving Figures File",
            QDir::homePath(),
            "Moving Figures Files (*.saf);;All Files (*)"
        );
    }
    if( !sFile.isEmpty() )
    {
        QString sError = "";
        this->setCursor( Qt::WaitCursor );
        try
        {
            QFile file( sFile );
            if( file.open( QFile::WriteOnly ) )
            {
                CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );

                __AppOptions.clearState();

                if( __EXPORT_FORMAT__ == ExportTypeJSON )
                {
                    QJsonObject obj; ::Write( obj, __AppOptions );
                    file.write( QJsonDocument( obj ).toJson() );
                }
                else
                {
                    QByteArray Buffer;
                    CIOStreamer ios( APP_DATA_VERSION );
                    ::Write( ios, __AppOptions );
                    Buffer.append( ios.m_Buffer, ios.m_Buffer_Size );
                    file.write( Buffer );
                }

                file.flush();
                file.close();

                __Logger.info( APP_LOG_LEVEL, "info: File saved [%s]", sFile.toLatin1().constData() );

                adjustForCurrentFile( sFile );

                init_ui( &__AppOptions );
                this->update();

                __History.Flush();

                statusBar()->showMessage( "project '" + __AppOptions.getProjectName() + "' saved to file " + sFile , DEFAULT_MSG_TIMEOUT );
            }
            else
            {
                //__Logger.error( APP_LOG_LEVEL, "error: Could not open file: %s", file.errorString().toStdString().data() );
                sError = "Error: Could not open file: %s" + file.errorString();
            }

        }
        catch( ... )
        {
            sError = "Exception error when trying to open a file.";
        }
        this->setCursor( Qt::ArrowCursor );

        if( !sError.isEmpty() )
        {
            QMessageBox::critical( this, "Exception", sError );
        }
    }
}

void
MainWindow::LoadFile( QString &filePath )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    if( filePath.isEmpty() )
    {
        filePath = QFileDialog::getOpenFileName(
            this,
            "Open Moving Figures File",
            QDir::homePath(),
            "Moving Figures Files (*.saf);;All Files (*.*)"
        );
    }

    if( !filePath.isEmpty() )
    {
        QString sError = "";
        this->setCursor(Qt::WaitCursor);
        try
        {
            QFile file( filePath );
            if( file.open( QIODevice::ReadOnly ) )
            {
                CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );

                if( __EXPORT_FORMAT__ == ExportTypeJSON )
                {
                    QJsonObject obj = QJsonDocument::fromJson( file.readAll() ).object();
                    ::Read( obj, __AppOptions, APP_DATA_VERSION );
                }
                else
                {
                    CIOStreamer ios( file.size(), APP_DATA_VERSION );
                    ::memcpy( ios.m_Buffer, (void*)file.readAll().begin(), file.size() );
                    ::Read( ios, __AppOptions, APP_DATA_VERSION );
                }
                __AppOptions.clearState();

                file.close();

                QSize size; m_pCanvas->setMinimumSize( m_pCanvas->figure_size( size, this->size() ) );

                init_ui( &__AppOptions );
                this->update();

                __Logger.info( APP_LOG_LEVEL, "info: File loaded [%s]", filePath.toLatin1().constData() );

                setWindowTitle( QString(APP_NAME) + " | " + __AppOptions.getProjectName() );

                adjustForCurrentFile( filePath );

                __History.Flush();

                statusBar()->showMessage( "project '" + __AppOptions.getProjectName() + "' loaded from file " + filePath, DEFAULT_MSG_TIMEOUT );
            }
            else
            {
                sError = "Error: Could not open file: %s" + file.errorString();

                //std::string sError = file.errorString().toStdString();
                //__Logger.error( APP_LOG_LEVEL, "error: Could not open file: %s", sError.c_str() );
            }
        }
        catch( ... )
        {
            sError = "Exception error when trying to open a file.";
        }
        this->setCursor(Qt::ArrowCursor);

        if( !sError.isEmpty() )
        {
            QMessageBox::critical( this, "Exception", sError );
        }
    }
}

void
MainWindow::closeEvent( QCloseEvent *event )
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );

    exit_or_ignore() ? QCoreApplication::quit() : event->ignore();
}

void
MainWindow::on_Exit()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    if( exit_or_ignore() )
    {
        QCoreApplication::quit();
    }
}

bool
MainWindow::exit_or_ignore()
{
    if( __History.size() > 1 )
    {
        QMessageBox::StandardButton reply =
            QMessageBox::question( this,
                                  "Unsaved changes",
                                  "Your project has unsaved changes",
                                  QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard );
        if( reply == QMessageBox::Save )
        {
            on_Save();
        }
        else
        if( reply == QMessageBox::Cancel )
        {
            return false;
        }
    }
    return true;
}

void MainWindow::on_Rectangle()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    CRefreshMainWnd w( this, "Design rectangle mode seletcted" );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
    __AppOptions.setFigureType(FigureTypeRectangle);
    __AppOptions.setActionType(ActionTypeAddFigure);
}

void MainWindow::on_Ellipse()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    CRefreshMainWnd w( this, "Design ellipse mode selected" );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
    __AppOptions.setFigureType(FigureTypeEllipse);
    __AppOptions.setActionType(ActionTypeAddFigure);
}

void MainWindow::on_Triangle()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    CRefreshMainWnd w( this, "Design triangle mode selected" );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
    __AppOptions.setFigureType(FigureTypeTriangle);
    __AppOptions.setActionType(ActionTypeAddFigure);
}

void MainWindow::on_Relation()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    CRefreshMainWnd w( this, "Relation mode selected" );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
    __AppOptions.setRelationType(RelationTypeLineNondirect);
    __AppOptions.setActionType(ActionTypeAddRelation);
}

void MainWindow::on_Move()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    CRefreshMainWnd w( this, "Move/Resize/Rotate mode selected" );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
    __AppOptions.setActionType(ActionTypeMove);
}

void MainWindow::on_Delete()
{
    FUNC_TRACE( &__Logger, __PRETTY_FUNCTION__ );
    CRefreshMainWnd w( this, "Delete mode selected" );
    CRefreshCanvas v( __PRETTY_FUNCTION__, m_pCanvas, &__History );
    __AppOptions.setActionType(ActionTypeDelete);
}

void MainWindow::refresh( const char *sMsg )
{
    init_ui( &__AppOptions );
    update();
    if( sMsg != nullptr )
    {
        statusBar()->showMessage( sMsg, DEFAULT_MSG_TIMEOUT );
    }
}
