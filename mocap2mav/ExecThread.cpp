#include "ExecThread.h"
#include "global.h"
#include <vector>
#include "utils.h"

namespace executioner{
    namespace land{
       bool land_sig;
       bool landed;
    }
    namespace take_off{
       bool take_off_sig;
       bool take_off_done;
    }
    namespace move{
       bool move_sig;
       bool move_done;
    }

}

std::vector<node> nodeList;
int actualNode = 0;


bool checkActions(char a);
void signalsReset();
bool can_run = false;
ExecThread::ExecThread(QObject *parent) :
    QThread(parent)
{
    //fill nodelist

    node node1;
    node1.a.type = 't';
    node1.a.params[0] = -2; //height
    nodeList.push_back(node1);

    node node2;
    node2.a.type = 'm';
    node2.p.x = 1;
    node2.p.y = 1;
    node2.p.z =-1;
    nodeList.push_back(node2);

    node node3;
    node3.a.type = 'm';
    node3.p.x = -1;
    node3.p.y = -1;
    node3.p.z =-3;
    nodeList.push_back(node3);

    node node4;
    node4.a.type = 'l';
    node4.a.params[0] = 0.3; //height
    nodeList.push_back(node4);



    //Initialization for take off
    if(nodeList.size()>0){
        nodeList[0].p.x = g::state.x();
        nodeList[0].p.y = g::state.y();
        can_run = true;
    }
    else{
        qDebug() << "WARNING, empty list";
        can_run = false;
    }

}

void ExecThread::run(){

    /*_____ACTIONS: _____________/*
     *
     * m : move (staring point)
     * l : land (landing altitude)
     * t : take off (altitude)
     * f : follow trajectory (traj type, staring point)
     * h : hover (seconds)
     * s : stare (staring point)
     *
     */


    QTime rate;
    int r = 40; //Hz
    rate.start();

    qDebug() << "executioner from: " << QThread::currentThreadId();


    bool message = true;
    while(actualNode < nodeList.size()){


        char act = nodeList[actualNode].a.type;

        // Spread the word
        if(message) {
                qDebug() << "Performing node: " << actualNode << "with action: " << act;
                message = false;
                qDebug() << "set point: " << g::setPoint.x() << " " << g::setPoint.y() << " " << g::setPoint.z() <<"yaw: "<<g::setPoint.yaw();
        }

        //Toggle action

        switch (act) {

        case 'm':
            executioner::move::move_sig = true;
            break;
        case 'l':

            nodeList[actualNode].p.x = g::state.x();
            nodeList[actualNode].p.y = g::state.y();
            nodeList[actualNode].p.z = g::state.z();

            executioner::land::land_sig = true;
            break;
        case 't':

            nodeList[actualNode].p.x = g::state.x();
            nodeList[actualNode].p.y = g::state.y();
            nodeList[actualNode].p.z = g::state.z();

            executioner::take_off::take_off_sig = true;
            break;
        default:
            break;
        }


        if(checkActions(act)) {
            actualNode++;
            signalsReset();
            message = true;
        }

        msleep(1000/r - (float)rate.elapsed());
        rate.restart();
    }

    qDebug() << "no more nodes!";
    this->exit();


}


bool checkActions(char a){

    switch (a) {

    case 'm':

        if(fabs(g::state.x() - nodeList[actualNode].p.x) < 0.15 &&
           fabs(g::state.z() - nodeList[actualNode].p.x) < 0.15 &&
           fabs(g::state.z() - nodeList[actualNode].p.x) < 0.15){

           executioner::move::move_done = true;

        }
        else{
            executioner::move::move_done = false;
        }


        return executioner::move::move_done;
        break;
    case 'l':

        return executioner::land::landed; // XXX Choose a better stopping condition based on vz;

        break;
    case 't':

        if(fabs(g::state.z() - nodeList[actualNode].a.params[0]) < 0.1 ){
            executioner::take_off::take_off_done = true;
        }
        else{
            executioner::take_off::take_off_done = false;
        }

        return executioner::take_off::take_off_done;
        break;




    default:
        break;
    }




}

void signalsReset(){

    executioner::land::landed = false;
    executioner::land::land_sig = false;
    executioner::take_off::take_off_sig = false;
    executioner::take_off::take_off_done = false;
    executioner::move::move_done = false;
    executioner::move::move_sig = false;

}







