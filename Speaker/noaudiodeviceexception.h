#ifndef NOAUDIODEVICEEXCEPTION_H
#define NOAUDIODEVICEEXCEPTION_H

#include "baseexception.h"

class NoAudioDeviceException : public BaseException
{
    Q_OBJECT
public:
    explicit NoAudioDeviceException(BaseException *parent = 0);
    ~NoAudioDeviceException();

signals:

public slots:
};

#endif // NOAUDIODEVICEEXCEPTION_H
