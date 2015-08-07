
#pragma once
#include <QObject>
#include <QtSerialPort>
#include <QTime>
#include <stdint.h>
#include<QMutex>
#include <mavlink/common/mavlink.h>



class PositionDispatcher : public QObject
{
    Q_OBJECT
public:
    explicit PositionDispatcher(QObject *parent = 0);
    ~PositionDispatcher();

signals:
    void finished();

public slots:
    void sendPosition();


private:
    QSerialPort _serial;
    QTime _dispatchTime;
    double _xSp;
    double _ySp;
    void setSp(double x, double y);
    void _sendMavlinkMessage(mavlink_message_t *msg);

    QMutex m;
};
