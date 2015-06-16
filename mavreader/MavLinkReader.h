#ifndef MAVLINKREADER_H
#define MAVLINKREADER_H

#include <QObject>

class MavLinkReader : public QObject
{
    Q_OBJECT
public:
    explicit MavLinkReader(QObject *parent = 0);

signals:
    void finished();

public slots:
    void run();
};

#endif // MAVLINKREADER_H
