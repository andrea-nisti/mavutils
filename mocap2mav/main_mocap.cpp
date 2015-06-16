#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include "PositionDispatcher.h"
#include "NatNetReceiver.h"
#include "ManualControl.h"
#include "ManualThread.h"
#include "global.h"
#include "CommanderThread.h"


CommanderThread commanderT;
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    //NatNetReceiver mocap;
    //PositionDispatcher positionDispatcher;
    ManualControl *setPointer = new ManualControl();
    AutoControl *autoC = new AutoControl();

    commanderT.startMe();

//    QObject::connect(&mocap, SIGNAL(dataUpdate()), &positionDispatcher, SLOT(sendPosition()));
//    QObject::connect(&positionDispatcher, SIGNAL(finished()), &a, SLOT(quit()));

    return a.exec();
}

