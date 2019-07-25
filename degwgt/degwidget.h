#ifndef DEGWIDGET_H
#define DEGWIDGET_H

#include <QWidget>
#include <QLineEdit>


namespace Ui {
class degWidget;
}

class degWidget : public QWidget
{
    Q_OBJECT

public:
    explicit degWidget(QWidget *parent = nullptr);
    ~degWidget();
public:
    void setDeg(double degrees);
    double getDegrees() const;

private slots:
    void changedDeg();

private:
    Ui::degWidget *ui;
    int deg;
    int min;
    double sec;

    double degrees;

};

#endif // DEGWIDGET_H
