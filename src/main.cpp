#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include "../include/playerwindow.h"
#include "../include/playlistmanager.h"

QPixmap createSplashScreen() {
    QPixmap splash(400, 300);
    splash.fill(QColor(111, 66, 193));
    
    QPainter painter(&splash);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制标题
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 24, QFont::Bold));
    painter.drawText(splash.rect(), Qt::AlignCenter, "现代音乐播放器");
    
    // 绘制副标题
    painter.setFont(QFont("Arial", 12));
    painter.drawText(QRect(0, 200, 400, 50), Qt::AlignCenter, "优雅的界面 · 强大的功能");
    
    return splash;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置应用信息
    app.setApplicationName("现代音乐播放器");
    app.setApplicationVersion("2.0.0");
    app.setOrganizationName("Music Player Team");
    
    // 检查是否在CI环境或无头模式
    bool isHeadless = qEnvironmentVariableIsSet("CI") || 
                     qEnvironmentVariableIsSet("GITHUB_ACTIONS") ||
                     qgetenv("QT_QPA_PLATFORM").contains("offscreen") ||
                     app.arguments().contains("--help") ||
                     app.arguments().contains("--test");
    
    // 处理命令行参数
    if (app.arguments().contains("--help")) {
        qDebug() << "现代音乐播放器 v2.0.0";
        qDebug() << "用法: musicplayer [选项]";
        qDebug() << "选项:";
        qDebug() << "  --help     显示此帮助信息";
        qDebug() << "  --test     运行测试模式";
        qDebug() << "  --version  显示版本信息";
        return 0;
    }
    
    if (app.arguments().contains("--version")) {
        qDebug() << "现代音乐播放器 版本 2.0.0";
        return 0;
    }
    
    if (app.arguments().contains("--test")) {
        qDebug() << "运行测试模式...";
        // 简单的测试：创建播放器窗口但不显示
        PlayerWindow *window = new PlayerWindow();
        Q_UNUSED(window)
        qDebug() << "测试完成 - 播放器窗口创建成功";
        return 0;
    }
    
    QSplashScreen *splash = nullptr;
    
    if (!isHeadless) {
        // 只在非CI环境显示启动画面
        splash = new QSplashScreen(createSplashScreen());
        splash->show();
        app.processEvents();
    }
    
    // 初始化播放器窗口
    PlayerWindow *window = new PlayerWindow();
    
    // 初始化播放列表管理器
    PlaylistManager manager;
    
    // 创建默认音乐目录
    QString musicDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString appMusicDir = musicDir + "/MyMusicPlayer";
    QDir dir;
    if (!dir.exists(appMusicDir)) {
        dir.mkpath(appMusicDir);
    }
    
    // 扫描音乐文件
    manager.scanMusicFolders(musicDir, appMusicDir);
    manager.loadPlaylists(appMusicDir);
    
    if (isHeadless) {
        // CI环境：立即显示窗口（但可能不可见）
        window->show();
        qDebug() << "音乐播放器已启动（无头模式）";
    } else {
        // 正常环境：模拟加载进度
        QTimer::singleShot(2000, [splash, window]() {
            if (splash) {
                splash->finish(window);
                splash->deleteLater();
            }
            window->show();
            
            // 显示欢迎信息
            QMessageBox::information(window, "欢迎", 
                "欢迎使用现代音乐播放器！\n\n"
                "新功能：\n"
                "• 现代化界面设计\n"
                "• 支持拖放文件导入\n"
                "• 音频均衡器\n"
                "• 歌词同步显示\n"
                "• 多种播放模式\n"
                "• 主题切换");
        });
    }
    
    return app.exec();
}
