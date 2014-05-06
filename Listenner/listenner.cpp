#include "listenner.h"

const int BufferSize = 14096;

Listenner::Listenner(GUI *gui, QObject *parent) :
    m_Outputdevice(QAudioDeviceInfo::defaultOutputDevice()),
    m_audioOutput(0),
    m_buffer(BufferSize, 0),
    QObject(parent)
{
    socket = new QUdpSocket(this);
    buffer = new QByteArray();

    this->gui = gui;

    socket->bind(QHostAddress::AnyIPv4, 45000, QUdpSocket::ShareAddress);
    groupAddress = QHostAddress("239.255.43.21");
    socket->joinMulticastGroup(groupAddress);

    format.setSampleRate(8000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);


    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        return;

    }
    m_audioOutput = new QAudioOutput(m_Outputdevice, format, this);

    m_output= m_audioOutput->start();

    qreal volume = gui->getVolume();
    m_audioOutput->setVolume(volume);

    connect(gui, SIGNAL(startPlayback()), this, SLOT(playback()));
    connect(gui, SIGNAL(stopPlayback()), this, SLOT(stopPlayback()));
    connect(gui, SIGNAL(volumeChanged()), this, SLOT(volumeChanged()));
    connect(gui, SIGNAL(portChanged(int)), this, SLOT(portChanged(int)));

}

void Listenner::receiveDatagramm() {
    qint64 length = socket->bytesAvailable();
    if(length > 0) {
        QByteArray *m_buffer = new QByteArray;
        m_buffer->resize(length);
        socket->readDatagram(m_buffer->data(), length);
        gui->setDataReceived(length);
        m_output->write((char*)m_buffer->data(), length);
    }
    qDebug() << length;
}

void Listenner::playback() {
    m_output = m_audioOutput->start();
    qDebug() << "start";
    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
    if(socket->bytesAvailable()) {
        receiveDatagramm();
    }
}

void Listenner::stopPlayback() {
    m_audioOutput->stop();
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(receiveDatagramm()));
}

void Listenner::volumeChanged() {
    qreal volume = gui->getVolume();
    qDebug() << volume;
    m_audioOutput->setVolume(volume/100);
}

void Listenner::portChanged(int port) {
    stopPlayback();
    socket->leaveMulticastGroup(groupAddress);
    delete socket;
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);
    socket->joinMulticastGroup(groupAddress);
    playback();
    gui->playbackButtonPushed();
}
