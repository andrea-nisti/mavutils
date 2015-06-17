#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <QObject>
#include"ManualControl.h"
#include "CommanderThread.h"
#include "Commander.h"
#include "AutoControl.h"

class MainControl : public QObject
{
    Q_OBJECT
public:
    explicit MainControl(QObject *parent = 0);

    ManualControl manual;
    AutoControl automatic;
    Commander commander;




signals:

public slots:

};

#endif // MAINCONTROL_H
