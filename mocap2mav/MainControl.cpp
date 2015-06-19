#include "MainControl.h"
#include"Commander.h"
#include "AutoThread.h"

MainControl::MainControl(QObject *parent) :
    QObject(parent)
{



    ExecThread executioner;
    AutoThread automaticThread;
    //Connect manual control to commander
    QObject::connect(&manual,SIGNAL(publish()),&commander,SLOT(checkCommands()));
    //QObject::connect(&automatic,SIGNAL(publish()),&commander,SLOT(checkCommands()));
    QObject::connect(&automaticThread,SIGNAL(publish()),&commander,SLOT(publishWrap()));

    executioner.start();
    automaticThread.start();


}
