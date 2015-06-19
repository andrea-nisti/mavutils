#ifndef EXECUTIONER_H
#define EXECUTIONER_H

#include <QObject>
#include "ExecThread.h"

class Executioner : public QObject
{
    Q_OBJECT

public:
    explicit Executioner(QObject *parent = 0);
    ExecThread thread;
signals:

public slots:


};

#endif // EXECUTIONER_H
