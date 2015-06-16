#include "MavRCOverride.h"
#include "common/MavUtils.h"

#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QVector>
#include <QTextStream>

void MavRCOverride::m_prepare_throttle_override(mavlink_message_t* msg, uint16_t ppmValue) {

    // PPM range from experimental evidence: 1096-2017 usDataHandler

    mavlink_rc_channels_override_t rcBody;

    rcBody.target_system = 1;
    rcBody.target_component = MAV_COMP_ID_ALL;
    rcBody.chan1_raw = 0;
    rcBody.chan2_raw = 0;
    rcBody.chan3_raw = ppmValue;
    rcBody.chan4_raw = 0;
    rcBody.chan5_raw = 0;
    rcBody.chan6_raw = 0;
    rcBody.chan7_raw = 0;
    rcBody.chan8_raw = 0;

    mavlink_msg_rc_channels_override_encode(m_mavlink_system.sysid, m_mavlink_system.compid, msg, &rcBody);
}

void MavRCOverride::m_prepare_yaw_override(mavlink_message_t* msg, uint16_t ppmValue) {

    // PPM range from experimental evidence: 1096-2017 us

    mavlink_rc_channels_override_t rcBody;

    rcBody.target_system = 1;
    rcBody.target_component = MAV_COMP_ID_ALL;
    rcBody.chan1_raw = 0;
    rcBody.chan2_raw = 0;
    rcBody.chan3_raw = 0;
    rcBody.chan4_raw = ppmValue;
    rcBody.chan5_raw = 0;
    rcBody.chan6_raw = 0;
    rcBody.chan7_raw = 0;
    rcBody.chan8_raw = 0;

    mavlink_msg_rc_channels_override_encode(m_mavlink_system.sysid, m_mavlink_system.compid, msg, &rcBody);
}

MavRCOverride::MavRCOverride(QObject *parent) :
    QObject(parent)
{
    m_mavlink_system.sysid = 255;
    m_mavlink_system.compid = MAV_COMP_ID_SYSTEM_CONTROL;
    m_mavlink_system.type = MAV_TYPE_GCS;
}

void MavRCOverride::run(){

    QSerialPort serial(this);
    serial.setPortName("/dev/ttyUSB0");
    serial.setBaudRate(QSerialPort::Baud57600);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    QTextStream qout(stdout);
    mavlink_message_t msg;

    uint16_t throttleValue;
    // Initialize the required buffers
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    // Open serial port

    if(!serial.open(QSerialPort::ReadWrite)) {
        qCritical() << "Error opening serial port";
        emit finished();
        return;
    }

    qout << "Started" << endl;

    uint64_t startTime = getTimeNanoSecond();

    // Arm
    forever {
        if(getTimeNanoSecond() < startTime + (uint64_t)5000000000) {
            throttleValue = 1600;
        }
        else {
            throttleValue = 0;
        }

        // Prepare RC_Override message
        m_prepare_throttle_override(&msg, throttleValue);
        //m_prepare_yaw_override(&msg, throttleValue);


        // Copy the message to the send buffer
        uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

        // Send the message
        QByteArray sendBuff((const char*)buf, len);
        int written = serial.write(sendBuff);

        //Wait for data to be sent
        if(!serial.waitForBytesWritten(500)) {
            qCritical() << "Serial unable to send data";
            emit finished();
            break;
        }

       qout << "Override Message sent: RC=" << throttleValue << endl;
       QThread::msleep(40);

    }

    serial.close();
    emit finished();
}
