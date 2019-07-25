#ifndef TEODALITDIALOG_H
#define TEODALITDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include "../degwgt/degwidget.h"

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
    void reject();
private:
    void readSettings();
    void writeSettings();
    inline double convertDegrees(const double &deg, const double &min, const double &sec);
    double PaKRight;
    double PaKLeft;
    double PvKRight;
    double PvKLeft;
    double PaPv[4];
    std::vector<QLineEdit*> lineEdits;
    Ui::teodalitDialog *ui;
    degWidget *PaKRwgt;
    degWidget *PaKLwgt;
    degWidget *PvKRwgt;
    degWidget *PvKLwgt;
    std::vector<degWidget*> degEdits;
};

#endif // TEODALITDIALOG_H
