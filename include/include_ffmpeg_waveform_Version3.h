#pragma once
#include <QString>
#include <QVector>

QVector<float> extractWaveformFFmpeg(const QString &filePath, int samplePoints = 256);