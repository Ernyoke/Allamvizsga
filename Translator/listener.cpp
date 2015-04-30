#include "listener.h"

const int BufferSize = 14096;

Listener::Listener() :
    m_audioOutput(0),
    m_buffer(BufferSize, 0)
{
    isPlaying = false;
    record = NULL;
}

Listener::~Listener() {
    this->stop();
    if(record != NULL) {
        record->stop();
        delete record;
    }
    outputBuffer->clear();
    delete outputBuffer;
    qDebug() << "Listener destruct!";
}

void Listener::receiveDatagramm() {
    qint64 length = socket->bytesAvailable();
    qint64 temp;
    while(length > 0) {
        QByteArray m_buffer;
        m_buffer.resize(length);
        int datagramSize = socket->readDatagram(m_buffer.data(), length);
        m_buffer.resize(datagramSize);
        emit dataReceived(datagramSize);
        Datagram datagram(&m_buffer);
        temp = datagram.getTimeStamp();
        if(timestamp < temp) {
            QByteArray content = datagram.getContent();
            SoundChunk soundChunk(&content);
            outputBuffer->insert(temp, soundChunk);
            QByteArray aux;
            if(outputBuffer->size() == 4) {
                for(QMap<qint64, SoundChunk>::iterator iter = outputBuffer->begin(); iter != outputBuffer->end(); ++iter) {
                    SoundChunk out = iter.value();
                    aux.append(out.getRawSound());
                    timestamp = temp;
                }
                QByteArray decomp;
                decomp.append(aux);
                m_output->write(decomp.data(), decomp.size());
                storeChunk(decomp);
                outputBuffer->clear();
            }
        }
        else {
        }
        length = socket->bytesAvailable();
    }
}

void Listener::start(QSharedPointer<ChannelInfo> channel, QAudioDeviceInfo device,
                     QHostAddress serverAddress, qreal volume) {

    outputBuffer = new QMap<qint64, SoundChunk>();
    this->binded_port = channel->getOutPort();
    this->serverAddress = serverAddress;

    format.setSampleRate(channel->getSampleRate());
    format.setSampleSize(channel->getSampleSize());
    format.setChannelCount(channel->getChannels());
    format.setCodec(channel->getCodec());
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    if(!isPlaying) {

        socket = new QUdpSocket(this);
        if(!socket->bind(QHostAddress::AnyIPv4, binded_port, QUdpSocket::ShareAddress)) {
            emit errorMessage("Error binding to shareaddress!");
            emit changePlayButtonState(isPlaying);
            emit finished();
            return;
        }

        try {
        m_Outputdevice = device;

        if(!m_Outputdevice.isFormatSupported(format)) {
            emit errorMessage("Format not supported!");
            emit changePlayButtonState(isPlaying);
            emit finished();
            return;
        }

        m_audioOutput = new QAudioOutput(m_Outputdevice, format, this);
        m_audioOutput->setBufferSize(BufferSize);

        m_audioOutput->setVolume(volume);

        m_output = m_audioOutput->start();
        connect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
        if(socket->bytesAvailable()) {
            receiveDatagramm();
        }
        isPlaying = true;
        emit changePlayButtonState(isPlaying);
        }
        catch(NoAudioDeviceException *exception) {
            emit errorMessage(exception->message());
            delete exception;
        }
    }
    else {
//        emit finished();
    }
}

void Listener::stop() {
    if(isPlaying) {
        m_audioOutput->stop();
        delete m_audioOutput;
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
        delete socket;
        isPlaying = false;
        emit changePlayButtonState(isPlaying);
        emit finished();
    }
}


void Listener::volumeChanged(qreal volume) {
    if(isPlaying) {
        m_audioOutput->setVolume(volume);
    }
}

void Listener::startRecord(Settings::CODEC codec, QString path) {
    if(record == NULL) {
        switch(codec) {
        case Settings::WAV: {
            record = new RecordWav(path, format, this);
            connect(record, SIGNAL(recordingState(RecordAudio::STATE)), this, SLOT(recordingStateChanged(RecordAudio::STATE)));
            if(record->getState() == RecordAudio::STOPPED) {
                if(!record->start()) {
                    QMessageBox msgBox;
                    msgBox.setText("Temporary record file can not be created. Please change record file path in the Settings!");
                    msgBox.exec();
                }
            }
            break;
        }
        }
    }
    else {
        if(record->getState() == RecordAudio::RECORDING || record->getState() == RecordAudio::PAUSED) {
            record->stop();
            delete record;
            record = NULL;
        }
    }
}

void Listener::storeChunk(QByteArray data) {
    if(record != NULL) {
        record->write(data);
    }
}

void Listener::pauseRecord() {
    if(record != NULL) {
        record->pause();
    }
}


//update GUI after recording state is changed on Recordaudio's side
void Listener::recordingStateChanged(RecordAudio::STATE state) {
    if(state == RecordAudio::PAUSED) {
        emit changePauseButtonState(state);
    }
    else {
        emit changePauseButtonState(state);
        emit changeRecordButtonState(state);
    }
}

bool Listener::isRecRunning() {
    if(record != NULL) {
        if(record->getState() != RecordAudio::STOPPED) {
            return true;
        }
    }
    return false;
}

