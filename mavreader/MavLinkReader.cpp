#include "MavLinkReader.h"
#include "common/MavUtils.h"
#include <QtSerialPort/QSerialPort>
#include <QTextStream>

#include <mavlink/common/mavlink.h>

static QTextStream qout(stdout);

MavLinkReader::MavLinkReader(QObject *parent) :
    QObject(parent)
{
}

void MavLinkReader::run(){

    uint8_t c;
    quint64 startTime = getTimeNanoSecond();

    QSerialPort serial(this);
    serial.setPortName("/dev/ttyUSB0");
    serial.setBaudRate(QSerialPort::Baud57600);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    mavlink_message_t msg;
    mavlink_status_t status;

    serial.open(QSerialPort::ReadOnly);
    qout << "Started";

    forever
    {
        if(!serial.waitForReadyRead(15000)) {
            qout << "Serial not ready";
            break;
        }

        while(serial.bytesAvailable() > 0){


            serial.read((char*)&c, 1);

            // Try to get a new message
            if(mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
                // Handle message
                qout << "MsgID " << msg.msgid << "\tSys ID" << msg.sysid << endl;
                qout.flush();

                switch(msg.msgid)
                {
                case MAVLINK_MSG_ID_HEARTBEAT:

                    mavlink_heartbeat_t heartbitMsg;
                    mavlink_msg_heartbeat_decode(&msg, &heartbitMsg);

                    qout << "HEARTBEAT from UAV " << msg.sysid
                             << "UAV Info:\n - Mode:" << heartbitMsg.base_mode
                             << "\n- Mavlink v. " << heartbitMsg.mavlink_version;
                    break;

                /*case MAVLINK_MSG_ID_SYS_STATUS:
                    mavlink_sys_status_t sysStatus;
                    mavlink_msg_sys_status_decode(&msg, &sysStatus);
                    qDebug() << "SYS STATUS from UAV " << msg.sysid;
                    qDebug() << "- Battery Voltage = " << sysStatus.voltage_battery;
                    break;*/
                case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE:
                    mavlink_rc_channels_override_t rcOverrideMsg;
                    mavlink_msg_rc_channels_override_decode(&msg, &rcOverrideMsg);

                    qout << "[" << (uint)((getTimeNanoSecond() - startTime)*1E-6) << "]\t"
                         << rcOverrideMsg.chan1_raw << "\t"
                         << rcOverrideMsg.chan2_raw << "\t"
                         << rcOverrideMsg.chan3_raw << endl;
                    break;
                default:
                    //Do nothing
                    break;
                }
            }

            // And get the next one
        }

        // Update global packet drops counter
        //packet_drops += status.packet_rx_drop_count;
    }

    serial.close();

    emit finished();
}
