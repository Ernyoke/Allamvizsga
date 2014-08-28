#include "recordwav.h"

RecordWav::RecordWav(QString filename, QAudioFormat format, GUI* gui, QObject *parent) :
    RecordAudio(filename, format, gui, parent)
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
    if(currentState == STOPPED) {
        if(!outputfile->open(QIODevice::WriteOnly)) {
            return false;
        }
        writeHeader();
        currentState = RECORDING;
    }
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
    }
    return true;
}

bool RecordWav::pause() {
    if(currentState == RECORDING) {
        currentState = PAUSED;
    }
    else if(currentState == PAUSED) {
        currentState = RECORDING;
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

      bool ok = false;
      bool renameOK = false;
      while(!renameOK) {
          QString newName = QInputDialog::getText(gui, tr("Save file as:"),
                                                  tr("Filename:"), QLineEdit::Normal,
                                                  tr(""), &ok);
          if(ok && !newName.isEmpty()) {
              if(outputfile->rename(path + "/" + newName + ".wav")) {
                  renameOK = true;
              }
              else {
                   QMessageBox msgBox;
                   msgBox.setText("File could not saved.");
                   msgBox.setInformativeText("Would you like to enter a new filname or save it as a temporary(tmp.wav) file?");
                   QPushButton *tryAgain = msgBox.addButton(tr("Try Again"), QMessageBox::ActionRole);
                   QPushButton *save = msgBox.addButton(tr("Save as temporary"), QMessageBox::ActionRole);
                   QPushButton *del = msgBox.addButton(tr("Delete"), QMessageBox::ActionRole);

                   msgBox.exec();

                   if((QPushButton*)msgBox.clickedButton() == tryAgain) {
                       renameOK = false;
                   }
                   else {
                       if((QPushButton*)msgBox.clickedButton() == save) {
                           renameOK = true;
                       }
                       else {
                           if((QPushButton*)msgBox.clickedButton() == del) {
                               outputfile->remove();
                               renameOK = true;
                           }
                       }
                   }
              }
          }
      }

}
