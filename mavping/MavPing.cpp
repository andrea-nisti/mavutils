#include "MavPing.h"
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QVector>
#include <QTextStream>

#include <ctime>

#include <mavlink/common/mavlink.h>

inline uint64_t getTimeNanoSecond() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    //gettimeofday(&tv, NULL);
    return ts.tv_sec * (uint64_t)1000000000 + ts.tv_nsec;
}

MavPing::MavPing(QObject *parent) :
    QObject(parent)
{
}

void MavPing::run(){

    uint8_t c;

    const int nTrials = 10;

    QSerialPort serial(this);
    serial.setPortName("/dev/ttyUSB0");
    serial.setBaudRate(QSerialPort::Baud57600);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    QVector<uint64_t> rttVect;

    QTextStream qout(stdout);

    mavlink_message_t msg;
    mavlink_status_t status;

    // Initialize the required buffers
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    mavlink_system_t mavlink_system;

    mavlink_system.sysid = 255;
    mavlink_system.compid = MAV_COMP_ID_SYSTEM_CONTROL;
    mavlink_system.type = MAV_TYPE_GCS;

    // Open serial port

    if(!serial.open(QSerialPort::ReadWrite)) {
        qCritical() << "Error opening serial port";
        emit finished();
        return;
    }

    qout << "Started" << endl;

    for(int i = 0; i < nTrials; ++i) {

        // Pack the message
        m_startTime = getTimeNanoSecond();
        mavlink_param_request_read_t requestBody;

        requestBody.target_system = 1;
        requestBody.target_component = MAV_COMP_ID_ALL;
        requestBody.param_index = -1;
        strncpy(requestBody.param_id, "SERIAL1_BAUD\0\0\0\0", 16);

        mavlink_msg_param_request_read_encode(mavlink_system.sysid, mavlink_system.compid, &msg, &requestBody);

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

       qout << "Request sent.... (" << written << " bytes of "<< len << ")" << endl;

        //Wait for answer
        bool gotAnswer = false;
        do {

            if(!serial.waitForReadyRead(5000)) {
                qCritical() << "Serial timeout";
                break;
            }

            // Get PING answer
            while(serial.bytesAvailable() > 0) {

                serial.read((char*)&c, 1);

                // Try to get a new message
                if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
                    // Handle message

                    if(msg.msgid == MAVLINK_MSG_ID_PARAM_VALUE) {
                        mavlink_param_value_t answer;
                        mavlink_msg_param_value_decode(&msg, &answer);
                        m_finishTime = getTimeNanoSecond();

                        qout << "PARAM reply from " << msg.sysid << " -> ";
                        qout << answer.param_id << " = " << answer.param_value << endl;

                        qout << "RTT: " << m_finishTime - m_startTime << " ns" << endl;
                        rttVect.push_back(m_finishTime - m_startTime);
                        gotAnswer = true;
                    }
                }

            } // Reading While

        } while(!gotAnswer);
    } // for ping number

    // Update global packet drops counter
    //packet_drops += status.packet_rx_drop_count;

    double latency = 0.0;
    foreach(double value,  rttVect){
        latency += static_cast<float>(value);
    }
    latency /= (2 * nTrials * 1E6);

    struct timespec ts;
    clock_getres(CLOCK_MONOTONIC, &ts);
    float resolution =  (ts.tv_sec * (uint64_t)1000000000 + ts.tv_nsec);

    qout << "------------- Stats ---------------\n"
         << "Timer resolution = " <<  resolution << "ns\n"
         << "Link latency = " << latency << "ms" << endl;
    serial.close();
    emit finished();
}


//Send PING command

// Pack the message
//    m_startTime = getTimeMicroSecond();
//    mavlink_msg_ping_pack(mavlink_system.sysid, mavlink_system.compid, &msg, m_startTime, 1, 0, MAV_COMP_ID_ALL);

//    // Copy the message to the send buffer
//    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

//    // Send the message
//    QByteArray sendBuff((const char*)buf, len);
//    int written = serial.write(sendBuff);

//    //Wait for data to be sent
//    if(!serial.waitForBytesWritten(500)) {
//        qDebug() << "Serial unable to send data";
//        emit finished();
//        return;
//    }

//    qDebug() << "Ping.... (" << written << "bytes of "<< len << ")";


//    mavlink_param_request_read_t paramRequestRead;
//    paramRequestRead.target_system = 1;
//    paramRequestRead.target_component = 0;
//    paramRequestRead.param_index
//    mavlink_msg_param_request_read_encode();
