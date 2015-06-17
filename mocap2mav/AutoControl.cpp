#include "AutoControl.h"
#include "MavState.h"
#include "global.h"
#include "unistd.h"
AutoControl::AutoControl(QObject *parent) :
    QObject(parent)
{

}


void AutoControl::publishWrap(){
    publish();
}
