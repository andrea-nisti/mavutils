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



ManualControl::ManualControl(QObject *parent) :
    QObject(parent)
{

    this->setHome();
    qDebug() << "homing" ;

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

    win->show();

}

void ManualControl::setHome(){

    g::setPoint.setPosition(0, 0 , -1);
    g::setPoint.setYaw(0);
    qDebug() << "HOMED: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();

}

void ManualControl::decrX(){

    command comm;
    comm.priority = 1;
    comm.p.x = - 0.1;
    comm.p.y = 0;

    comm.p.yaw = 0;
    //g::setPoint.setX(g::setPoint.x() - 0.1);
    commVect.push_back(comm);
    commanderT.checkCommands();

     qDebug() << commVect.size();
    qDebug() << "SET POINT: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
}

void ManualControl::decrY(){

    g::setPoint.setY(g::setPoint.y() - 0.1);
    qDebug() << "SET POINT: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
}

void ManualControl::incrX(){

    g::setPoint.setX(g::setPoint.x() + 0.1);
    qDebug() << "SET POINT: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
}

void ManualControl::incrY(){

    g::setPoint.setY(g::setPoint.y() + 0.1);
    qDebug() << "SET POINT: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
}

void ManualControl::incrZ(){

    g::setPoint.setZ(g::setPoint.z() + 0.1);
    qDebug() << "SET POINT: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
}

void ManualControl::decrZ(){

    g::setPoint.setZ(g::setPoint.z() - 0.1);
    qDebug() << "SET POINT: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z();
}

void ManualControl::incrYaw(){

    float yaw;

    yaw = g::setPoint.yaw();
    yaw += PI / 10 ;
    if (yaw > PI){
        yaw = -yaw + 2*PI/10;
    }

    g::setPoint.setYaw(yaw);
    qDebug() << "yaw: " << g::setPoint.yaw();
}

void ManualControl::decrYaw(){

    float yaw;

    yaw = g::setPoint.yaw();
    yaw -= PI / 10 ;
    if (yaw < -PI){
        yaw = -yaw - 2*PI/10;
    }

    g::setPoint.setYaw(yaw);
    qDebug() << "yaw: " << g::setPoint.yaw();
}

