#include "listener.h"

const int BufferSize = 14096;

Listener::Listener(Settings *settings) :
    m_audioOutput(0),
    m_buffer(BufferSize, 0)
{
    socket = new QUdpSocket(this);
    outputBuffer = new QMap<qint64, QByteArray>();

    binded_port = -1;
    timestamp = 0;

    isPlaying = false;
    record = NULL;

    this->settings = settings;
}

Listener::~Listener() {
    delete outputBuffer;
    qDebug() << "Listener destruct!";
}

//this SLOT is called when Start/Stop button is pushed
void Listener::changePlaybackState(int port) {
    //if the client is receiveng packets then stop
    if(isPlaying) {
        qDebug() << "stopped";
        this->stopPlayback();
    }
    //otherswise start receiving
    else {
        qDebug() << "playing";
        binded_port = port;
        this->playback();
    }
}

void Listener::receiveDatagramm() {
    qint64 length = socket->bytesAvailable();
    qint64 temp;
    if(length > 0) {
        QByteArray m_buffer;
        m_buffer.resize(length);
        socket->readDatagram(m_buffer.data(), length);
        emit dataReceived(length);
        memcpy(&temp, m_buffer.data(), sizeof(qint64));
        if(timestamp < temp) {
            m_buffer.remove(0, sizeof(qint64));
            outputBuffer->insert(temp, m_buffer);
            QByteArray aux;
            if(outputBuffer->size() == 4) {
                for(QMap<qint64, QByteArray>::iterator iter = outputBuffer->begin(); iter != outputBuffer->end(); ++iter) {
                    QByteArray out = iter.value();
                    aux.append(out);
                    timestamp = temp;
                }
                QByteArray decomp;
                decomp.append(aux);
               /* for(int i = 0; i < asd.size(); ++i) {
                    short tmp = G711::Snack_Alaw2Lin(asd[i]);
                    decomp.append(tmp);
                    decomp.append(tmp >> 8);
                }*/
                m_output->write(aux.data(), aux.size());
//                storeChunk(decomp);
                outputBuffer->clear();
            }
        }
        else {
            qDebug() << temp;
        }
    }
}

void Listener::playback() {
    if(!isPlaying) {

        socket = new QUdpSocket(this);
        socket->bind(QHostAddress::AnyIPv4, binded_port, QUdpSocket::ShareAddress);

        format = settings->getListennerAudioFormat();
        m_Outputdevice = settings->getOutputDevice();

        if(!m_Outputdevice.isFormatSupported(format)) {
            qWarning() << "Format not supported!";
            return;
        }

        m_audioOutput = new QAudioOutput(m_Outputdevice, format, this);
        m_audioOutput->setBufferSize(16384);

        qreal volume = 0.5;
        m_audioOutput->setVolume(volume);

        m_output = m_audioOutput->start();
        connect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
        if(socket->bytesAvailable()) {
            receiveDatagramm();
        }
        isPlaying = true;
        emit changePlayButtonState(isPlaying);
    }
}

void Listener::stopPlayback() {
    if(isPlaying) {
        m_audioOutput->stop();
        delete m_audioOutput;
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
        delete socket;
        isPlaying = false;
        emit changePlayButtonState(isPlaying);
    }
}

void Listener::volumeChanged(int volume_int) {
    qreal volume = volume_int;
    if(isPlaying) {
        m_audioOutput->setVolume(volume/100);
    }
}

void Listener::portChanged(int port) {
    qDebug() << "portChanged";
    binded_port = port;
    stopPlayback();
    playback();
}

void Listener::startRecord() {
    if(record == NULL) {
        Settings::CODEC codec = settings->getRecordCodec();
        QString path = settings->getRecordPath();
        qDebug() << path;
        switch(codec) {
        case Settings::WAV: {
//            record = new RecordWav(path, format, this);
//            if(record->getState() == RecordAudio::STOPPED) {
//                if(!record->start()) {
//                    QMessageBox msgBox;
//                    msgBox.setText("Temporary record file can not be created. Please change record file path in the Settings!");
//                    msgBox.exec();
//                }
//                gui->changeRecordButtonState(record->getState());
//            }
            break;
        }
        }
    }
    else {
        if(record->getState() == RecordAudio::RECORDING || record->getState() == RecordAudio::PAUSED) {
            record->stop();
//            gui->changeRecordButtonState(record->getState());
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
//        gui->changePauseButtonState(record->getState());
    }
}

void Listener::stopRunning() {
    this->stopPlayback();
    outputBuffer->clear();
    emit finished();
}

