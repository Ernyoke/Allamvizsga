#ifndef RECORDWAV_H
#define RECORDWAV_H

#include <QObject>
#include <QIODevice>
#include <QAudioFormat>
#include <QDataStream>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <QDir>
#include <QAbstractButton>
#include "recordaudio.h"

class RecordWav : public RecordAudio
{
    Q_OBJECT
public:
    explicit RecordWav(QString, QAudioFormat, QObject *parent = 0);

    bool start();
    bool stop();
    bool pause();
    bool write(QByteArray);

private:
    void writeHeader();

    void finalize();
    void file();
    QString extension();

signals:

public slots:

};

#endif // RECORDWAV_H
