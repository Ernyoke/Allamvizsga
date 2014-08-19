#include "managevoice.h"

ManageVoice::ManageVoice(QUdpSocket *socket, QObject *parent) :
    QObject(parent)
{
    this->socket = socket;
    this->gui = new GUI();

    settings = gui->getSettings();

    connect(gui, SIGNAL(startButtonPressed()), this, SLOT(startRecording()));
    connect(gui, SIGNAL(stopButtonPressed()), this, SLOT(stopRecording()));

    audioInput = NULL;
    QDateTime now = QDateTime::currentDateTime();
    timestamp = now.currentDateTime().toMSecsSinceEpoch();

    isRecording = false;

}

void ManageVoice::showGUI() {
    gui->show();
}

void ManageVoice::startRecording() {
    if(!isRecording) {
        broadcasting_port = gui->getBroadcastingPort();
        format = settings->getSpeakerAudioFormat();

        QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
        if (!info.isFormatSupported(format)) {
            qWarning()<<"raw audio format not supported by backend, cannot play audio.";
            return;
        }

       audioInput = new QAudioInput(format, this);
       intermediateDevice  = audioInput->start();
       connect(intermediateDevice, SIGNAL(readyRead()), this, SLOT(transferData()));

       isRecording = true;
    }
}

void ManageVoice::transferData(){
    if(audioInput->bytesReady() > 0) {
        QByteArray chunk;
        QByteArray sendBuffer;
        chunk = intermediateDevice->readAll();
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

void ManageVoice::stopRecording()
{
    if(isRecording) {
        audioInput->stop();
        isRecording = false;
    }
}

ManageVoice::~ManageVoice() {
    if(audioInput != NULL) {
        audioInput->stop();
        delete audioInput;
    }
    delete gui;
    qDebug() << "Managevoice destruct!";
}

