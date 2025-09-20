/*
Project:Software Architect
File: RelationDetails.h
Licensee: MIT
Source: https://github.com/pogrebenko/sw_architect
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef RELATIONDETAILS_H
#define RELATIONDETAILS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>

class __Relation;

class CRelationDetails : public QDialog
{
    Q_OBJECT
public:
    CRelationDetails(const QString &title, __Relation *pItem, QWidget *parent);

public:
    void setData();
    void getData();

private:
    QLabel           *Label_Name              ;
    QLabel           *Label_PKTableLabel      ;
    QLabel           *Label_FKTableLabel      ;

    QLineEdit        *Edit_Name               ;
    QLineEdit        *Edit_PKTableLabel       ;
    QLineEdit        *Edit_FKTableLabel       ;

    QLabel           *Label_nType             ;
    QComboBox        *ComboBox_nType          ;

    QLabel           *Label_nCardinalityPKTable   ;
    QComboBox        *ComboBox_nCardinalityPKTable;

    QLabel           *Label_nCardinalityFKTable   ;
    QComboBox        *ComboBox_nCardinalityFKTable;

    QLabel           *Label_nDeferrability        ;
    QComboBox        *ComboBox_nDeferrability     ;

    QLabel           *Label_nUpdateRule           ;
    QComboBox        *ComboBox_nUpdateRule        ;

    QLabel           *Label_nDeleteRule           ;
    QComboBox        *ComboBox_nDeleteRule        ;

    QDialogButtonBox *buttonBox;

    __Relation       *m_pItem;
};

#endif // RELATIONDETAILS_H
