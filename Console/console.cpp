#include "console.h"

#include <QScrollBar>
#include <QScrollArea>
#include <QtCore/QDebug>
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
}
//-----------------------------------------------------------
// Назначение:
//-----------------------------------------------------------
void Console::putData(const QByteArray &data)
{
    QString str=static_cast<QString>(data);

    insertPlainText(data.toHex());
    insertPlainText("\n");
    QScrollBar *bar=verticalScrollBar();
    bar->setValue(bar->maximum());

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
