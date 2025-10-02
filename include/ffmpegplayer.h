#pragma once
#include <QObject>

class FFmpegPlayer : public QObject {
    Q_OBJECT
public:
    explicit FFmpegPlayer(QObject *parent = nullptr);
    ~FFmpegPlayer();
    
    bool load(const QString &filePath);
    void play();
    void pause();
    void stop();
    void seek(qint64 positionMs);
    
    qint64 duration() const;
    qint64 position() const;
    
signals:
    void positionChanged(qint64 ms);
    void durationChanged(qint64 ms);
    void playbackStarted();
    void playbackPaused();
    void playbackStopped();
    void errorOccurred(const QString &errorMessage);
    
private:
    // 私有实现细节
    class Private;
    Private *d;
};