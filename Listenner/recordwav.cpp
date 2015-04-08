#include "recordwav.h"

RecordWav::RecordWav(QString filename, QAudioFormat format, QObject *parent) :
    RecordAudio(filename, format, parent)
{
    currentState = STOPPED;
}

void RecordWav::writeHeader() {
    QDataStream out(outputfile);
    out.setByteOrder(QDataStream::LittleEndian);

     // RIFF chunk
    out.writeRawData("RIFF", 4);
    out << quint32(0); // Placeholder for the RIFF chunk size (filled by close())
    out.writeRawData("WAVE", 4);

     // Format description chunk
    out.writeRawData("fmt ", 4);
    out << quint32(16); // "fmt " chunk size (always 16 for PCM)
    out << quint16(1);  // data format (1 => PCM)
    out << quint16(format.channelCount());
    out << quint32(format.sampleRate());
    out << quint32(format.sampleRate() * format.channelCount()
                    * format.sampleSize() / 8 ); // bytes per second
    out << quint16(format.channelCount() * format.sampleSize() / 8); // Block align
    out << quint16(format.sampleSize()); // Significant Bits Per Sample

     // Data chunk
    out.writeRawData("data", 4);
    out << quint32(0);  // Placeholder for the data chunk size (filled by close())
}

bool RecordWav::start() {
    this->file();
    if(currentState == STOPPED) {
        if(!outputfile->open(QIODevice::WriteOnly)) {
            return false;
        }
        writeHeader();
        currentState = RECORDING;
    }
    emit recordingState(currentState);
    return true;
}

bool RecordWav::write(QByteArray data) {
    if(currentState == RECORDING) {
        outputfile->write(data);
    }
    return true;
}

bool RecordWav::stop() {
    if(currentState == RECORDING || currentState == PAUSED) {
        finalize();
        outputfile->close();
        currentState = STOPPED;
        emit recordingState(currentState);
    }
    return true;
}

bool RecordWav::pause() {
    if(currentState == RECORDING) {
        currentState = PAUSED;
        emit recordingState(currentState);
    }
    else if(currentState == PAUSED) {
        currentState = RECORDING;
        emit recordingState(currentState);
    }
    return true;
}

void RecordWav::finalize() {
    // Fill the header size placeholders
      quint32 fileSize = outputfile->size();

      QDataStream out(outputfile);
      // Set the same ByteOrder like in writeHeader()
      out.setByteOrder(QDataStream::LittleEndian);
      // RIFF chunk size
      outputfile->seek(4);
      out << quint32(fileSize - 8);

      // data chunk size
      outputfile->seek(40);
      out << quint32(fileSize - 44);

      finalized = true;
      outputfile->close();

}

void RecordWav::file() {
    //generate filename
    QString fileName("/rec_");
    //get current timestamp for filename
    fileName += QString::number(QDateTime::currentMSecsSinceEpoch());
    //get extension
    fileName += extension();
    outputfile = new QFile(path + fileName);
}

QString RecordWav::extension() {
    return QString(".wav");
}

