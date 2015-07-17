#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QVector>
#include <QTextStream>

#include "PositionDispatcher.h"
#include "common/MavUtils.h"
#include "MavState.h"
#include "NatNetReceiver.h"
#include "global.h"
#define DISPATCH_INTERVAL 100 //ms

namespace g {
    MavState state;
    MavState setPoint;
    MavState platform;

}

PositionDispatcher::PositionDispatcher(QObject *parent) :
    QObject(parent),
    _serial(this)
{
    _serial.setPortName("/dev/ttyUSB0");

    // Open serial port

    if(!_serial.open(QSerialPort::ReadWrite)) {
        qCritical() << "Error opening serial port";
        emit finished();
        return;
    }

    _serial.setBaudRate(QSerialPort::Baud57600);
    _serial.setFlowControl(QSerialPort::NoFlowControl);

    // Start dispatch time counter
    _dispatchTime.start();

    qDebug() << "Connected to MavLink Radio...";
}

PositionDispatcher::~PositionDispatcher()
{
    _serial.close();
    qDebug() << "Serial Port closed";
}


void PositionDispatcher::sendPosition()
{
    mavlink_message_t msg1;
    mavlink_message_t msg2;

    if(_dispatchTime.elapsed() >= DISPATCH_INTERVAL){

        float roll, pitch, yaw;
        g::state.orientation2(&roll, &pitch, &yaw);

        // ADD CONVERSION IN LOCAL FRAME HERE

        // Pack the message
        mavlink_msg_vision_position_estimate_pack(
                    1,
                    MAV_COMP_ID_ALL, &msg1,
                    (uint64_t) g::state.timestamp * 1000,
                    g::state.x(),
                    g::state.y(),
                    g::state.z(),
                    roll, //rad
                    pitch, //rad
                    yaw); //rad

        _sendMavlinkMessage(&msg1);



//        qDebug() << "Sent now     :     " << g::state.x() << "\t"
//                             <<  g::state.y() << "\t"
//                             << g::state.z()<< "\t"
//                             ;



        mavlink_msg_vicon_position_estimate_pack(
                    1,
                    MAV_COMP_ID_ALL, &msg2,
                    (uint64_t) g::state.timestamp * 1000,
                    g::setPoint.x(),
                    g::setPoint.y(),
                    g::setPoint.z(),

                    5, //rate saturation
                    0.5, //Rate gain

                    g::setPoint.yaw()); //rad

         _sendMavlinkMessage(&msg2);
         //qDebug() << "Sent position target";


        _dispatchTime.restart();


    }

}

void PositionDispatcher::_sendMavlinkMessage(mavlink_message_t *msg)
{
    // Initialize the required buffers
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    // Copy the message to the send buffer
    uint16_t len = mavlink_msg_to_send_buffer(buf, msg);

    // Send the message
    QByteArray sendBuff((const char*)buf, len);
    int written = _serial.write(sendBuff);

    //Wait for data to be sent
    if(!_serial.waitForBytesWritten(500)) {
        qCritical() << "Serial unable to send data";
        emit finished();
    }

}
