/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class CAppOptions;
class QPrinter;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public :
     MainWindow( QWidget *parent = nullptr );
    ~MainWindow();

    void refresh( const char *sMsg = nullptr );

protected :
    void setup_ui();
    void init_ui( CAppOptions *pAppOptions );
    void delete_ui();
    void SaveFile( QString &File);
    void LoadFile( QString &File);

    void      adjustForCurrentFile( const QString& filePath );
    void      updateRecentActionList();
    QString   strippedName( const QString &fullFileName );

    QPrinter* initPrinter( QPrinter &Printer );

private slots :
     void on_Save();
     void on_SaveAs();
     void on_Load();
     void on_Print();
     void on_PrintPreview();
     void on_Exit();
     void on_ClearRecentFiles();

     void on_Rectangle();
     void on_Ellipse();
     void on_Triangle();

     void on_Relation();
     void on_Delete();
     void on_Move();

     void on_Undo();
     void on_Redo();
     void on_AlignHorizontalyLeft();
     void on_AlignVerticallyUp();
     void on_AlignHorizontalyRight();
     void on_AlignVerticallyDown();
     void on_Copy();
     void on_Paste();
     void on_ScaleMinus();
     void on_ScalePlus();
     void on_ScaleReset();

     void on_ResetRotate();
     void on_Clear();

     void on_EditOptions();

     void on_OpenRecentFile();

     void on_ExpotToSQL();
     void on_ImportFromSQL();

     void on_About();
     void on_AboutQt();

     void printCanvas( QPrinter *pPrinter );

private :
    Ui::MainWindow *ui;

    QAction        *actionSave
                 , *actionSaveAs
                 , *actionLoad
                 , *actionPrint
                 , *actionPrintPreview
                 , *actionExit
                 , *actionClear
                 , *actionUndo
                 , *actionRedo
                 , *actionHorizontalyLeft
                 , *actionVerticallyUp
                 , *actionHorizontalyRight
                 , *actionVerticallyDown
                 , *actionCopy
                 , *actionPaste
                 , *actionSettings
                 , *actionExpotToSQL
                 , *actionImportFromSQL
                 , *actionAbout
                 , *actionAboutQt
                 , *actionZoomIn
                 , *actionZoomOut
                 , *actionActual;

    CCanvas        *m_pCanvas;

    QString         m_File;
    QMenu          *m_pRecentFilesMenu;
    QList<QAction*> m_pRecentFileActionList;
    QMenu          *m_pZoomMenu, *m_pAlignMenu;
};

#endif // MAINWINDOW_H
