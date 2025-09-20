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
    QLabel           *Label_DatabaseName;
    QLabel           *Label_TablePrefix ;
    QLabel           *Label_ClassPrefix ;

    QLineEdit        *Edit_ProjectName ;
    QLineEdit        *Edit_DatabaseName;
    QLineEdit        *Edit_TablePrefix ;
    QLineEdit        *Edit_ClassPrefix ;

    QDialogButtonBox *buttonBox;
};

#endif // OPTIONDETAILS_H
