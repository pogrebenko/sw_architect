/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef CLASSDETAILS_H
#define CLASSDETAILS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

class __Class;

class ClassDetails : public QDialog
{
    Q_OBJECT
public:
    ClassDetails(const QString &title, __Class *pItem, QWidget *parent);

public:
    void setData();
    void getData();

private:
    QLabel           *Label_LogicalName;
    QLabel           *Label_PhysicalName;

    QLineEdit        *Edit_LogicalName;
    QLineEdit        *Edit_PhysicalName;

    QDialogButtonBox *buttonBox;

    __Class          *m_pItem;
};

#endif // CLASSDETAILS_H
