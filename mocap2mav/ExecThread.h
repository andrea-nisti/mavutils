#ifndef EXECTHREAD_H
#define EXECTHREAD_H

#include <QThread>
#include <vector>
#include "utils.h"
#include "global.h"


/*_____ACTIONS: _____________/*
 *
 * m : move
 * l : land
 * t : take off
 * f : follow trajectory
 * s : stare at point
 *
 */

class ExecThread : public QThread
{
    Q_OBJECT


    node actualNode;
public:
    explicit ExecThread(QObject *parent = 0);

signals:

public slots:

};

#endif // EXECTHREAD_H
