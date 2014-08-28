#include "recordaudio.h"

RecordAudio::RecordAudio(QString path, QAudioFormat format, GUI* gui, QObject *parent) :
    QObject(parent)
{
    this->format = format;
    this->gui = gui;
    this->path = path;
    outputfile = new QFile(path + "/tmp.wav");
    currentState = STOPPED;
}

RecordAudio::~RecordAudio() {
    delete outputfile;
    qDebug() << "RecordAudio destruct!";
}

RecordAudio::STATE RecordAudio::getState() {
    return currentState;
}
