#pragma once
#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>
#include <QGraphicsEffect>
#include <QTimer>
#include <QEasingCurve>
#include <QColor>
#include <QFont>

/**
 * Material Design 风格的按钮组件
 * 支持涟漪效果、阴影动画、状态变化
 */
class MaterialButton : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal rippleRadius READ rippleRadius WRITE setRippleRadius)
    Q_PROPERTY(qreal shadowElevation READ shadowElevation WRITE setShadowElevation)

public:
    enum ButtonType {
        Raised,      // 凸起按钮
        Flat,        // 扁平按钮
        Floating,    // 浮动操作按钮
        Outlined     // 轮廓按钮
    };

    explicit MaterialButton(const QString &text = "", ButtonType type = Raised, QWidget *parent = nullptr);
    
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    void setButtonType(ButtonType type);
    void setAccentColor(const QColor &color);
    void setEnabled(bool enabled) override;
    
    qreal rippleRadius() const { return m_rippleRadius; }
    void setRippleRadius(qreal radius);
    
    qreal shadowElevation() const { return m_shadowElevation; }
    void setShadowElevation(qreal elevation);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    
private slots:
    void onRippleAnimation();
    
private:
    void startRippleEffect(const QPoint &center);
    void updateShadow();
    void paintRaisedButton(QPainter &painter);
    void paintFlatButton(QPainter &painter);
    void paintFloatingButton(QPainter &painter);
    void paintOutlinedButton(QPainter &painter);
    
    QString m_text;
    QIcon m_icon;
    ButtonType m_buttonType;
    QColor m_accentColor;
    QColor m_backgroundColor;
    QColor m_textColor;
    
    // 涟漪效果
    QPropertyAnimation *m_rippleAnimation;
    QTimer *m_rippleTimer;
    qreal m_rippleRadius;
    QPoint m_rippleCenter;
    bool m_rippleActive;
    
    // 阴影效果
    QPropertyAnimation *m_shadowAnimation;
    qreal m_shadowElevation;
    QGraphicsDropShadowEffect *m_shadowEffect;
    
    // 状态
    bool m_hovered;
    bool m_pressed;
};

/**
 * Material Design 风格的卡片组件
 * 支持阴影、圆角、动画效果
 */
class MaterialCard : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal elevation READ elevation WRITE setElevation)
    Q_PROPERTY(qreal cornerRadius READ cornerRadius WRITE setCornerRadius)

public:
    explicit MaterialCard(QWidget *parent = nullptr);
    
    void setElevation(qreal elevation);
    qreal elevation() const { return m_elevation; }
    
    void setCornerRadius(qreal radius);
    qreal cornerRadius() const { return m_cornerRadius; }
    
    void setCardColor(const QColor &color);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    
private:
    void updateShadow();
    
    qreal m_elevation;
    qreal m_cornerRadius;
    QColor m_cardColor;
    QPropertyAnimation *m_elevationAnimation;
    QGraphicsDropShadowEffect *m_shadowEffect;
};

/**
 * 高级进度条组件
 * 支持缓冲进度、动画、Material Design 风格
 */
class AdvancedProgressBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal progress READ progress WRITE setProgress)
    Q_PROPERTY(qreal bufferProgress READ bufferProgress WRITE setBufferProgress)

public:
    explicit AdvancedProgressBar(QWidget *parent = nullptr);
    
    void setProgress(qreal progress);
    qreal progress() const { return m_progress; }
    
    void setBufferProgress(qreal progress);
    qreal bufferProgress() const { return m_bufferProgress; }
    
    void setAccentColor(const QColor &color);
    void setTrackColor(const QColor &color);
    void setBufferColor(const QColor &color);
    
    void setAnimationEnabled(bool enabled);

signals:
    void progressChanged(qreal progress);
    void clicked(qreal position);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
private:
    void updateProgress(qreal newProgress);
    
    qreal m_progress;
    qreal m_bufferProgress;
    qreal m_animatedProgress;
    
    QColor m_accentColor;
    QColor m_trackColor;
    QColor m_bufferColor;
    
    QPropertyAnimation *m_progressAnimation;
    bool m_animationEnabled;
    bool m_dragging;
};

/**
 * 波纹效果控件
 * 可以应用到任何QWidget上
 */
class RippleEffect : public QObject {
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit RippleEffect(QWidget *parent);
    
    void setRadius(qreal radius);
    qreal radius() const { return m_radius; }
    
    void setOpacity(qreal opacity);
    qreal opacity() const { return m_opacity; }
    
    void startRipple(const QPoint &center, const QColor &color = QColor());
    
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    
private slots:
    void onAnimationFinished();
    
private:
    QWidget *m_widget;
    QPropertyAnimation *m_radiusAnimation;
    QPropertyAnimation *m_opacityAnimation;
    
    qreal m_radius;
    qreal m_opacity;
    QPoint m_center;
    QColor m_color;
    bool m_active;
};

/**
 * 智能提示组件
 * Material Design 风格的工具提示
 */
class SmartTooltip : public QWidget {
    Q_OBJECT

public:
    explicit SmartTooltip(QWidget *parent = nullptr);
    
    void showTooltip(const QString &text, const QPoint &position, int duration = 2000);
    void hideTooltip();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private slots:
    void onHideAnimation();
    
private:
    QString m_text;
    QPropertyAnimation *m_showAnimation;
    QPropertyAnimation *m_hideAnimation;
    QTimer *m_hideTimer;
};
