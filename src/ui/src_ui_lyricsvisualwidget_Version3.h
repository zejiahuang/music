#pragma once
#include <QWidget>
#include <QVector>
#include <QColor>
#include <QFont>
#include <QTimer>

class LyricsVisualWidget : public QWidget {
    Q_OBJECT
public:
    explicit LyricsVisualWidget(QWidget *parent = nullptr);

    bool loadLrc(const QString &lrcFile);
    void setAudioWaveform(const QVector<float> &wave);
    void setDynamicTheme(const QColor &bg, const QColor &highlight, const QColor &normal);
    void updatePosition(qint64 ms);
    void setFontStyle(const QFont &font, int size);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct LyricLine {
        int timeMs;
        QString text;
    };
    QVector<LyricLine> lyrics;
    QVector<float> audioWaveform;
    int currentIndex = 0;
    QFont lyricsFont;
    int fontSize = 24;
    QColor bgColor = QColor(245, 247, 250);
    QColor highlightColor = QColor(103, 80, 164);
    QColor normalColor = QColor(33, 33, 33);
    QTimer *animationTimer = nullptr;
    float highlightProgress = 1.0;
    void startHighlightAnim();
};