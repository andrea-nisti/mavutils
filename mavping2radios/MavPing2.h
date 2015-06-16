#pragma once

#include <QObject>
#include <QtSerialPort>
#include <QTime>
#include <stdint.h>

#include <mavlink/common/mavlink.h>

#define PINGCOUNT 200

class MavPing2 : public QObject
{
    Q_OBJECT
public:
    explicit MavPing2(QObject *parent = 0);
    ~MavPing2();

signals:
    void finished();
    void receivedAll();

public slots:
    void run();
    void receivePing();
    void sendPing();
    void printStats();

private:
    int m_countSent;
    int m_lastRcvd;
    QSerialPort m_tx, m_rx;
    QVector<quint64> m_sendTimes, m_recvTimes;
    mavlink_system_t m_mavlink_system;

    QTimer m_sendTimer;

    void m_prepare_throttle_override(mavlink_message_t* msg, uint16_t ppmValue);

};
