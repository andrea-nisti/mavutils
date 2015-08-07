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

    if (manualCommand.size()>0){




        for(int i = 0;i < manualCommand.size();i++){
            g::setPoint = manualCommand[i];
        }

    }
    else if(autoCommand.size()>0){

        for(int j = 0;j < autoCommand.size();j++){
            //publish
            g::setPoint = autoCommand[j];

        }

    }


    manualCommand.clear();
    autoCommand.clear();

}
