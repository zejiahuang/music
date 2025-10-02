#include "lyricsvisualwidget.h"
#include <QPainter>
#include <QFile>
#include <QRegExp>
#include <QLinearGradient>
#include <cmath>

LyricsVisualWidget::LyricsVisualWidget(QWidget *parent)
    : QWidget(parent), animationTimer(new QTimer(this))
{
    setMinimumHeight(180);
    setAutoFillBackground(true);
    lyricsFont = QFont("Microsoft YaHei", fontSize, QFont::Bold);

    connect(animationTimer, &QTimer::timeout, this, [this]() {
        highlightProgress += 0.08f;
        if (highlightProgress >= 1.0f) {
            highlightProgress = 1.0f;
            animationTimer->stop();
        }
        update();
    });
}

bool LyricsVisualWidget::loadLrc(const QString &lrcFile) {
    lyrics.clear();
    QFile file(lrcFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    while (!file.atEnd()) {
        QString line = file.readLine();
        QRegExp rx("\\[(\\d+):(\\d+\\.\\d+)\\](.*)");
        if (rx.indexIn(line) != -1) {
            int min = rx.cap(1).toInt();
            float sec = rx.cap(2).toFloat();
            int ms = (min * 60 + sec) * 1000;
            QString lyric = rx.cap(3).trimmed();
            lyrics.append({ms, lyric});
        }
    }
    update();
    return !lyrics.isEmpty();
}

void LyricsVisualWidget::setAudioWaveform(const QVector<float> &wave) {
    audioWaveform = wave;
    update();
}

void LyricsVisualWidget::setDynamicTheme(const QColor &bg, const QColor &highlight, const QColor &normal) {
    bgColor = bg;
    highlightColor = highlight;
    normalColor = normal;
    update();
}

void LyricsVisualWidget::setFontStyle(const QFont &font, int size) {
    lyricsFont = font;
    fontSize = size;
    update();
}

void LyricsVisualWidget::updatePosition(qint64 ms) {
    if (lyrics.isEmpty()) return;
    int idx = 0;
    for (int i = 0; i < lyrics.size(); ++i) {
        if (ms < lyrics[i].timeMs) {
            idx = qMax(i - 1, 0);
            break;
        }
        idx = i;
    }
    if (currentIndex != idx) {
        currentIndex = idx;
        highlightProgress = 0.0f;
        startHighlightAnim();
    }
    update();
}

void LyricsVisualWidget::startHighlightAnim() {
    animationTimer->start(30);
}

void LyricsVisualWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 背景
    painter.fillRect(rect(), bgColor);

    // 歌词显示
    if (lyrics.isEmpty()) {
        painter.setFont(lyricsFont);
        painter.setPen(normalColor);
        painter.drawText(rect(), Qt::AlignCenter, "No lyrics available");
        return;
    }

    int centerY = height() / 2;
    int lineHeight = fontSize + 16;
    int start = qMax(currentIndex - 2, 0);
    int end = qMin(currentIndex + 2, lyrics.size() - 1);

    for (int i = start; i <= end; i++) {
        QRect lineRect(0, centerY + (i - currentIndex) * lineHeight - lineHeight / 2,
                       width(), lineHeight);

        QFont f = lyricsFont;
        QColor color = (i == currentIndex)
            ? highlightColor
            : normalColor;

        if (i == currentIndex) {
            f.setPointSize(fontSize + 6);
            painter.setFont(f);

            // 渐变高亮
            QLinearGradient grad(lineRect.topLeft(), lineRect.bottomLeft());
            grad.setColorAt(0, color.lighter(120));
            grad.setColorAt(1, color.darker(120));
            painter.setPen(QPen(QBrush(grad), 0));
        } else {
            f.setPointSize(fontSize);
            painter.setFont(f);
            painter.setPen(color);
        }

        QString txt = lyrics[i].text;
        if (i == currentIndex && highlightProgress < 1.0f) {
            int charCount = std::ceil(txt.size() * highlightProgress);
            QString left = txt.left(charCount);
            QString right = txt.mid(charCount);
            
            // 计算文本宽度以保持居中
            QFontMetrics fm(f);
            int textWidth = fm.horizontalAdvance(txt);
            int leftWidth = fm.horizontalAdvance(left);
            
            painter.save();
            painter.translate((width() - textWidth) / 2, 0);
            
            painter.setPen(highlightColor);
            painter.drawText(lineRect, Qt::AlignLeft | Qt::AlignVCenter, left);
            
            painter.setPen(normalColor);
            painter.drawText(lineRect.adjusted(leftWidth, 0， 0, 0), Qt::AlignLeft | Qt::AlignVCenter, right);
            
            painter.restore();
        } else {
            painter.drawText(lineRect, Qt::AlignCenter, txt);
        }
    }

    // 波形可视化
    if (!audioWaveform.isEmpty()) {
        int w = width(), h = height();
        int baseY = h - 40;
        int waveH = 32;
        painter.setPen(QColor(103, 80, 164, 128));
        for (int i = 0; i < audioWaveform.size(); i++) {
            int x = i * w / audioWaveform.size();
            int y = baseY - audioWaveform[i] * waveH;
            painter.drawLine(x, baseY, x, y);
        }
    }
}

void LyricsVisualWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    update();
}
