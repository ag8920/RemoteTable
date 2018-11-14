#ifndef TABLERS232_H
#define TABLERS232_H

#include "../comport/comport.h"
#include <QObject>

class tableRS232 : public QObject
{
    Q_OBJECT
public:
    explicit tableRS232(QObject *parent = nullptr);

signals:
    void OutputToComPort(const QByteArray &data);
public slots:
    void ExecutePos();
    void ZeroPosition();
    void GetPosition();
    void GoToPosition(QVariant position);
    void RequestPosition();
    void OnMotion();
    void OffMotion();
    void BeginMotion();
    void StopMotion();
    void FinishedMotion();
    void ResetAbsCoord();

    void SendCMD(const QString &cmd);

    void SetSpeed(const QString &str);
    void SetAngle(const QString &str);
    void SetTypePositioning (bool pos);

    void AddThreads();
private:
    comPort *DeviceComPort;
    QThread *ComPortThread;

    QString speed;
    QString angle;
    QString typePositioning;

    int nextPosition;
};

#endif // TABLERS232_H
