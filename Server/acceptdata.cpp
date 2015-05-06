#include "acceptdata.h"

const int SOUND_PORT = 20000;

AcceptData::AcceptData(QHostAddress broadcastAddress, QMutex *mutex) : PacketLogger(mutex)
{
    qRegisterMetaType<ChannelInfo>("ChannelInfo");
    this->broadcastAddress = broadcastAddress;
}

void AcceptData::init() {
    socket = new QUdpSocket(this);
    socket->bind(QHostAddress::Any, SOUND_PORT);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readData()));
    isDataAvailable = false;
    isRunning = true;
}

AcceptData::~AcceptData() {
    QMapIterator<qint32, ChannelInfo*> iter(channels);
    while(iter.hasNext()) {
        delete iter.value();
        channels.remove(iter.key());
    }
}

void AcceptData::readData() {
    isDataAvailable = true;
    while(socket->hasPendingDatagrams()) {
        if(isRunning == false) {
            break;
        }
        data.resize(socket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(data.data(), data.size(), &sender, &senderPort);
        Datagram receivedDatagram(&data);
        createLogEntry(receivedDatagram);
        if(receivedDatagram.getId() == Datagram::SOUND) {
            QMap<qint32, ChannelInfo*>::iterator iter = channels.find(receivedDatagram.getClientId());
            if(iter != channels.end()) {
                qint16 port = iter.value()->getOutPort();
                socket->writeDatagram(data.data(), data.size(), broadcastAddress, port);
            }
        }
    }
    isDataAvailable = false;
    if(!isRunning) {
        emit finished();
    }
}

void AcceptData::addChannel(ChannelInfo channel) {
    ChannelInfo *channelInfo = new ChannelInfo(channel);
    channels.insert(channel.getOwner(), channelInfo);
}

void AcceptData::changeBroadcastAddress(QString address) {
    this->broadcastAddress = QHostAddress(address);
    qDebug() << "address changed";
}

void AcceptData::removeChannel(qint32 id) {
    QMap<qint32, ChannelInfo*>::iterator iter;
    iter = channels.find(id);
    if(iter != channels.end()) {
        ChannelInfo *temp = iter.value();
        channels.remove(id);
        delete temp;
    }
}

void AcceptData::stopWorker() {
    isRunning = false;
    stopPacketLog();
    if(!isDataAvailable) {
        emit finished();
    }
}

