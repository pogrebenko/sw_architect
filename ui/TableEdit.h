/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#pragma once
#ifndef TABLEEDIT_H
#define TABLEEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class CTableEdit : public QTextEdit
{
public:

    explicit CTableEdit(QWidget *parent = nullptr);
    explicit CTableEdit(const QString &text, QWidget *parent = nullptr);

protected:

    void keyPressEvent( QKeyEvent *event ) override;
};

#endif // TABLEEDIT_H
