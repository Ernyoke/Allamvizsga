#include "speaker.h"

Speaker::Speaker(Settings *settings) :
Worker(settings)
{
    this->socket = new QUdpSocket(this);

    IPAddress = NULL;

    audioInput = NULL;
    QDateTime now = QDateTime::currentDateTime();
    timestamp = now.currentDateTime().toMSecsSinceEpoch();
    isRecording = false;
}

Speaker::~Speaker() {
    stopRecording();
    qDebug() << "Speaker deleted!";
}

void Speaker::changeRecordState(QAudioFormat speakerFormat) {

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

void Speaker::startRecording() {
    if(!isRecording) {

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
       emit recordingState(true);
    }
}

void Speaker::transferData(){
    if(audioInput->bytesReady() >= buffLen) {
        //initialize byte array
        QByteArray chunk;
        chunk = intermediateDevice->read(buffLen);
        timestamp++;
        //serialize data conform to the protocol
        SoundChunk *soundChunk = new SoundChunk(format.sampleRate(), format.sampleSize(), format.channelCount(), format.codec(), &chunk);
        //create datagram
        Datagram dataGram(Datagram::SOUND, settings->getClientId(), timestamp, soundChunk);
        qDebug() << timestamp;
        //send the data
        dataGram.sendDatagram(socket, &IPAddress, broadcasting_port);
        emit dataSent(dataGram.getSize());
        delete soundChunk;
    }
}

void Speaker::stopRecording()
{
    if(isRecording) {
        audioInput->stop();
        delete audioInput;
        isRecording = false;
        emit recordingState(false);
    }
}

void Speaker::stopRunning() {
    stopRecording();
    emit finished();
}

