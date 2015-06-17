#include "MainControl.h"
#include"Commander.h"

MainControl::MainControl(QObject *parent) :
    QObject(parent)
{

    //Connect manual control to commander
    QObject::connect(&manual,SIGNAL(publish()),&commander,SLOT(checkCommands()));
    QObject::connect(&automatic,SIGNAL(publish()),&commander,SLOT(checkCommands()));
    QObject::connect(&automaticThread,SIGNAL(publish()),&automatic,SLOT(publishWrap()));
    automaticThread.start();


}
