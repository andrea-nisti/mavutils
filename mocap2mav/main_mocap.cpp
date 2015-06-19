#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include "PositionDispatcher.h"
#include "NatNetReceiver.h"
#include "ManualControl.h"
#include "ManualThread.h"
#include "global.h"
#include "CommanderThread.h"
#include <QThread>
#include "MainControl.h"
#include "Commander.h"
#include "utils.h"
#include "Automatic.h"
#include "Executioner.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    //NatNetReceiver mocap;
    //PositionDispatcher positionDispatcher;
   // MainControl controller;
    Commander commander;
    ManualControl manual;
    Automatic autom;
    Executioner ex;


    //Connect manual control to commander
    QObject::connect(&manual,SIGNAL(publish()),&commander,SLOT(checkCommands()));
    QObject::connect(&autom,SIGNAL(publish()),&commander,SLOT(checkCommands()));



    qDebug() << "main from: " << QThread::currentThreadId();



//    QObject::connect(&mocap, SIGNAL(dataUpdate()), &positionDispatcher, SLOT(sendPosition()));
//    QObject::connect(&positionDispatcher, SIGNAL(finished()), &a, SLOT(quit()));

    return a.exec();
}

