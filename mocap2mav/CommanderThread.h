#ifndef COMMANDERTHREAD_H
#define COMMANDERTHREAD_H

#include <QThread>

class CommanderThread : public QThread
{
    Q_OBJECT

    void run();

public:
    explicit CommanderThread(QObject *parent = 0);
    bool m_stop;

signals:

public slots:

    void startMe();
    void stopMe();
    void checkCommands();
};

#endif // COMMANDERTHREAD_H


