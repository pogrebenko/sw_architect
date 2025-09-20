/*
Project: Software Architect
Source: https://github.com/pogrebenko/sw_architect
Licensee: MIT
Copyright (C) 2025, pogrebenko
*/
#include <QtWidgets>

#include "TableEdit.h"

#include "canvas.h"

CTableEdit::CTableEdit(QWidget *parent )
: QTextEdit( parent )
{
}

CTableEdit::CTableEdit(const QString &text, QWidget *parent )
: QTextEdit( text, parent )
{
}

void
CTableEdit::keyPressEvent( QKeyEvent *event )
{
    if(    event->key() == Qt::Key_Return
        || event->key() == Qt::Key_Enter
        || event->key() == Qt::Key_Escape
    )
    {
        CCanvas *pParent = (CCanvas*)parentWidget();
        pParent->hideNameEdit();
        pParent->draw_figure_relation();
        pParent->update();

        event->ignore();
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
}
