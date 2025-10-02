#include "ffmpegplayer.h"
#include <QDebug>

class FFmpegPlayer::Private {
public:
    // 实现细节
    bool isPlaying = false;
    qint64 currentPosition = 0;
    qint64 totalDuration = 0;
};

FFmpegPlayer::FFmpegPlayer(QObject *parent) : QObject(parent), d(new Private) {}

FFmpegPlayer::~FFmpegPlayer() {
    delete d;
}

bool FFmpegPlayer::load(const QString &filePath) {
    qDebug() << "Loading file:" << filePath;
    // 实现加载逻辑
    d->totalDuration = 1000 * 60 * 3; // 3分钟示例
    emit durationChanged(d->totalDuration);
    return true;
}

void FFmpegPlayer::play() {
    qDebug() << "Play";
    d->isPlaying = true;
    emit playbackStarted();
}

void FFmpegPlayer::pause() {
    qDebug() << "Pause";
    d->isPlaying = false;
    emit playbackPaused();
}

void FFmpegPlayer::stop() {
    qDebug() << "Stop";
    d->isPlaying = false;
    d->currentPosition = 0;
    emit playbackStopped();
}

void FFmpegPlayer::seek(qint64 positionMs) {
    qDebug() << "Seek to:" << positionMs;
    d->currentPosition = positionMs;
    emit positionChanged(positionMs);
}

qint64 FFmpegPlayer::duration() const {
    return d->totalDuration;
}

qint64 FFmpegPlayer::position() const {
    return d->currentPosition;
}
