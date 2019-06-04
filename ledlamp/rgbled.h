#ifndef RGBLED_H
#define RGBLED_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QGradient>
#include <QTimer>

class RgbLed : public QWidget
{
    Q_OBJECT

public:
    RgbLed(QWidget *parent = 0);
    RgbLed(QWidget *parent = 0,QColor color=Qt::red);
    ~RgbLed();
public slots:
    void setColor(QColor color);
protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

    QColor ledColor;
    QColor lightColor;
    QColor shadowColor;
    QColor ringShadowDarkColor;
    QColor ringShadowMedColor;
    QColor ringShadowLightColor;
    QColor topReflexUpColor;
    QColor topReflexDownColor;
    QColor bottomReflexCenterColor;
    QColor bottomReflexSideColor;
private:
    void drawLed(const QColor &color);

    int height;
    int width;
    int minDim;
    int half;
    int centerX;
    int centerY;
    QRect drawingRect;
    QTimer *tmr;

    int outerBorderWidth;
    int innerBorderWidth;
    int outerBorderRadius;
    int innerBorderRadius;
    int topReflexY;
    int bottomReflexY;
    int topReflexWidth;
    int topReflexHeight;
    int bottomReflexWidth;
    int bottomReflexHeight;
};

#endif // RGBLED_H
