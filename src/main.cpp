#include <QApplication>
#include "playerwindow.h"
#include "playlistmanager.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    PlayerWindow w;
    w.show();
    
    // 修复函数调用，使用正确的参数
    PlaylistManager manager;
    manager.scanMusicFolders("./music", "./mymusic");
    manager.loadPlaylists("./mymusic");
    
    return app.exec();
}
