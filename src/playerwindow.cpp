#include "playerwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include "ffmpeg_waveform.h"
#include "taglib_utils.h"

// 添加必要的包含头文件
PlayerWindow::PlayerWindow(QWidget *parent) : QMainWindow(parent) {
    // 初始化成员变量
    lyricsVisualWidget = new LyricsVisualWidget(this);
    searchEdit = new QLineEdit(this);
    playlistWidget = new QListWidget(this);
    player = new FFmpegPlayer(this);
    
    setupUi();
    setupStyle();
}

void PlayerWindow::setupUi() {
    // 创建中央控件和布局
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *centerLayout = new QVBoxLayout(centralWidget);
    
    // 设置搜索框提示文字
    searchEdit->setPlaceholderText("搜索音乐...");
    
    // 添加控件到布局
    centerLayout->addWidget(searchEdit);
    centerLayout->addWidget(playlistWidget, 1);
    centerLayout->addWidget(lyricsVisualWidget, 2);
    
    // 设置窗口属性
    setWindowTitle("音乐播放器");
    resize(800, 600);
    
    // 修复函数调用，使用正确的参数
    lyricsVisualWidget->setDynamicTheme(QColor(245, 247, 250), QColor(103, 80, 164), QColor(33, 33, 33));
}

void PlayerWindow::setupStyle() {
    // 设置窗口样式
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #f5f7fa;"
        "    color: #333333;"
        "}"
        "QListWidget {"
        "    background-color: white;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "}"
        "QLineEdit {"
        "    padding: 8px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    background-color: white;"
        "}"
    );
}

void PlayerWindow::loadSong(const QString &audioPath) {
    QString lrcPath = QFileInfo(audioPath).absolutePath() + "/" + QFileInfo(audioPath).completeBaseName() + ".lrc";
    lyricsVisualWidget->loadLrc(lrcPath);
    
    QVector<float> waveform = extractWaveformFFmpeg(audioPath, 256);
    lyricsVisualWidget->setAudioWaveform(waveform);
    
    connect(player, &FFmpegPlayer::positionChanged, lyricsVisualWidget, &LyricsVisualWidget::updatePosition);
}

void PlayerWindow::switchTheme(bool dark) {
    if (dark) {
        setStyleSheet(
            "QMainWindow {"
            "    background-color: #2b2b2b;"
            "    color: #ffffff;"
            "}"
            "QListWidget {"
            "    background-color: #3c3c3c;"
            "    border: 1px solid #555555;"
            "    border-radius: 4px;"
            "    color: #ffffff;"
            "}"
            "QLineEdit {"
            "    padding: 8px;"
            "    border: 1px solid #555555;"
            "    border-radius: 4px;"
            "    background-color: #3c3c3c;"
            "    color: #ffffff;"
            "}"
        );
    } else {
        setupStyle(); // 重新应用默认的浅色主题
    }
}
