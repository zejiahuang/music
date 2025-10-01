#include "playerwindow.h"
// ...其他包含

void PlayerWindow::setupUi() {
    // ...其他UI布局

    lyricsVisualWidget = new LyricsVisualWidget(this);

    // 中间区
    QVBoxLayout *centerLayout = new QVBoxLayout;
    centerLayout->addWidget(searchEdit);
    centerLayout->addWidget(playlistWidget, 1);
    centerLayout->addWidget(lyricsVisualWidget, 2); // 集成歌词可视化控件

    // ...主布局
}

void PlayerWindow::setupStyle() {
    lyricsVisualWidget->setDynamicTheme(QColor(245， 247, 250)， QColor(103， 80， 164), QColor(33, 33, 33));
    lyricsVisualWidget->setFontStyle(QFont("Microsoft YaHei", QFont::Bold), 28);
}

void PlayerWindow::loadSong(const QString &audioPath) {
    QString lrcPath = QFileInfo(audioPath).absolutePath() + "/" + QFileInfo(audioPath).completeBaseName() + ".lrc";
    lyricsVisualWidget->loadLrc(lrcPath);

    QVector<float> waveform = extractWaveformFFmpeg(audioPath, 256);
    lyricsVisualWidget->setAudioWaveform(waveform);

    connect(player, &FFmpegPlayer::positionChanged, lyricsVisualWidget, &LyricsVisualWidget::updatePosition);
}

void PlayerWindow::switchTheme(bool dark) {
    if (dark)
        lyricsVisualWidget->setDynamicTheme(QColor(33, 33, 33), QColor(220, 190, 255), QColor(180, 180, 180));
    else
        lyricsVisualWidget->setDynamicTheme(QColor(245, 247, 250), QColor(103, 80, 164), QColor(33, 33, 33));
}