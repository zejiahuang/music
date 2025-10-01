#include "playlistmanager.h"
#include "playerwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    PlaylistManager manager;
    manager.loadPlaylists("./mymusic");
    manager.scanMusicFolders("./music"， "./mymusic");
    manager.savePlaylists("./mymusic");

    PlayerWindow w;
    w.show();
    return app.exec();
}
