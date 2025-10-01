#include "playlist.h"

QJsonObject Playlist::toJson() const {
    QJsonObject obj;
    obj["name"] = name;
    QJsonArray arr;
    for (const auto& s : songs) {
        QJsonObject so;
        so["filePath"] = s.filePath;
        so["title"] = s.title;
        so["artist"] = s.artist;
        so["album"] = s.album;
        so["durationMs"] = QString::number(s.durationMs);
        so["lyrics"] = s.lyrics;
        arr.append(so);
    }
    obj["songs"] = arr;
    return obj;
}

Playlist Playlist::fromJson(const QJsonObject &obj) {
    Playlist pl;
    pl.name = obj["name"].toString();
    for (const auto& v : obj["songs"].toArray()) {
        QJsonObject so = v.toObject();
        SongInfo s;
        s.filePath = so["filePath"].toString();
        s.title = so["title"].toString();
        s.artist = so["artist"].toString();
        s.album = so["album"].toString();
        s.durationMs = so["durationMs"].toString().toLongLong();
        s.lyrics = so["lyrics"].toString();
        pl.songs.append(s);
    }
    return pl;
}