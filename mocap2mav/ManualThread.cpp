#include "ManualThread.h"
#include "unistd.h"
#include <QDebug>
#include "MavState.h"
#include <QTime>
#include <cmath>
#include "global.h"
#include "utils.h"

#define PI 3.141592653589

//Apply translation to a
void translate(double a[2], double b[2], double res[2]){

    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];

}


ManualThread::ManualThread(QObject *parent) :
    QThread(parent)
{
    m_stop = false;
}



float omega = 0.25;
QTime t;
QTime rate;
int r = 10; //Hz
int milli;
float a = 1.1;
float b = 3;
double yaw_sp,x_sp_glob,y_sp_glob;
double c[2] = {0.4,0};
double c0[2] = {  a/2 , 0};
double c1[2] = { -a/2 , 0};
double c0_glob[2];
double c1_glob[2];
double sp_local[2];
double sp_global[2];
int c_actual = 1;
bool check = true;
int count = 0;


void ManualThread::run(){

    g::setPoint.setPosition(c[0], c[1], -1);
    sleep(3);

    t.start();
    rate.start();


    while (!m_stop && t.elapsed() < 80000) {

        rate.restart();
        double yaw_comm;
        double yaw_check;

        if(!check){
            if(++count == r * 4){
                count = 0;
                check = true;
            }
        }

        // Lemniscate trajectory
        milli = t.elapsed();

        float sin_ = sin(PI/2 + omega * milli / 1000);
        float cos_ = cos(PI/2 + omega * milli / 1000);
        float factor = cos_ / (pow(sin_, 2) + 1);

        sp_local[0] = (a * factor);
        sp_local[1] = (b * sin_ * factor);

        translate(sp_local,c,sp_global);
        translate(c0,c,c0_glob);
        translate(c1,c,c1_glob);

        if (c_actual == 0){

            yaw_sp = atan2( c0_glob[1] - g::state.y(),c0_glob[0] - g::state.x());
            yaw_check = atan2( c0_glob[1] - sp_global[1],c0_glob[0] - sp_global[0]);

        }
        else{

            yaw_sp = atan2( c1_glob[1] - g::state.y(),c1_glob[0] - g::state.x());
            yaw_check = atan2( c1_glob[1] - sp_global[1],c1_glob[0] - sp_global[0]);
        }

        calculateYawIntem(yaw_sp,g::state.getYaw(),yaw_comm);

        g::setPoint.setX(sp_global[0]);
        g::setPoint.setY(sp_global[1]);
        g::setPoint.setYaw(yaw_comm);

        if(yaw_sp < -PI/2 + PI/18 && yaw_sp > -PI/2 - PI/18 && yaw_sp < 0){
            // if x > 0 choose center to look, use yaw_sp or yaw_check?
            if (sp_local[0] > 0) c_actual = 1;
            else if (sp_local[0] < 0) c_actual = 0;

            check = false;
            qDebug() << "looking center: "<< c_actual;

        }
        qDebug() << "set point: " << sp_global[0] << " " << g::setPoint.y() << " " << g::setPoint.z() <<" "<< c0_glob[0] <<" "<< c1_glob[0];
        msleep(1000/r - (float)rate.elapsed());

    }

}

void ManualThread::startMe(){

    qDebug() << "starting thread";
    m_stop = false;
    this->start();
    begin();

}

void ManualThread::stopMe(){

    qDebug() << "killing thread";
    m_stop = true;
    this->quit();

}


/*
        rate.restart();
        double yaw_comm;

        // Lemniscate trajectory
        milli = t.elapsed();

        float sin_ = sin(PI/2 + omega * milli / 1000);
        float cos_ = cos(PI/2 + omega * milli / 1000);
        float factor = cos_ / (pow(sin_, 2) + 1);

        x_sp = (a * factor);
        y_sp = (b * sin_ * factor);

        x_sp_glob = c[0] + x_sp;
        y_sp_glob = c[1] + y_sp;

        yaw_sp = atan2( 0 - g::state.y(),-2 - g::state.x());
        calculateYawIntem(yaw_sp,g::state.getYaw(),yaw_comm);

        g::setPoint.setX(x_sp_glob);
        g::setPoint.setY(y_sp_glob);
        g::setPoint.setYaw(yaw_comm);

        qDebug() << "set point: " << g::setPoint.x() << " " << g::setPoint.y() << " t: " << sin_;
*/


