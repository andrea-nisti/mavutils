#ifndef COMMANDER_H
#define COMMANDER_H

#include <QObject>
#include "CommanderThread.h"

class Commander : public QObject
{

    Q_OBJECT

public:
    explicit Commander(QObject *parent = 0);
    CommanderThread commanderThr;
signals:


public slots:

    void checkCommands();


};

#endif // COMMANDER_H
