#include "../include/materialui_components.h"
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QFontMetrics>
#include <QPainterPath>
#include <QTimer>
#include <cmath>

// MaterialButton Implementation
MaterialButton::MaterialButton(const QString &text, ButtonType type, QWidget *parent)
    : QWidget(parent)
    , m_text(text)
    , m_buttonType(type)
    , m_accentColor(QColor(103, 80, 164))
    , m_backgroundColor(QColor(255, 255, 255))
    , m_textColor(QColor(33, 33, 33))
    , m_rippleRadius(0)
    , m_rippleActive(false)
    , m_shadowElevation(2)
    , m_hovered(false)
    , m_pressed(false)
{
    setAttribute(Qt::WA_Hover);
    setMinimumSize(88, 36);
    setCursor(Qt::PointingHandCursor);
    
    // 初始化动画
    m_rippleAnimation = new QPropertyAnimation(this, "rippleRadius");
    m_rippleAnimation->setDuration(300);
    m_rippleAnimation->setEasingCurve(QEasingCurve::OutQuad);
    connect(m_rippleAnimation, &QPropertyAnimation::valueChanged, this, QOverload<>::of(&QWidget::update));
    connect(m_rippleAnimation, &QPropertyAnimation::finished, [this]() { m_rippleActive = false; update(); });
    
    m_shadowAnimation = new QPropertyAnimation(this, "shadowElevation");
    m_shadowAnimation->setDuration(150);
    m_shadowAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 设置阴影效果
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    updateShadow();
    setGraphicsEffect(m_shadowEffect);
}

void MaterialButton::setText(const QString &text) {
    m_text = text;
    update();
}

void MaterialButton::setIcon(const QIcon &icon) {
    m_icon = icon;
    update();
}

void MaterialButton::setButtonType(ButtonType type) {
    m_buttonType = type;
    updateShadow();
    update();
}

void MaterialButton::setAccentColor(const QColor &color) {
    m_accentColor = color;
    update();
}

void MaterialButton::setRippleRadius(qreal radius) {
    m_rippleRadius = radius;
    update();
}

void MaterialButton::setShadowElevation(qreal elevation) {
    m_shadowElevation = elevation;
    updateShadow();
}

void MaterialButton::setEnabled(bool enabled) {
    QWidget::setEnabled(enabled);
    update();
}

void MaterialButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    switch (m_buttonType) {
        case Raised:
            paintRaisedButton(painter);
            break;
        case Flat:
            paintFlatButton(painter);
            break;
        case Floating:
            paintFloatingButton(painter);
            break;
        case Outlined:
            paintOutlinedButton(painter);
            break;
    }
    
    // 绘制涟漪效果
    if (m_rippleActive) {
        painter.save();
        painter.setClipRect(rect());
        QColor rippleColor = m_accentColor;
        rippleColor.setAlpha(50);
        painter.setBrush(rippleColor);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QRectF(m_rippleCenter.x() - m_rippleRadius/2, 
                                   m_rippleCenter.y() - m_rippleRadius/2, 
                                   m_rippleRadius, m_rippleRadius));
        painter.restore();
    }
}

void MaterialButton::paintRaisedButton(QPainter &painter) {
    QRect buttonRect = rect().adjusted(2, 2, -2, -2);
    
    // 背景色根据状态调整
    QColor bgColor = m_backgroundColor;
    if (m_pressed) {
        bgColor = bgColor.darker(110);
    } else if (m_hovered) {
        bgColor = bgColor.lighter(105);
    }
    
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(buttonRect, 4, 4);
    
    // 绘制文本
    painter.setPen(m_textColor);
    painter.setFont(QFont("Microsoft YaHei", 10, QFont::Medium));
    painter.drawText(buttonRect, Qt::AlignCenter, m_text);
}

void MaterialButton::paintFlatButton(QPainter &painter) {
    QRect buttonRect = rect();
    
    // 只有hover或pressed状态才绘制背景
    if (m_hovered || m_pressed) {
        QColor bgColor = m_accentColor;
        bgColor.setAlpha(m_pressed ? 30 : 15);
        painter.setBrush(bgColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(buttonRect, 4, 4);
    }
    
    // 绘制文本
    painter.setPen(m_accentColor);
    painter.setFont(QFont("Microsoft YaHei", 10, QFont::Medium));
    painter.drawText(buttonRect, Qt::AlignCenter, m_text);
}

void MaterialButton::paintFloatingButton(QPainter &painter) {
    QRect buttonRect = rect().adjusted(4, 4, -4, -4);
    
    // 绘制圆形背景
    QColor bgColor = m_accentColor;
    if (m_pressed) {
        bgColor = bgColor.darker(110);
    } else if (m_hovered) {
        bgColor = bgColor.lighter(110);
    }
    
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(buttonRect);
    
    // 绘制图标或文本
    painter.setPen(Qt::white);
    if (!m_icon.isNull()) {
        QPixmap pixmap = m_icon.pixmap(24, 24);
        QRect iconRect = QRect(buttonRect.center() - QPoint(12, 12), QSize(24, 24));
        painter.drawPixmap(iconRect, pixmap);
    } else {
        painter.setFont(QFont("Microsoft YaHei", 14, QFont::Bold));
        painter.drawText(buttonRect, Qt::AlignCenter, m_text);
    }
}

void MaterialButton::paintOutlinedButton(QPainter &painter) {
    QRect buttonRect = rect().adjusted(1, 1, -1, -1);
    
    // 绘制边框
    QPen pen(m_accentColor, 1);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    
    if (m_hovered || m_pressed) {
        QColor bgColor = m_accentColor;
        bgColor.setAlpha(m_pressed ? 20 : 10);
        painter.setBrush(bgColor);
    }
    
    painter.drawRoundedRect(buttonRect, 4, 4);
    
    // 绘制文本
    painter.setPen(m_accentColor);
    painter.setFont(QFont("Microsoft YaHei", 10, QFont::Medium));
    painter.drawText(buttonRect, Qt::AlignCenter, m_text);
}

void MaterialButton::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_pressed = true;
        startRippleEffect(event->pos());
        update();
    }
    QWidget::mousePressEvent(event);
}

void MaterialButton::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && m_pressed) {
        m_pressed = false;
        emit clicked();
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void MaterialButton::enterEvent(QEvent *event) {
    m_hovered = true;
    if (m_buttonType == Raised || m_buttonType == Floating) {
        m_shadowAnimation->setStartValue(m_shadowElevation);
        m_shadowAnimation->setEndValue(m_shadowElevation + 2);
        m_shadowAnimation->start();
    }
    update();
    QWidget::enterEvent(event);
}

void MaterialButton::leaveEvent(QEvent *event) {
    m_hovered = false;
    m_pressed = false;
    if (m_buttonType == Raised || m_buttonType == Floating) {
        m_shadowAnimation->setStartValue(m_shadowElevation);
        m_shadowAnimation->setEndValue(m_shadowElevation - 2);
        m_shadowAnimation->start();
    }
    update();
    QWidget::leaveEvent(event);
}

void MaterialButton::startRippleEffect(const QPoint &center) {
    m_rippleCenter = center;
    m_rippleActive = true;
    
    qreal maxRadius = qMax(width(), height()) * 0.8;
    m_rippleAnimation->setStartValue(0);
    m_rippleAnimation->setEndValue(maxRadius);
    m_rippleAnimation->start();
}

void MaterialButton::updateShadow() {
    if (m_shadowEffect) {
        m_shadowEffect->setBlurRadius(m_shadowElevation * 2);
        m_shadowEffect->setOffset(0, m_shadowElevation);
        m_shadowEffect->setColor(QColor(0, 0, 0, 40));
    }
}

// 声明为槽的更新函数，供 Qt MOC 引用
void MaterialButton::onRippleAnimation() {
    update();
}

// MaterialCard Implementation
MaterialCard::MaterialCard(QWidget *parent)
    : QWidget(parent)
    , m_elevation(2)
    , m_cornerRadius(8)
    , m_cardColor(Qt::white)
{
    setAttribute(Qt::WA_Hover);
    
    m_elevationAnimation = new QPropertyAnimation(this, "elevation");
    m_elevationAnimation->setDuration(150);
    m_elevationAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    m_shadowEffect = new QGraphicsDropShadowEffect(this);
    updateShadow();
    setGraphicsEffect(m_shadowEffect);
}

void MaterialCard::setElevation(qreal elevation) {
    m_elevation = elevation;
    updateShadow();
}

void MaterialCard::setCornerRadius(qreal radius) {
    m_cornerRadius = radius;
    update();
}

void MaterialCard::setCardColor(const QColor &color) {
    m_cardColor = color;
    update();
}

void MaterialCard::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect cardRect = rect().adjusted(2, 2, -2, -2);
    painter.setBrush(m_cardColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(cardRect, m_cornerRadius, m_cornerRadius);
}

void MaterialCard::enterEvent(QEvent *event) {
    m_elevationAnimation->setStartValue(m_elevation);
    m_elevationAnimation->setEndValue(m_elevation + 2);
    m_elevationAnimation->start();
    QWidget::enterEvent(event);
}

void MaterialCard::leaveEvent(QEvent *event) {
    m_elevationAnimation->setStartValue(m_elevation);
    m_elevationAnimation->setEndValue(m_elevation - 2);
    m_elevationAnimation->start();
    QWidget::leaveEvent(event);
}

void MaterialCard::updateShadow() {
    if (m_shadowEffect) {
        m_shadowEffect->setBlurRadius(m_elevation * 3);
        m_shadowEffect->setOffset(0, m_elevation);
        m_shadowEffect->setColor(QColor(0, 0, 0, 30));
    }
}

// RippleEffect Implementation
RippleEffect::RippleEffect(QWidget *parent)
    : QObject(parent)
    , m_widget(parent)
    , m_radiusAnimation(new QPropertyAnimation(this, "radius"))
    , m_opacityAnimation(new QPropertyAnimation(this, "opacity"))
    , m_radius(0)
    , m_opacity(0)
    , m_active(false)
{
    if (m_widget) {
        m_widget->installEventFilter(this);
    }

    m_radiusAnimation->setDuration(300);
    m_radiusAnimation->setEasingCurve(QEasingCurve::OutQuad);

    m_opacityAnimation->setDuration(300);
    m_opacityAnimation->setEasingCurve(QEasingCurve::OutQuad);

    connect(m_radiusAnimation, &QPropertyAnimation::valueChanged, [this]() {
        if (m_widget) m_widget->update();
    });
    connect(m_opacityAnimation, &QPropertyAnimation::valueChanged, [this]() {
        if (m_widget) m_widget->update();
    });
    connect(m_opacityAnimation, &QPropertyAnimation::finished, this, &RippleEffect::onAnimationFinished);
}

void RippleEffect::setRadius(qreal radius) {
    m_radius = radius;
    if (m_widget) m_widget->update();
}

void RippleEffect::setOpacity(qreal opacity) {
    m_opacity = opacity;
    if (m_widget) m_widget->update();
}

void RippleEffect::startRipple(const QPoint &center, const QColor &color) {
    m_center = center;
    m_color = color.isValid() ? color : QColor(0, 0, 0, 60);
    m_active = true;

    const int maxDim = m_widget ? qMax(m_widget->width(), m_widget->height()) : 0;
    m_radiusAnimation->stop();
    m_radiusAnimation->setStartValue(0);
    m_radiusAnimation->setEndValue(maxDim * 0.8);

    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(0.4);
    m_opacityAnimation->setEndValue(0.0);

    m_radiusAnimation->start();
    m_opacityAnimation->start();
}

bool RippleEffect::eventFilter(QObject *obj, QEvent *event) {
    if (!m_active || obj != m_widget)
        return QObject::eventFilter(obj, event);

    if (event->type() == QEvent::Paint && m_widget) {
        QPainter painter(m_widget);
        painter.setRenderHint(QPainter::Antialiasing);
        QColor c = m_color;
        c.setAlphaF(qBound(0.0, m_opacity, 1.0));
        painter.setBrush(c);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(QRectF(m_center.x() - m_radius / 2.0,
                                   m_center.y() - m_radius / 2.0,
                                   m_radius, m_radius));
    }
    return QObject::eventFilter(obj, event);
}

void RippleEffect::onAnimationFinished() {
    m_active = false;
    if (m_widget) m_widget->update();
}

// SmartTooltip minimal Implementation
SmartTooltip::SmartTooltip(QWidget *parent)
    : QWidget(parent)
    , m_showAnimation(new QPropertyAnimation(this, "windowOpacity"))
    , m_hideAnimation(new QPropertyAnimation(this, "windowOpacity"))
    , m_hideTimer(new QTimer(this))
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
    setWindowOpacity(0.0);

    m_showAnimation->setDuration(150);
    m_showAnimation->setStartValue(0.0);
    m_showAnimation->setEndValue(1.0);

    m_hideAnimation->setDuration(150);
    m_hideAnimation->setStartValue(1.0);
    m_hideAnimation->setEndValue(0.0);

    connect(m_hideAnimation, &QPropertyAnimation::finished, this, &SmartTooltip::onHideAnimation);
    connect(m_hideTimer, &QTimer::timeout, [this]() { m_hideAnimation->start(); });
}

void SmartTooltip::onHideAnimation() {
    hide();
}

void SmartTooltip::showTooltip(const QString &text, const QPoint &position, int duration) {
    m_text = text;
    adjustSize();
    move(position);
    show();
    raise();
    m_hideTimer->stop();
    m_showAnimation->start();
    if (duration > 0) {
        m_hideTimer->start(duration);
    }
}

void SmartTooltip::hideTooltip() {
    m_hideTimer->stop();
    m_hideAnimation->start();
}

void SmartTooltip::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 动态计算尺寸以适配文本
    QFont f("Microsoft YaHei", 9);
    painter.setFont(f);
    QFontMetrics fm(f);
    const int paddingH = 10;
    const int paddingV = 6;
    QRect textRect = fm.boundingRect(m_text);
    textRect.adjust(-paddingH, -paddingV, paddingH, paddingV);
    resize(textRect.size());

    // 背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(50, 50, 50, 230));
    painter.drawRoundedRect(rect(), 6, 6);

    // 文本
    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, m_text);
}

// AdvancedProgressBar Implementation
AdvancedProgressBar::AdvancedProgressBar(QWidget *parent)
    : QWidget(parent)
    , m_progress(0)
    , m_bufferProgress(0)
    , m_animatedProgress(0)
    , m_accentColor(QColor(103, 80, 164))
    , m_trackColor(QColor(224, 224, 224))
    , m_bufferColor(QColor(187, 187, 187))
    , m_animationEnabled(true)
    , m_dragging(false)
{
    setMinimumHeight(4);
    setCursor(Qt::PointingHandCursor);
    
    m_progressAnimation = new QPropertyAnimation(this, "progress");
    m_progressAnimation->setDuration(300);
    m_progressAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void AdvancedProgressBar::setProgress(qreal progress) {
    progress = qBound(0.0, progress, 1.0);
    
    if (m_animationEnabled && !m_dragging) {
        m_progressAnimation->setStartValue(m_progress);
        m_progressAnimation->setEndValue(progress);
        m_progressAnimation->start();
    } else {
        m_progress = progress;
        update();
    }
}

void AdvancedProgressBar::setBufferProgress(qreal progress) {
    m_bufferProgress = qBound(0.0, progress, 1.0);
    update();
}

void AdvancedProgressBar::setAccentColor(const QColor &color) {
    m_accentColor = color;
    update();
}

void AdvancedProgressBar::setTrackColor(const QColor &color) {
    m_trackColor = color;
    update();
}

void AdvancedProgressBar::setBufferColor(const QColor &color) {
    m_bufferColor = color;
    update();
}

void AdvancedProgressBar::setAnimationEnabled(bool enabled) {
    m_animationEnabled = enabled;
}

void AdvancedProgressBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    QRect trackRect = rect();
    int trackHeight = 4;
    trackRect.setY((height() - trackHeight) / 2);
    trackRect.setHeight(trackHeight);
    
    // 绘制轨道
    painter.setBrush(m_trackColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(trackRect, trackHeight / 2, trackHeight / 2);
    
    // 绘制缓冲进度
    if (m_bufferProgress > 0) {
        QRect bufferRect = trackRect;
        bufferRect.setWidth(trackRect.width() * m_bufferProgress);
        painter.setBrush(m_bufferColor);
        painter.drawRoundedRect(bufferRect, trackHeight / 2, trackHeight / 2);
    }
    
    // 绘制主进度
    if (m_progress > 0) {
        QRect progressRect = trackRect;
        progressRect.setWidth(trackRect.width() * m_progress);
        painter.setBrush(m_accentColor);
        painter.drawRoundedRect(progressRect, trackHeight / 2, trackHeight / 2);
    }
    
    // 绘制进度点（拖拽时显示）
    if (m_dragging) {
        int thumbX = trackRect.x() + trackRect.width() * m_progress;
        int thumbY = trackRect.center().y();
        painter.setBrush(m_accentColor);
        painter.drawEllipse(QPoint(thumbX, thumbY), 8, 8);
    }
}

void AdvancedProgressBar::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        qreal position = qBound(0.0, (double)event->x() / width(), 1.0);
        m_progress = position;
        emit progressChanged(position);
        emit clicked(position);
        update();
    }
    QWidget::mousePressEvent(event);
}

void AdvancedProgressBar::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging) {
        qreal position = qBound(0.0, (double)event->x() / width(), 1.0);
        m_progress = position;
        emit progressChanged(position);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void AdvancedProgressBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        update();
    }
    QWidget::mouseReleaseEvent(event);
}
