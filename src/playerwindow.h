#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include "ui/lyricsvisualwidget.h"
#include "ffmpegplayer.h"

class PlayerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = nullptr);

private:
    LyricsVisualWidget *lyricsVisualWidget;
    QLineEdit *searchEdit;
    QListWidget *playlistWidget;
    FFmpegPlayer *player;
    
    void setupUi();
    void setupStyle();
    void loadSong(const QString &audioPath);
    void switchTheme(bool dark);
};
