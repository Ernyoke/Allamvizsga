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


void ManageVoice::changeRecordState(QAudioFormat speakerFormat) {

    if(!isRecording) {
        this->IPAddress = settings->getServerAddress();
        this->broadcasting_port = settings->getClientPortForSound();
        format = speakerFormat;
        startRecording();
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
        //initialize byte array
        QByteArray chunk;
        chunk = intermediateDevice->read(buffLen);
        timestamp++;
        //serialize data conform to the protocol
        SoundChunk *soundChunk = new SoundChunk(format.sampleRate(), format.channelCount(), format.codec(), &chunk);
        //create datagram
        Datagram dataGram(Datagram::SOUND, settings->getClientId(), timestamp, soundChunk);
        qDebug() << timestamp;
        //send the data
        dataGram.sendDatagram(socket, IPAddress, broadcasting_port);
        emit dataSent(dataGram.getSize());
        delete soundChunk;
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

bool ManageVoice::isRunning() {
    return this->isRecording;
}

