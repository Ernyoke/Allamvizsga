#include "managevoice.h"

ManageVoice::ManageVoice(QUdpSocket *socket, GUI *gui, QObject *parent) :
    QObject(parent)
{
    this->socket = socket;
    this->gui = gui;

    settings = gui->getSettings();

    connect(gui, SIGNAL(startButtonPressed()), this, SLOT(startRecording()));
    connect(gui, SIGNAL(stopButtonPressed()), this, SLOT(stopRecording()));

    audioInput = NULL;
    QDateTime now = QDateTime::currentDateTime();
    timestamp = now.currentDateTime().toMSecsSinceEpoch();
}

void ManageVoice::startRecording() {
    broadcasting_port = gui->getBroadcastingPort();
    format = settings->getSpeakerAudioFormat();

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(*format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        return;
    }

   audioInput = new QAudioInput(*format, this);
   audioInput->setBufferSize(1000);
   intermediateDevice  = audioInput->start();
   connect(intermediateDevice, SIGNAL(readyRead()), this, SLOT(transferData()));

}

void ManageVoice::transferData(){
    int len = 640;
    qDebug() << audioInput->bytesReady();
    if(audioInput->bytesReady() >= len) {
        QByteArray chunk;
        QByteArray sendBuffer;
        QDateTime now = QDateTime::currentDateTime();
        chunk = intermediateDevice->read(len);
        //timestamp = now.currentDateTime().toMSecsSinceEpoch();
            timestamp++;
        prevstamp = timestamp;
        //qDebug() << timestamp;
        QString stm = "";
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

void ManageVoice::stopRecording()
{
  audioInput->stop();
  //outputFile.close();
  //delete audioInput;
  //intermediateDevice->close();
}

ManageVoice::~ManageVoice() {
    if(audioInput != NULL) {
        delete audioInput;
        delete format;
    }
}

