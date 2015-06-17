#include "AutoControl.h"
#include "MavState.h"
#include "global.h"
#include "unistd.h"
AutoControl::AutoControl(QObject *parent) :
    QObject(parent)
{
}
int AutoControl::land(float speed,float dt){

    //landing procedure

    bool done = false;
    MavState comm= g::setPoint;
    float z = comm.z();
    if(z > -0.30){

        z = 0.7;
        done = true;


    }
    else{
        z += speed * dt;
    }

        autoCommand.push_back(comm);
        publish();
        if(done) return 0;

}

void AutoControl::takeOff(){

}
