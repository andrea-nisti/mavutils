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

    void publishWrap();

};

#endif // AUTOCONTROL_H
