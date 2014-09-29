#include "recordaudio.h"

RecordAudio::RecordAudio(QString path, QAudioFormat format, QObject *parent) :
    QObject(parent)
{
    this->format = format;
    this->path = path;
    outputfile = new QFile(path + "/tmp.wav");
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
