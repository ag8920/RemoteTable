//------------------------------------------------------------------------------
//     Данный модуль создает виджет консоли
//     Автор: Щербаков Александр
//     дата создания: 13.09.2018
//
//------------------------------------------------------------------------------
#include "console.h"

#include <QScrollBar>
#include <QScrollArea>
#include <QtCore/QDebug>

enum { HEX=1,DEC,OCT,BIN,ASCII,};
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
Console::Console(QWidget *parent):QPlainTextEdit(parent),localEchoEnabled(false)
{
    document()->setMaximumBlockCount(100);
    QPalette p=palette();
    p.setColor(QPalette::Base,Qt::black);
    p.setColor(QPalette::Text,Qt::green);
    setPalette(p);
    this->FormatData=HEX;
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::putData(const QByteArray &data)
{
    QString str=static_cast<QString>(data);

    switch (this->FormatData) {
    case HEX:
        insertPlainText(data.toHex());
        insertPlainText("\n");
        break;
    case ASCII:
        insertPlainText(static_cast<QString>(data).toLocal8Bit());
//        insertPlainText(data);
        break;
    default:
        break;
    }
//    insertPlainText(data.toHex());
//    insertPlainText("\n");
    QScrollBar *bar=verticalScrollBar();
    bar->setValue(bar->maximum());

}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::SetFormat(const int Format)
{
    FormatData=Format;
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled=set;
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);
        emit getData(e->text().toLocal8Bit());
    }
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
