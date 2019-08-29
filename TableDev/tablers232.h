#ifndef TABLERS232_H
#define TABLERS232_H

#include "../comport/comport.h"
#include <QObject>

class tableRS232 : public QObject
{
    Q_OBJECT
public:
    explicit tableRS232(QObject *parent = nullptr);

    QString getSpeed() const;
    void setSpeed(const QString &value);

    QString getAngle() const;
    void setAngle(const QString &value);

    QString getTypePositioning() const;
    void setTypePositioning(const QString &value);

    inline void SendCMD(const QString &cmd);

    QString getDirrection() const;
    void setDirrection(const QString &value);

signals:
    void OutputToComPort(const QByteArray &data);
public slots:
    void initMotion();
    void ExecutePos();
    void ZeroPosition();
    QString GetPosition(const QByteArray &data);
    void GoToPosition(QVariant position);
    void RequestPosition();
    void OnMotion();
    void OffMotion();
    void BeginMotion();
    void StopMotion();
    void FinishedMotion();
    void ResetAbsCoord();
    void DirectionTurn();




//    void AddThreads();
private:

    QString speed;
    QString angle;
    QString typePositioning;
    QString dirrection;

    int nextPosition;


};

#endif // TABLERS232_H
