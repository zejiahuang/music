#include "lyricsvisualwidget.h"
#include <QPainter>
#include <QFile>
#include <QRegExp>
#include <QLinearGradient>
#include <cmath>

// 修复函数实现，移除非法字符
void LyricsVisualWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 绘制背景
    painter.fillRect(rect(), bgColor);
    
    if (lyrics.isEmpty()) {
        painter.setFont(lyricsFont);
        painter.setPen(normalColor);
        painter.drawText(rect(), Qt::AlignCenter, "No lyrics available");
        return;
    }
    
    // 绘制歌词
    int centerY = height() / 2;
    int lineHeight = fontSize + 16;
    int start = qMax(currentIndex - 2, 0);
    int end = qMin(currentIndex + 2, lyrics.size() - 1);
    
    for (int i = start; i <= end; i++) {
        QRect lineRect(0, centerY + (i - currentIndex) * lineHeight - lineHeight / 2，
                       width(), lineHeight);
        
        QFont f = lyricsFont;
        QColor color = (i == currentIndex) ? highlightColor : normalColor;
        
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
        painter.drawText(lineRect, Qt::AlignCenter, txt);
    }
    
    // 绘制波形
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
