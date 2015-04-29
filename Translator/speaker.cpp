#include "speaker.h"

Speaker::Speaker() : AbstractSpeaker()
{
}

void Speaker::start(QAudioFormat speakerFormat, QAudioDeviceInfo device,
                    QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId) {

    if(!isRecording) {

        format = speakerFormat;
        socket = new QUdpSocket(this);
        this->broadcasting_port = broadcasting_port;
        this->serverAddress = serverAddress;
        timestamp = Datagram::generateTimestamp();
        this->clientId = clientId;
        QAudioDeviceInfo info = device;

        if (!info.isFormatSupported(format)) {
            emit errorMessage("Audio format is not supported!");
            return;
        }

        audioInput = new QAudioInput(info, format, this);
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
        Datagram dataGram(Datagram::SOUND, clientId, timestamp, soundChunk);
        //send the data
        dataGram.sendDatagram(socket, &serverAddress, broadcasting_port);
        emit dataSent(dataGram.getSize());
        delete soundChunk;
    }
}

void Speaker::stop()
{
    if(isRecording) {
        audioInput->stop();
        delete audioInput;
        isRecording = false;
        emit recordingState(isRecording);
        emit finished();
    }
}


Speaker::~Speaker() {
    stop();
    qDebug() << "Managevoice destruct!";
}

