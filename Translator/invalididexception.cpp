#include "invalididexception.h"

InvalidIdException::InvalidIdException(QObject *parent) : BaseException(parent)
{
    qDebug() << "InvalidIdException created!";
}

InvalidIdException::~InvalidIdException()
{
    qDebug() << "InvalidIdException destroyed!";
}

