#include "playerwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include "ffmpeg_waveform.h"
#include "taglib_utils.h"

// 添加必要的包含头文件
PlayerWindow::PlayerWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    setupStyle();
}

void PlayerWindow::setupUi() {
    // 添加布局和控件初始化代码
    QVBoxLayout *centerLayout = new QVBoxLayout;
    centerLayout->addWidget(searchEdit);
    centerLayout->addWidget(playlistWidget, 1);
    centerLayout->addWidget(lyricsVisualWidget, 2);
    
    // 修复函数调用，使用正确的参数
    lyricsVisualWidget->setDynamicTheme(QColor(245, 247, 250), QColor(103, 80, 164), QColor(33, 33, 33));
}

void PlayerWindow::loadSong(const QString &audioPath) {
    QString lrcPath = QFileInfo(audioPath).absolutePath() + "/" + QFileInfo(audioPath).completeBaseName() + ".lrc";
    lyricsVisualWidget->loadLrc(lrcPath);
    
    QVector<float> waveform = extractWaveformFFmpeg(audioPath, 256);
    lyricsVisualWidget->setAudioWaveform(waveform);
    
    connect(player, &FFmpegPlayer::positionChanged, lyricsVisualWidget, &LyricsVisualWidget::updatePosition);
}
