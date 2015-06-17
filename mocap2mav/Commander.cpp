#include "Commander.h"
#include <QDebug>
#include "global.h"

Commander::Commander(QObject *parent) :
    QObject(parent)
{
    this->moveToThread(&commanderThr);
    commanderThr.start();
}

void Commander::checkCommands(){

    //priority check
    qDebug() << "checking from: " << QThread::currentThreadId();
    if (manualCommand.size()>0){



        g::setPoint = manualCommand[0];
        for(int i = 0;i < manualCommand.size();i++){

        }

    }
    else if(autoCommand.size()>0){

        for(int j = 0;j < autoCommand.size();j++){
            //publish
        }

    }


    manualCommand.clear();
    autoCommand.clear();

}
