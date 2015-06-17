#ifndef MANUALCONTROL_H
#define MANUALCONTROL_H

#include <QObject>
#include <QThread>

#include "Window.h"
#include "PositionDispatcher.h"
#include "ManualThread.h"

class ManualControl : public QObject
{
    Q_OBJECT
public:
    explicit ManualControl(QObject *parent = 0);
    ManualThread thread;


signals:

   void publish();

public slots:

   void setHome();
   void incrX();
   void incrY();
   void decrX();
   void decrY();
   void incrZ();
   void decrZ();
   void incrYaw();
   void decrYaw();

private:

   Window *win;
   QMutex mutex;

};

#endif
