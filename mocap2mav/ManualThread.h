#ifndef MANUALTHREAD_H
#define MANUALTHREAD_H

#include <QThread>

class ManualThread : public QThread
{
    Q_OBJECT

    void run();

public:
    explicit ManualThread(QObject *parent = 0);
    bool m_stop;
signals:

    void begin();

public slots:

    void startMe();
    void stopMe();

};

#endif // MANUALTHREAD_H
