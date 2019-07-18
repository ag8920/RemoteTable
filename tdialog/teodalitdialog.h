#ifndef TEODALITDIALOG_H
#define TEODALITDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class teodalitDialog;
}

class teodalitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit teodalitDialog(QWidget *parent = nullptr);
    ~teodalitDialog();
signals:
    void sendValue(QByteArray data);
private slots:
    void accept();
private:
    void readSettings();
    double PaKRight;
    double PaKLeft;
    double PvKRight;
    double PvKLeft;
    double PaPv[4];
    std::vector<QLineEdit*> lineEdits;
    Ui::teodalitDialog *ui;
};

#endif // TEODALITDIALOG_H
