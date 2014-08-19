#include "listener.h"

const int BufferSize = 14096;

Listener::Listener(QObject *parent) :
    m_Outputdevice(QAudioDeviceInfo::defaultOutputDevice()),
    m_audioOutput(0),
    m_buffer(BufferSize, 0),
    QThread(parent)
{
    socket = new QUdpSocket(this);
    outputBuffer = new QMap<qint64, QByteArray>();

    gui = new GUI();
    binded_port = -1;
    timestamp = 0;

    settings = gui->getSettings();
    format = settings->getListennerAudioFormat();

    isPlaying = false;

    connect(gui, SIGNAL(startPlayback()), this, SLOT(playback()));
    connect(gui, SIGNAL(stopPlayback()), this, SLOT(stopPlayback()));
    connect(gui, SIGNAL(volumeChanged()), this, SLOT(volumeChanged()));
    connect(gui, SIGNAL(portChanged(int)), this, SLOT(portChanged(int)));

    m_audioOutput = new QAudioOutput(m_Outputdevice, format, this);

    qreal volume = gui->getVolume();
    m_audioOutput->setVolume(volume);
    isPlaying = false;

}

Listener::~Listener() {
    this->stopPlayback();
    if(m_audioOutput != NULL) {
        delete m_audioOutput;
    }
    outputBuffer->clear();
    delete outputBuffer;
    delete gui;
    qDebug() << "Listener destruct!";
}

void Listener::showGUI() {
    gui->show();
}


void Listener::receiveDatagramm() {
    qint64 length = socket->bytesAvailable();
    qint64 temp;
    if(length > 0) {
        QByteArray m_buffer;
        m_buffer.resize(length);
        socket->readDatagram(m_buffer.data(), length);
        gui->setDataReceived(length);
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
                m_output->write(decomp.data(), decomp.size());
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
        m_output = m_audioOutput->start();
        connect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
        if(socket->bytesAvailable()) {
            receiveDatagramm();
        }
        isPlaying = true;
    }
}

void Listener::stopPlayback() {
    if(isPlaying) {
        m_audioOutput->stop();
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
        isPlaying = false;
    }
}

void Listener::volumeChanged() {
    qreal volume = gui->getVolume();
    qDebug() << volume;
    m_audioOutput->setVolume(volume/100);
}

void Listener::portChanged(int port) {
    stopPlayback();
    delete socket;
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);
    playback();
}
