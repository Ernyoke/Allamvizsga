#ifndef INVALIDIDEXCEPTION_H
#define INVALIDIDEXCEPTION_H

#include <QObject>
#include "baseexception.h"

class InvalidIdException : public BaseException
{
    Q_OBJECT
public:
    explicit InvalidIdException(QObject *parent = 0);
    ~InvalidIdException();

signals:

public slots:
};

#endif // INVALIDIDEXCEPTION_H
