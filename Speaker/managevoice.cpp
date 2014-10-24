#include "managevoice.h"

ManageVoice::ManageVoice(QObject *parent, Settings *settings) :
    QObject(parent)
{

    this->socket = new QUdpSocket(this);

    this->settings = settings;

    IPAddress = NULL;

    audioInput = NULL;
    QDateTime now = QDateTime::currentDateTime();
    timestamp = now.currentDateTime().toMSecsSinceEpoch();

    isRecording = false;
    broadcasting_port = -1;

}


void ManageVoice::changeRecordState(QString IPAddress, QString port) {

    if(!isRecording) {
        //check if input port is valid number
        bool ok = false;
        broadcasting_port = port.toInt(&ok);
        if(!ok) {
            emit errorMessage("Invalid port!");
        }
        else {
            //check if IP is valid
            if(checkIP(IPAddress)) {
                if(this->IPAddress == NULL) {
                    this->IPAddress = new QHostAddress(IPAddress);
                }
                else {
                    delete this->IPAddress;
                    this->IPAddress = new QHostAddress(IPAddress);
                }
                startRecording();
            }
            else {
              emit errorMessage("Invalid IP!");
            }
        }
    }
    else {
        stopRecording();
    }
}

bool ManageVoice::checkIP(QString ip) {
    QHostAddress address(ip);
    if (QAbstractSocket::IPv4Protocol == address.protocol())
    {
       qDebug("Valid IPv4 address.");
       return true;
    }
    else if (QAbstractSocket::IPv6Protocol == address.protocol())
    {
       qDebug("Valid IPv6 address.");
       return true;
    }
    else
    {
       qDebug("Unknown or invalid address.");
       return false;
    }
}

void ManageVoice::startRecording() {
    if(!isRecording && broadcasting_port > 0) {
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
       emit recordingState(isRecording);
    }
}

void ManageVoice::transferData(){
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

        emit dataSent(sendBuffer.size());

        socket->writeDatagram(sendBuffer, *IPAddress, broadcasting_port);
    }
}

void ManageVoice::stopRecording()
{
    if(isRecording) {
        audioInput->stop();
        delete audioInput;
        isRecording = false;
        emit recordingState(isRecording);
    }
}


ManageVoice::~ManageVoice() {
    stopRecording();
    delete this->IPAddress;
    qDebug() << "Managevoice destruct!";
}

