#pragma once
#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include "songinfo.h"

struct Playlist {
    QString name;
    QList<SongInfo> songs;

    QJsonObject toJson() const;
    static Playlist fromJson(const QJsonObject &obj);
};