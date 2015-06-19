#ifndef AUTOMATIC_H
#define AUTOMATIC_H

#include <QObject>
#include "AutoThread.h"

class Automatic : public QObject
{
    Q_OBJECT
    AutoThread thread;
public:
    explicit Automatic(QObject *parent = 0);

signals:

    void publish();

public slots:

    void publishWrapper();

};

#endif // AUTOMATIC_H
