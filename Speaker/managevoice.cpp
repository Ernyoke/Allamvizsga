#include "managevoice.h"

ManageVoice::ManageVoice(QUdpSocket *socket, GUI *gui, QObject *parent) :
    QObject(parent)
{
    this->socket = socket;
    this->gui = gui;

    settings = gui->getSettings();

    connect(gui, SIGNAL(startButtonPressed()), this, SLOT(startRecording()));
    connect(gui, SIGNAL(stopButtonPressed()), this, SLOT(stopRecording()));
}

void ManageVoice::startRecording() {
    broadcasting_port = gui->getBroadcastingPort();
    QAudioFormat *format;
    format = settings->getSpeakerAudioFormat();

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(*format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        return;
    }

   audioInput = new QAudioInput(*format, this);
   intermediateDevice  = audioInput->start();
   audioInput->setNotifyInterval(5);
   connect(audioInput, SIGNAL(notify()), this, SLOT(transferData()));

}

void ManageVoice::transferData(){
    int len = 640;
    if(audioInput->bytesReady() >= 0) {
        QByteArray chunk;
        QByteArray sendBuffer;
        qint64 timestamp;
        QDateTime now = QDateTime::currentDateTime();
        //qDebug() << audioInput->bytesReady();
        chunk = intermediateDevice->readAll();
        timestamp = now.currentDateTime().toMSecsSinceEpoch();
        QString stm = "";
        for(int i = sizeof(qint64); i > 0; --i) {
            char x = (timestamp >> ((i - 1) * 8));
            sendBuffer.prepend(x);

           /* for(int j = 0; j < 8; ++j) {
                if(x&1 == 1) {
                    stm += "1";
                }
                    else {
                        stm += "0";
                    }
                x = x >> 1;
            }*/
            //stm += QString::number(x) + " ";
        }

        sendBuffer.append(chunk);
        len = chunk.size();
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

