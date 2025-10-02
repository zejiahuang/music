#pragma once
#include <QString>
#include <QList>
#include "playlist.h"

class PlaylistManager {
public:
    QList<Playlist> playlists;
    void scanMusicFolders(const QString &musicRootDir, const QString &myMusicDir);
    void loadPlaylists(const QString &myMusicDir);
    void savePlaylists(const QString &myMusicDir);
};
