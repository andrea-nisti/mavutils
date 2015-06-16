#include "MavPing2.h"
#include "common/MavUtils.h"

#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QVector>
#include <QTextStream>

static QTextStream qout(stdout);

MavPing2::MavPing2(QObject *parent) :
    QObject(parent),
    m_tx(this),
    m_rx(this),
    m_sendTimes(PINGCOUNT),
    m_recvTimes(PINGCOUNT),
    m_sendTimer(this)
{
    m_countSent = 0;
    m_lastRcvd = 0;

    m_mavlink_system.sysid = 255;
    m_mavlink_system.compid = MAV_COMP_ID_SYSTEM_CONTROL;
    m_mavlink_system.type = MAV_TYPE_GCS;

    connect(&m_rx, SIGNAL(readyRead()), this, SLOT(receivePing()));
    connect(this, SIGNAL(receivedAll()), this, SLOT(printStats()));
}

void MavPing2::m_prepare_throttle_override(mavlink_message_t* msg, uint16_t ppmValue)
{
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

void MavPing2::run()
{
    m_tx.setPortName("/dev/ttyUSB0");
    m_tx.setBaudRate(QSerialPort::Baud57600);
    m_tx.setFlowControl(QSerialPort::NoFlowControl);

    if(!m_tx.open(QIODevice::WriteOnly)) {
        qCritical() << "Error opening sending serial port";
        emit finished();
        return;
    }

    m_rx.setPortName("/dev/ttyUSB1");
    m_rx.setBaudRate(QSerialPort::Baud57600);
    m_rx.setFlowControl(QSerialPort::NoFlowControl);

    if(!m_rx.open(QIODevice::ReadOnly)) {
        qCritical() << "Error opening receiving serial port";
        emit finished();
        return;
    }

    // Initialize the required buffers
    qout << "Started" << endl;

    m_sendTimer.setInterval(40);
    m_sendTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_sendTimer, SIGNAL(timeout()), this, SLOT(sendPing()));
    m_sendTimer.start();
}

MavPing2::~MavPing2()
{
    m_sendTimer.stop();
    m_tx.close();
    m_rx.close();
}

void MavPing2::sendPing()
{
    mavlink_message_t msg;
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];

    if(m_countSent >= PINGCOUNT) {
       // Done!
       m_sendTimer.stop();
       return;
    }

    m_sendTimes[m_countSent] = getTimeNanoSecond();

    // Prepare RC_Override message
    m_prepare_throttle_override(&msg, 1000 + m_countSent);


    // Copy the message to the send buffer
    uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

    // Send the message
    QByteArray sendBuff((const char*)buf, len);
    int written = m_tx.write(sendBuff);

    //Wait for data to be sent
    if(!m_tx.waitForBytesWritten(500)) {
        qCritical() << "Serial unable to send data";
        emit finished();
    }

   qout << "[" << (int)(m_sendTimes.at(m_countSent) * 1E-9) << "]" << "\tSent " << m_countSent << endl;
   m_countSent++;

}

void MavPing2::receivePing()
{
    char c;
    int seq = -1;

    mavlink_message_t msg;
    mavlink_status_t status;
    mavlink_rc_channels_override_t msgDecoded;

    while(m_rx.bytesAvailable() > 0){
        m_rx.read(&c, 1);

        // Try to get a new message
        if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
            // Handle message
            if(msg.msgid == MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE ) {
                mavlink_msg_rc_channels_override_decode(&msg, &msgDecoded);
                seq = msgDecoded.chan3_raw - 1000;

                m_recvTimes[seq] = getTimeNanoSecond();
                qout << "[" << (int) (m_recvTimes.at(seq) * 1E-9) << "]" << "\tReceived " << seq << endl;

                if(seq != m_lastRcvd+1) {
                    // Mark missing packets
                    for(int i = m_lastRcvd+1; i < seq; ++i ) {
                        m_recvTimes[i] = 0;
                    }
                }
                m_lastRcvd = seq;
            }
        }
    } // Serial buffer is over

    if(seq >= PINGCOUNT-1)
        emit(receivedAll());
}

void MavPing2::printStats()
{
    double avgTripTime = 0.0;
    int packetLost = 0;

    struct timespec ts;
    clock_getres(CLOCK_MONOTONIC, &ts);
    float resolution =  (ts.tv_sec * (uint64_t)1000000000 + ts.tv_nsec);

    qout << "------------- Stats ---------------\n";

    for(int i = 0; i < PINGCOUNT; ++i ){
        qout << "[" << i << "]";
        if(m_recvTimes.at(i) != 0){
            qout << "Trip Time: "
                 << (m_recvTimes.at(i) - m_sendTimes.at(i))*1E-6
                 << " ms\n";
            avgTripTime += (m_recvTimes.at(i) - m_sendTimes.at(i))*1E-6; //ms
        }
        else {
            qout << "\tLost!\n";
            packetLost++;
        }

    }

    avgTripTime /= PINGCOUNT - packetLost;

    qout << "\nAverage Trip Time: " << avgTripTime << " ms\n"
         << "Packet Lost: " << packetLost << "\n"
         << "(Timer resolution = " <<  resolution << "ns)\n" << endl;

    emit finished();
}
