#include "recordaudio.h"

RecordAudio::RecordAudio(QString path, QAudioFormat format, QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<STATE>("RecordAudio::STATE");

    this->format = format;
    this->path = path;
    currentState = STOPPED;
    this->finalized = false;
}

RecordAudio::~RecordAudio() {
    delete outputfile;
    qDebug() << "RecordAudio destruct!";
}

RecordAudio::STATE RecordAudio::getState() {
    return currentState;
}
