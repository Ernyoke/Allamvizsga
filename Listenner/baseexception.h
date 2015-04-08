#ifndef BASEEXCEPTION_H
#define BASEEXCEPTION_H

#include <QObject>
#include <QDebug>

class BaseException : public QObject
{
    Q_OBJECT
public:
    explicit BaseException(QObject *parent = 0);
    virtual ~BaseException();

    QString message();
    void setMessage(QString);

private:
    QString msg;

signals:


public slots:
};

#endif // BASEEXCEPTION_H
