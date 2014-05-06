#ifndef SPEAKER_H
#define SPEAKER_H

#include <QObject>

class Speaker : public QObject
{
    Q_OBJECT
public:
    explicit Speaker(QObject *parent = 0);

signals:

public slots:

};

#endif // SPEAKER_H
