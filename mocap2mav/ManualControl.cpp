#include "ManualControl.h"
#include <QObject>
#include <QPushButton>
#include <Window.h>
#include <QDebug>
#include "MavState.h"
#include <QTime>
#include <QThread>
#include <unistd.h>
#include "global.h"
#define PI 3.141592653589

float yawT = 0;

ManualControl::ManualControl(QObject *parent) :
    QObject(parent)
{

    this->setHome();
    qDebug() << "homing" ;
    qDebug() << "manual from: " << QThread::currentThreadId();
    //Start Calculate()
    win = new Window(400,400);
    win->button->setText("HOME");
    connect(win->button, SIGNAL(clicked()), this, SLOT(setHome()));

    //START button XXX add STOP button
    QPushButton *start = new QPushButton("START",win);
    start->setGeometry(30,340,80,30);
    connect(start, SIGNAL(clicked()), &thread, SLOT(startMe()));

    //STOP button
    QPushButton *stop = new QPushButton("STOP",win);
    stop->setGeometry(290,340,80,30);
    connect(stop, SIGNAL(clicked()), &thread, SLOT(stopMe()));

    // INCREMENTAL MODE (Orientation linked to calibration and lab workstation)
    QPoint c =  win->geometry().center();

    //front
    QPushButton *front = new QPushButton("front",win);
    front->setGeometry(c.x()-20,c.y() - 100,40,40);
    connect(front, SIGNAL(clicked()), this, SLOT(decrX()));

    //back
    QPushButton *back = new QPushButton("back",win);
    back->setGeometry(c.x()-20,c.y() + 60,40,40);
    connect(back, SIGNAL(clicked()), this, SLOT(incrX()));

    //left
    QPushButton *left = new QPushButton("left",win);
    left->setGeometry(c.x()-100,c.y() -20 ,40,40);
    connect(left, SIGNAL(clicked()), this, SLOT(incrY()));

    //right
    QPushButton *right = new QPushButton("right",win);
    right->setGeometry(c.x()+60,c.y() -20 ,40,40);
    connect(right, SIGNAL(clicked()),this, SLOT(decrY()));

    //up
    QPushButton *up = new QPushButton("up",win);
    up->setGeometry(c.x()+120,c.y() - 100,40,40);
    connect(up, SIGNAL(clicked()),this, SLOT(decrZ()));

    //down
    QPushButton *down = new QPushButton("down",win);
    down->setGeometry(c.x()+120,c.y() + 60,40,40);
    connect(down, SIGNAL(clicked()),this, SLOT(incrZ()));

    //Yaw CW
    QPushButton *cw = new QPushButton("->",win);
    cw->setGeometry(c.x()-160,c.y() - 100,40,40);
    connect(cw, SIGNAL(clicked()), this, SLOT(incrYaw()));

    // Yaw CCW
    QPushButton *ccw = new QPushButton("<-",win);
    ccw->setGeometry(c.x()-160,c.y() + 60,40,40);
    connect(ccw, SIGNAL(clicked()),this, SLOT(decrYaw()));


    //PUBLISH SLOT

    win->show();

}

void ManualControl::setHome(){

    g::setPoint.setPosition(0, 0 , -1);
    g::setPoint.setYaw(0);
    qDebug() << "HOMED: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();

}

void ManualControl::decrX(){

    MavState comm = g::setPoint;
    comm.setX(comm.x() - 0.1);
    manualCommand.push_back(comm);
    publish();



}

void ManualControl::decrY(){

    MavState comm = g::setPoint;
    comm.setY(comm.y() - 0.1);
    manualCommand.push_back(comm);
    publish();

}

void ManualControl::incrX(){
    MavState comm = g::setPoint;
    comm.setX(comm.x() + 0.1);
    manualCommand.push_back(comm);
    publish();

}

void ManualControl::incrY(){
    MavState comm = g::setPoint;
    comm.setY(comm.y() + 0.1);
    manualCommand.push_back(comm);
    publish();

}

void ManualControl::incrZ(){
    MavState comm = g::setPoint;
    comm.setZ(comm.z() + 0.1);
    manualCommand.push_back(comm);
    publish();

}

void ManualControl::decrZ(){
    MavState comm = g::setPoint;
    comm.setZ(comm.z() - 0.1);
    manualCommand.push_back(comm);
    publish();

}

void ManualControl::incrYaw(){

    MavState comm = g::setPoint;



    float yawT = comm.yaw();


    yawT += PI / 10 ;
    if (yawT > PI){
        yawT = -yawT + 2*PI/10;
    }

    comm.setYaw(yawT);
    manualCommand.push_back(comm);
    publish();

}

void ManualControl::decrYaw(){
    MavState comm = g::setPoint;
    float yawT=comm.yaw();


    yawT -= PI / 10 ;
    if (yawT < -PI){
        yawT = -yawT - 2*PI/10;
    }

    comm.setYaw(yawT);
    manualCommand.push_back(comm);
    publish();

}

