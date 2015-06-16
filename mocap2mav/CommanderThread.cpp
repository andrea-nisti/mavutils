#include "CommanderThread.h"
#include "global.h"
#include <vector>
#include "QDebug"

std::vector <command> commVect;
CommanderThread::CommanderThread(QObject *parent) :
    QThread(parent)
{
    commVect.clear();
    m_stop = false;

}


QTime ra;
int rt = 10; //Hz



void CommanderThread::run(){

    ra.start();
    while (!m_stop) {


        sleep(1/rt - ra.elapsed());
        ra.restart();

    }


}


void CommanderThread::startMe(){

    qDebug() << "starting thread";
    m_stop = false;
    this->start();

}

void CommanderThread::stopMe(){

    qDebug() << "killing thread";
    m_stop = true;
    this->quit();

}

void CommanderThread::checkCommands(){

    std::vector <command> highPriorityVect;
    std::vector <command> lowPriorityVect;

    for (int i = 0; i < commVect.size();i++){

        if(commVect[i].priority == 1){


            highPriorityVect.push_back(commVect[i]);
        }

        else if(commVect[i].priority == 0){
            lowPriorityVect.push_back(commVect[i]);
        }



    }


    if(highPriorityVect.size() > 0){

        for(int j = 0;j < highPriorityVect.size(); j++){


            g::setPoint.setX(g::setPoint.x() + highPriorityVect[j].p.x);


        }
        commVect.clear();


    }
}
