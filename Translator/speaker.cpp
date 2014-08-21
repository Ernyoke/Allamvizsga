#include "speaker.h"

Speaker::Speaker(GUI *gui, QObject *parent) :
    QThread(parent)
{
    this->socket = new QUdpSocket(this);
    this->gui = gui;

    settings = gui->getSettings();

    audioInput = NULL;
    QDateTime now = QDateTime::currentDateTime();
    timestamp = now.currentDateTime().toMSecsSinceEpoch();
    isRecording = false;
}

Speaker::~Speaker() {
    stopRecording();
    qDebug() << "Speaker deleted!";
}

void Speaker::run() {
    connect(gui, SIGNAL(startButtonPressed()), this, SLOT(startRecording()));
    connect(gui, SIGNAL(stopButtonPressed()), this, SLOT(stopRecording()));
}

void Speaker::startRecording() {
    if(!isRecording) {
        broadcasting_port = gui->getBroadcastingPort();
        format = settings->getSpeakerAudioFormat();

        QAudioDeviceInfo info = settings->getInputDevice();
        if (!info.isFormatSupported(format)) {
            qWarning()<<"raw audio format not supported by backend, cannot play audio.";
            return;
        }

       audioInput = new QAudioInput(info, format, this);
       qDebug() << audioInput->bufferSize();
       intermediateDevice  = audioInput->start();
       connect(intermediateDevice, SIGNAL(readyRead()), this, SLOT(transferData()));

       buffLen = audioInput->periodSize();
       isRecording = true;
    }

}

void Speaker::transferData(){
    if(audioInput->bytesReady() >= buffLen) {
        QByteArray chunk;
        QByteArray sendBuffer;
        chunk = intermediateDevice->read(buffLen);
        timestamp++;
        for(int i = sizeof(qint64); i > 0; --i) {
            char x = (timestamp >> ((i - 1) * 8));
            sendBuffer.prepend(x);
        }

        sendBuffer.append(chunk);
        /*for(int i = 0; i < len; ++i) {
            //short pcm_value = chunk[i + 1];
            //pcm_value = (pcm_value << 8) | chunk[i];
            //sendBuffer.append(G711::Snack_Lin2Alaw(pcm_value));
            //++i;
            sendBuffer.append(chunk[i]);
        }*/

        gui->setDataSent(sendBuffer.size());

        socket->writeDatagram(sendBuffer, QHostAddress::LocalHost, broadcasting_port);
    }
}

void Speaker::stopRecording()
{
    if(isRecording) {
        audioInput->stop();
        delete audioInput;
        isRecording = false;
    }
}
