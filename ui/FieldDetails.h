/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef FIELDDETAILS_H
#define FIELDDETAILS_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>

class __Field;

class FieldDetails : public QDialog
{
    Q_OBJECT
public:
    FieldDetails(const QString &titlec, __Field *pItem, QWidget *parent);

public:
    void setData();
    void getData();

private:
    QLabel           *Label_LogicalName;
    QLabel           *Label_PhysicalName;

    QLineEdit        *Edit_LogicalName;
    QLineEdit        *Edit_PhysicalName;

    QCheckBox        *CheckBox_bPrimaryKey;
    QCheckBox        *CheckBox_bForeignKey;

    QLabel           *Label_nFieldDataType;
    QComboBox        *ComboBox_nFieldDataType;

    QLabel           *Label_nPrecission;
    QLineEdit        *Edit_nPrecission;

    QLabel           *Label_nScale;
    QLineEdit        *Edit_nScale;

    QCheckBox        *CheckBox_bAllowsNulls;
    QCheckBox        *CheckBox_bAutoIncrement;

    QLabel           *Label_DefaultValue;
    QLineEdit        *Edit_DefaultValue;

    QLabel           *Label_Remarks;
    QTextEdit        *Edit_Remarks;

    QDialogButtonBox *buttonBox;

    __Field          *m_pItem;
};

// FieldRelationType_t m_nFieldRelationType;
// FieldDataType_t     m_nFieldDataType    ;
// int                 m_nPrecission       ;
// int                 m_nScale            ;
// bool                m_bAllowsNulls      ;
// bool                m_bAutoIncrement    ;
// std::string         m_DefaultValue      ;
// std::string         m_Remarks           ;

#endif // FIELDDETAILS_H
