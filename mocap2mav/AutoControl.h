#ifndef AUTOCONTROL_H
#define AUTOCONTROL_H

#include <QObject>


class AutoControl : public QObject
{
    Q_OBJECT


public:
    explicit AutoControl(QObject *parent = 0);

signals:
    void publish();

public slots:
    int land(float speed, float dt);
    void takeOff();

};

#endif // AUTOCONTROL_H
