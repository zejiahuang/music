#pragma once
#include <QString>
#include <QImage>
#include <QVector>

struct SongInfo {
    QString filePath;
    QString title;
    QString artist;
    QString album;
    qint64 durationMs;
    QString lyrics;
    QImage cover;
    QVector<float> waveform; // 可视化用波形数据
};