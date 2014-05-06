#ifndef AUDIOCHUNK_H
#define AUDIOCHUNK_H

#include <QByteArray>

struct AudioChunk {
    QByteArray chunk;
    int timestamp;
};

#endif // AUDIOCHUNK_H
