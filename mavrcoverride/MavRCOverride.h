#ifndef MAVRCOVERRIDE_H
#define MAVRCOVERRIDE_H

#include <QObject>
#include <QtSerialPort>
#include <QTime>
#include <stdint.h>

#include <mavlink/common/mavlink.h>

class MavRCOverride : public QObject
{
    Q_OBJECT
public:
    explicit MavRCOverride(QObject *parent = 0);

signals:
    void finished();

public slots:
    void run();

private:
    mavlink_system_t m_mavlink_system;
    void m_prepare_throttle_override(mavlink_message_t* msg, uint16_t ppmValue);
    void m_prepare_yaw_override(mavlink_message_t* msg, uint16_t ppmValue);

};

#endif // MAVRCOVERRIDE_H
