#ifndef JUSTDIALOG_H
#define JUSTDIALOG_H

#include <QDialog>

namespace Ui {
class justDialog;
}
class justDialog : public QDialog
{
    Q_OBJECT

public:
    explicit justDialog(QWidget *parent = nullptr);
    ~justDialog();
signals:
    void sendValue(QByteArray data);
private slots:
    void accept();
    void reject();
    void input(const QString &text);
private:
    void readSettings();
    double dvalue[4];
    int value;
    Ui::justDialog *ui;
};

#endif // DIALOG_H
