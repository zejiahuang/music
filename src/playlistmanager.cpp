#include "playlistmanager.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include "taglib_utils.h"
#include "ffmpeg_waveform.h"

void PlaylistManager::scanMusicFolders(const QString& musicRootDir, const QString& myMusicDir) {
    QDir musicDir(musicRootDir);
    for (const QString& folderName : musicDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString folderPath = musicDir.absoluteFilePath(folderName);
        QString playlistJsonPath = myMusicDir + "/" + folderName + ".json";
        
        if (QFile::exists(playlistJsonPath)) {
            continue;
        }
        
        Playlist pl;
        pl.name = folderName;
        QDir fdir(folderPath);
        QStringList filters = {"*.mp3"， "*.flac"， "*.wav"， "*.ape"， "*.aac"， "*.ogg"， "*.m4a"};
        
        for (const QString& musicFile : fdir.entryList(filters, QDir::Files)) {
            QString fullPath = fdir.absoluteFilePath(musicFile);
            SongInfo s = readAudioMeta(fullPath);
            s.waveform = extractWaveformFFmpeg(fullPath, 256);
            pl.songs.append(s);
        }
        
        playlists.append(pl);
        
        QFile f(playlistJsonPath);
        if (f.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(pl.toJson());
            f.write(doc.toJson());
            f.close();
        }
    }
}

void PlaylistManager::loadPlaylists(const QString& myMusicDir) {
    // Implement loading playlists logic
    QDir dir(myMusicDir);
    QStringList filters = {"*.json"};
    for (const QString& file : dir.entryList(filters, QDir::Files)) {
        QString filePath = dir.absoluteFilePath(file);
        QFile f(filePath);
        if (f.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            Playlist pl = Playlist::fromJson(doc.object());
            playlists.append(pl);
            f.close();
        }
    }
}

void PlaylistManager::savePlaylists(const QString& myMusicDir) {
    // Ensure directory exists
    QDir dir(myMusicDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Save all playlists
    for (const Playlist& pl : playlists) {
        QString filePath = myMusicDir + "/" + pl.name + ".json";
        QFile f(filePath);
        if (f.open(QIODevice::WriteOnly)) {
            QJsonDocument doc(pl.toJson());
            f.write(doc.toJson());
            f.close();
        }
    }
}
