#ifndef CONSOLE_H
#define CONSOLE_H

#include <QObject>

#include <QPlainTextEdit>


class Console:public QPlainTextEdit
{
    Q_OBJECT
public slots:
    void putData(const QByteArray &data);
    void SetFormat(const int Format);
signals:
    void getData(const QByteArray &data);

public:
    explicit Console(QWidget *parent=nullptr);
    void setLocalEchoEnabled(bool set);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    bool localEchoEnabled;
    int FormatData;
};

#endif // CONSOLE_H
