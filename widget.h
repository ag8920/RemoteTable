#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QWidget>
#include <QtGui>
#include <QMenu>
#include <QLabel>
#include <QLineEdit>

class Widget : public QMainWindow
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
signals:

protected:

public slots:

private:
    void CreateActions();
    void CreateMenus();
    void CreateToolBars();
    void CreateStatusBar();
    void CreateWidgets();

    QAction *DeltaPsProtocol;
    QAction *Rate2Protocol;
    QAction *DadvttProtocol;

    QMenu *fileMenu;
    QMenu *remoteMenu;

    QLabel *currValueLabel;
    QLabel *meanValueLabel;
    QLabel *minValueLabel;
    QLabel *maxValueLabel;
    QLabel *skoLabel;

    QLabel *timeAccumulateLabel;
    QLabel *azimuthMeasureLabel;

    QLineEdit *currValueLineEdit;
    QLineEdit *meanVelueLineEdit;
    QLineEdit *minValueLineEdit;
    QLineEdit *maxValueLineEdit;
    QLineEdit *skoLineEdit;

    QLineEdit *timeAcculateLineEdit;
    QLineEdit *azimuthMeasureLineEdit;

    QWidget *measureWidget;

};

#endif // WIDGET_H
