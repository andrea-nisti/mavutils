#include "CommanderThread.h"
#include "global.h"
#include <vector>
#include "QDebug"
#include "Commander.h"

std::vector <MavState> manualCommand;
std::vector <MavState> autoCommand;


CommanderThread::CommanderThread(QObject *parent) :
    QThread(parent)
{



}

void CommanderThread::run(){


    qDebug() << "starting COMMANDER thread";

    exec();

}


void CommanderThread::startMe(){

    qDebug() << "starting thread";

    this->start();

}

void CommanderThread::stopMe(){

    qDebug() << "killing thread";

    this->quit();

}



