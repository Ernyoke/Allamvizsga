#ifndef NEWCHANNELDIALOG_H
#define NEWCHANNELDIALOG_H

#include <QDialog>
#include <QUdpSocket>
#include <QTimer>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QComboBox>

#include "channelinfo.h"
#include "datagram.h"
#include "invalididexception.h"

namespace Ui {
class NewChannelDialog;
}

class NewChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewChannelDialog(QWidget *parent = 0);
    ~NewChannelDialog();

    ChannelInfo getChannelInfo();
    QAudioFormat getAudioFormat();

    bool isChannelAvailable();
    const ChannelInfo* getChannelInformation() const;
    void setClientId(qint32 id);
    void setAudioDeviceInfo(QAudioDeviceInfo deviceInfo);

private:
    Ui::NewChannelDialog *ui;

    qint32 clientId;
    qint64 creationTimeStamp;

    QAudioFormat formatSpeaker;
    QAudioDeviceInfo deviceInfo;
    QString language;
    QUdpSocket *socket;
    QTimer *timer;

    bool isChannelOnline;

    ChannelInfo *chInfo;

    void displayDeviceProperties();
    void clearDeviceProperties();
    int getBoxIndex(QComboBox *box, QString *content);
    QVariant boxValue(const QComboBox *box);

    void sendNewChannelReq();

private slots:
    void setFormatProperties();
    void newChannelRequestTimedOut();
    void newChannelAck(Datagram dgram);

public slots:
    void sendCloseChannelReq();

signals:
    void requestNewChannel(Datagram dgram);
    void closeChannel(Datagram);

};


#endif // NEWCHANNELDIALOG_H
