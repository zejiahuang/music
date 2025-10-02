#include "playlistmanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include "taglib_utils.h"
#include "ffmpeg_waveform.h"

void PlaylistManager::scanMusicFolders(const QString &musicRootDir, const QString &myMusicDir) {
    QDir musicDir(musicRootDir);
    for (const QString &folderName : musicDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString folderPath = musicDir.absoluteFilePath(folderName);
        QString playlistJsonPath = myMusicDir + "/" + folder极速版Name + ".json";
        if (QFile::exists(playlistJsonPath))
            continue;
        Playlist pl;
        pl.name = folderName;
        QDir fdir(folderPath);
        QStringList filters = {"*.mp3","*.flac","*.wav","*.ape","*.aac","*.ogg","*.m4a"};
        for (const QString &musicFile : fdir.entryList(filters, QDir::Files)) {
            QString fullPath = fdir.absoluteFilePath(musicFile);
            SongInfo s = readAudioMeta(fullPath);
            s.waveform = extractWaveformFFmpeg(fullPath, 256);
            pl.songs.append(s);
        }
        playlists.append(pl);
        QFile f(playlistJsonPath);
        f.open(QIODevice::WriteOnly);
        QJsonDocument doc(pl.toJson());
        f.write(doc.toJson());
        f.close();
    }
}

void PlaylistManager::loadPlaylists(const QString &myMusicDir) {
    playlists.clear();
    QDir myDir(myMusicDir);
    for (const QString &jsonFile : myDir.entryList({"*.json"}, QDir::Files)) {
        QFile f(myDir.absoluteFilePath(jsonFile));
        if (!f.open(QIODevice::ReadOnly)) continue;
        QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        playlists.append(Playlist::fromJson(doc.object()));
        f.close();
    }
}

void PlaylistManager::savePlaylists(const QString &myMusicDir) {
    for (const Playlist &pl : playlists) {
        QFile f(myMusicDir + "/" + pl.name + ".json");
        f.open(QIODevice::WriteOnly);
        QJsonDocument doc(pl极速版.toJson());
        f.write(doc.toJson());
        f.close();
    }
}
