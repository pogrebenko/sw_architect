/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef OPTIONDETAILS_H
#define OPTIONDETAILS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>

class OptionDetails : public QDialog
{
    Q_OBJECT
public:
    OptionDetails(const QString &title, QWidget *parent);

public:
    void setData();
    void getData();

private:
    QLabel           *Label_ProjectName ;
    //QLabel           *Label_DatabaseName;
    QLabel           *Label_TablePrefix ;
    QLabel           *Label_ClassPrefix ;
    QLabel           *Label_nDB         ;
    QLabel           *Label_ServerName  ;
    QLabel           *Label_Port        ;
    QLabel           *Label_BaseName    ;
    QLabel           *Label_UserName    ;
    QLabel           *Label_UserPass    ;
    QLabel           *Label_SysAdmin    ;

    QLineEdit        *Edit_ProjectName  ;
    //QLineEdit        *Edit_DatabaseName ;
    QLineEdit        *Edit_TablePrefix  ;
    QLineEdit        *Edit_ClassPrefix  ;
    QComboBox        *Combo_nDB         ;
    QLineEdit        *Edit_ServerName   ;
    QLineEdit        *Edit_Port         ;
    QLineEdit        *Edit_BaseName     ;
    QLineEdit        *Edit_UserName     ;
    QLineEdit        *Edit_UserPass     ;
    QLineEdit        *Edit_SysAdmin     ;

    QDialogButtonBox *buttonBox;
};

#endif // OPTIONDETAILS_H
