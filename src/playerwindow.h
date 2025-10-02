#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include "ui/lyricsvisualwidget.h"

class PlayerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = nullptr);

private:
    LyricsVisualWidget *lyricsVisualWidget;
    // ...其他成员
    void setupUi();
    void setupStyle();
    void loadSong(const QString &audioPath);
    void switchTheme(bool dark);
};
