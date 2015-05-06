#include "testspeaker.h"

TestSpeaker::TestSpeaker() : AbstractSpeaker()
{
}

TestSpeaker::~TestSpeaker()
{
    delete file;
//    delete timer;
}

void TestSpeaker::start(QAudioFormat speakerFormat, QAudioDeviceInfo device,
                        QHostAddress serverAddress, qint32 broadcasting_port, qint32 clientId) {
    if(!isRecording) {

        format = speakerFormat;
        socket = new QUdpSocket(this);
        this->broadcasting_port = broadcasting_port;
        this->serverAddress = serverAddress;
        timestamp = Datagram::generateTimestamp();
        this->clientId = clientId;

        isRecording = true;
        file = new QFile("test.raw");
        if(!file->open(QIODevice::ReadOnly)) {
            emit errorMessage("File could not be opened!");
            return;
        }
        else {

            int interval = 10;
            buffLen = calcBufferSize(format, interval);
//            timer = new QTimer;
//            timer->setInterval(1000 / interval);
//            connect(timer, SIGNAL(timeout()), this, SLOT(transferData()));
//            timer->start();

            timerId = this->startTimer(1000 / interval, Qt::PreciseTimer);

            emit recordingState(isRecording);
        }
    }

}

void TestSpeaker::timerEvent(QTimerEvent *event) {
    transferData();
}

void TestSpeaker::transferData() {
    if(file->atEnd()) {
        file->seek(0);
    }
    char *buffer = new char[buffLen];
    int bytes = file->read(buffer, buffLen);
    QByteArray content(buffer, buffLen);

    if(bytes > 0) {
        SoundChunk *soundChunk = new SoundChunk(format.sampleRate(), format.sampleSize(), format.channelCount(), format.codec(), &content);
        Datagram dataGram(Datagram::SOUND, clientId, timestamp, soundChunk);
        //send the data
        dataGram.sendDatagram(socket, &serverAddress, broadcasting_port);
        emit dataSent(dataGram.getSize());
        delete soundChunk;
        timestamp++;
    }
    delete buffer;
}

void TestSpeaker::stop() {
    if(isRecording) {
        isRecording = false;
//        timer->stop();
        this->killTimer(timerId);

        file->close();
        emit recordingState(isRecording);
        emit finished();
    }
}

int TestSpeaker::calcBufferSize(QAudioFormat &format, int interval) {
    int freq = format.sampleRate();
    int sampleSize = format.sampleSize();
    return ((freq * sampleSize) / 8) / interval;
}

