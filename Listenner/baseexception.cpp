#include "baseexception.h"

BaseException::BaseException(QObject *parent) : QObject(parent)
{

}

BaseException::~BaseException()
{

}

QString BaseException::message() {
    return this->msg;
}

void BaseException::setMessage(QString msg) {
    this->msg = msg;
}

