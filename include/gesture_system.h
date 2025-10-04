#pragma once
#include <QObject>
#include <QWidget>
#include <QGestureEvent>
#include <QTapGesture>
#include <QPanGesture>
#include <QPinchGesture>
#include <QSwipeGesture>
#include <QShortcut>
#include <QKeySequence>
#include <QAction>
#include <QMap>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QJsonObject>

/**
 * 高级手势识别器
 * 支持多种复杂手势的识别和处理
 */
class AdvancedGestureRecognizer : public QObject {
    Q_OBJECT

public:
    enum GestureType {
        // 基础手势
        Tap,                // 单击
        DoubleTap,          // 双击
        LongPress,          // 长按
        Pan,                // 拖拽
        Pinch,              // 缩放
        Swipe,              // 滑动
        
        // 自定义手势
        CircleClockwise,    // 顺时针画圆
        CircleCounterwise,  // 逆时针画圆
        ZigZag,             // 之字形
        Heart,              // 心形
        Star,               // 星形
        Letter,             // 字母形状
        
        // 多指手势
        TwoFingerTap,       // 双指点击
        ThreeFingerTap,     // 三指点击
        TwoFingerSwipe,     // 双指滑动
        ThreeFingerSwipe,   // 三指滑动
        FourFingerSwipe,    // 四指滑动
        
        // 时间相关手势
        SlowSwipe,          // 慢速滑动
        FastSwipe,          // 快速滑动
        AcceleratingSwipe,  // 加速滑动
        DeceleratingSwipe   // 减速滑动
    };

    enum SwipeDirection {
        SwipeLeft,
        SwipeRight,
        SwipeUp,
        SwipeDown,
        SwipeUpLeft,
        SwipeUpRight,
        SwipeDownLeft,
        SwipeDownRight
    };

    struct GestureData {
        GestureType type;
        QPointF startPos;
        QPointF endPos;
        QPointF currentPos;
        qreal velocity;
        qreal distance;
        qreal angle;
        qint64 duration;
        int fingerCount;
        SwipeDirection direction;
        QVector<QPointF> path;
        
        QJsonObject toJson() const;
        static GestureData fromJson(const QJsonObject &json);
    };

    explicit AdvancedGestureRecognizer(QWidget *parent);
    
    // 启用/禁用手势识别
    void setGestureEnabled(GestureType type, bool enabled);
    bool isGestureEnabled(GestureType type) const;
    
    // 手势参数配置
    void setTapTimeout(int ms);                    // 点击超时
    void setDoubleTapInterval(int ms);             // 双击间隔
    void setLongPressThreshold(int ms);            // 长按阈值
    void setPanThreshold(int pixels);              // 拖拽阈值
    void setSwipeMinDistance(int pixels);          // 滑动最小距离
    void setSwipeMaxTime(int ms);                  // 滑动最大时间
    
    // 自定义手势训练
    void startGestureTraining(const QString &gestureName);
    void stopGestureTraining();
    void saveTrainedGesture(const QString &gestureName);
    void loadTrainedGestures(const QString &filePath);
    
    // 手势模板匹配
    void addGestureTemplate(const QString &name, const QVector<QPointF> &template_);
    qreal matchGesture(const QVector<QPointF> &gesture, const QString &templateName);

signals:
    void gestureDetected(GestureType type, const GestureData &data);
    void customGestureDetected(const QString &gestureName, qreal confidence);
    void gestureStarted(GestureType type, const QPointF &startPos);
    void gestureUpdated(GestureType type, const QPointF &currentPos);
    void gestureFinished(GestureType type, const GestureData &data);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onTapTimeout();
    void onLongPressTimeout();

private:
    // 手势状态跟踪
    struct GestureState {
        bool active;
        QPointF startPos;
        QPointF lastPos;
        qint64 startTime;
        qint64 lastTime;
        QVector<QPointF> path;
        int fingerCount;
        qreal totalDistance;
    };
    
    // 事件处理
    void handleMousePress(QMouseEvent *event);
    void handleMouseMove(QMouseEvent *event);
    void handleMouseRelease(QMouseEvent *event);
    void handleTouchBegin(QTouchEvent *event);
    void handleTouchUpdate(QTouchEvent *event);
    void handleTouchEnd(QTouchEvent *event);
    
    // 手势识别算法
    bool recognizeTap();
    bool recognizeDoubleTap();
    bool recognizeLongPress();
    bool recognizePan();
    bool recognizeSwipe();
    bool recognizeCircle();
    bool recognizeCustomShape();
    
    // 几何计算
    qreal calculateDistance(const QPointF &p1, const QPointF &p2);
    qreal calculateAngle(const QPointF &p1, const QPointF &p2);
    qreal calculateVelocity(const QPointF &p1, const QPointF &p2, qint64 timeDiff);
    SwipeDirection calculateSwipeDirection(const QPointF &start, const QPointF &end);
    bool isCircularMotion(const QVector<QPointF> &path, qreal &clockwise);
    
    // 模板匹配算法
    qreal dollarOneRecognizer(const QVector<QPointF> &gesture, const QVector<QPointF> &template_);
    QVector<QPointF> resamplePath(const QVector<QPointF> &path, int numPoints);
    QVector<QPointF> rotatePath(const QVector<QPointF> &path, qreal angle);
    QVector<QPointF> scalePath(const QVector<QPointF> &path);
    QVector<QPointF> translatePath(const QVector<QPointF> &path, const QPointF &centroid);
    
    QWidget *m_targetWidget;
    QMap<GestureType, bool> m_enabledGestures;
    
    // 手势状态
    GestureState m_currentGesture;
    bool m_gestureInProgress;
    QTimer *m_tapTimer;
    QTimer *m_longPressTimer;
    
    // 配置参数
    int m_tapTimeout;
    int m_doubleTapInterval;
    int m_longPressThreshold;
    int m_panThreshold;
    int m_swipeMinDistance;
    int m_swipeMaxTime;
    
    // 双击检测
    bool m_waitingForSecondTap;
    QPointF m_firstTapPos;
    qint64 m_firstTapTime;
    
    // 自定义手势
    bool m_trainingMode;
    QString m_trainingGestureName;
    QMap<QString, QVector<QPointF>> m_gestureTemplates;
    
    // 多指跟踪
    QMap<int, QPointF> m_touchPoints;
    QMap<int, qint64> m_touchStartTimes;
};

/**
 * 音乐播放器专用手势处理器
 * 为音乐播放器定制的手势命令映射
 */
class MusicPlayerGestureHandler : public QObject {
    Q_OBJECT

public:
    enum MusicAction {
        PlayPause,          // 播放/暂停
        NextTrack,          // 下一首
        PreviousTrack,      // 上一首
        VolumeUp,           // 音量加
        VolumeDown,         // 音量减
        Seek,               // 跳转
        ToggleShuffle,      // 切换随机
        ToggleRepeat,       // 切换重复
        ShowPlaylist,       // 显示播放列表
        ShowEqualizer,      // 显示均衡器
        ShowLyrics,         // 显示歌词
        ToggleVisualizer,   // 切换可视化
        ToggleMute,         // 静音切换
        ShowSearch,         // 显示搜索
        ToggleFullscreen,   // 全屏切换
        MinimizeToTray,     // 最小化到托盘
        CreatePlaylist,     // 创建播放列表
        RateTrack,          // 评分
        AddToFavorites,     // 加入收藏
        ShowTrackInfo       // 显示曲目信息
    };

    explicit MusicPlayerGestureHandler(QObject *parent = nullptr);
    
    // 设置手势识别器
    void setGestureRecognizer(AdvancedGestureRecognizer *recognizer);
    
    // 手势动作映射
    void mapGestureToAction(AdvancedGestureRecognizer::GestureType gesture, 
                           MusicAction action,
                           const QVariant &parameter = QVariant());
    void mapCustomGestureToAction(const QString &gestureName, 
                                 MusicAction action,
                                 const QVariant &parameter = QVariant());
    
    // 手势配置预设
    void loadDefaultGestures();
    void loadTouchscreenGestures();
    void loadTrackpadGestures();
    void loadCustomGestures(const QString &filePath);
    
    // 手势反馈
    void setHapticFeedbackEnabled(bool enabled);
    void setVisualFeedbackEnabled(bool enabled);
    void setAudioFeedbackEnabled(bool enabled);
    
    // 配置管理
    void saveGestureConfig(const QString &filePath);
    void loadGestureConfig(const QString &filePath);

signals:
    void actionTriggered(MusicAction action, const QVariant &parameter);
    void gestureExecuted(const QString &gestureName, MusicAction action);

private slots:
    void onGestureDetected(AdvancedGestureRecognizer::GestureType type, 
                          const AdvancedGestureRecognizer::GestureData &data);
    void onCustomGestureDetected(const QString &gestureName, qreal confidence);

private:
    struct GestureMapping {
        MusicAction action;
        QVariant parameter;
        bool enabled;
        qreal minimumConfidence;
    };
    
    void executeAction(MusicAction action, const QVariant &parameter);
    void provideFeedback(const QString &actionName);
    void showVisualFeedback(const QPointF &position, const QString &actionName);
    void playHapticFeedback();
    void playAudioFeedback(const QString &soundName);
    
    AdvancedGestureRecognizer *m_gestureRecognizer;
    
    QMap<AdvancedGestureRecognizer::GestureType, GestureMapping> m_gestureMap;
    QMap<QString, GestureMapping> m_customGestureMap;
    
    // 反馈设置
    bool m_hapticFeedbackEnabled;
    bool m_visualFeedbackEnabled;
    bool m_audioFeedbackEnabled;
    
    // 视觉反馈
    QTimer *m_feedbackTimer;
    QPropertyAnimation *m_feedbackAnimation;
};

/**
 * 全局快捷键管理器
 * 管理应用程序的快捷键系统
 */
class GlobalShortcutManager : public QObject {
    Q_OBJECT

public:
    enum ShortcutContext {
        Global,             // 全局快捷键
        ApplicationFocus,   // 应用有焦点时
        PlayerWidget,       // 播放器控件
        PlaylistWidget,     // 播放列表控件
        EqualizerWidget,    // 均衡器控件
        SearchWidget,       // 搜索控件
        VisualizerWidget    // 可视化控件
    };

    struct ShortcutInfo {
        QString name;
        QString description;
        QKeySequence keySequence;
        ShortcutContext context;
        bool enabled;
        QAction *action;
        
        QJsonObject toJson() const;
        static ShortcutInfo fromJson(const QJsonObject &json);
    };

    explicit GlobalShortcutManager(QObject *parent = nullptr);
    ~GlobalShortcutManager();
    
    // 快捷键注册
    bool registerShortcut(const QString &name, 
                         const QKeySequence &keySequence,
                         ShortcutContext context = Global);
    bool unregisterShortcut(const QString &name);
    
    // 快捷键管理
    void setShortcutEnabled(const QString &name, bool enabled);
    void setShortcutKeySequence(const QString &name, const QKeySequence &keySequence);
    void setShortcutContext(const QString &name, ShortcutContext context);
    
    // 快捷键查询
    QStringList getAllShortcutNames() const;
    ShortcutInfo getShortcutInfo(const QString &name) const;
    QVector<ShortcutInfo> getShortcutsByContext(ShortcutContext context) const;
    
    // 预设快捷键
    void loadDefaultShortcuts();
    void loadMediaPlayerShortcuts();
    void loadCustomShortcuts(const QString &filePath);
    
    // 快捷键冲突检测
    bool hasConflict(const QKeySequence &keySequence, ShortcutContext context) const;
    QStringList getConflictingShortcuts(const QKeySequence &keySequence, 
                                       ShortcutContext context) const;
    
    // 配置管理
    void saveShortcuts(const QString &filePath);
    void loadShortcuts(const QString &filePath);
    void resetToDefaults();
    
    // 上下文管理
    void setActiveContext(ShortcutContext context);
    ShortcutContext activeContext() const { return m_activeContext; }

signals:
    void shortcutActivated(const QString &name);
    void shortcutRegistered(const QString &name);
    void shortcutUnregistered(const QString &name);
    void contextChanged(ShortcutContext context);

private slots:
    void onShortcutActivated();

private:
    void updateShortcutAction(const QString &name);
    QWidget* getContextWidget(ShortcutContext context);
    
    QMap<QString, ShortcutInfo> m_shortcuts;
    ShortcutContext m_activeContext;
    
    // 上下文控件映射
    QMap<ShortcutContext, QWidget*> m_contextWidgets;
};

/**
 * 快捷键配置对话框
 * 用于配置和自定义快捷键的UI对话框
 */
class ShortcutConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit ShortcutConfigDialog(GlobalShortcutManager *manager, QWidget *parent = nullptr);

private slots:
    void onShortcutChanged();
    void onResetToDefaults();
    void onImportConfig();
    void onExportConfig();
    void onSearchTextChanged();

private:
    void setupUI();
    void updateShortcutList();
    void showConflictWarning(const QKeySequence &keySequence);
    
    GlobalShortcutManager *m_shortcutManager;
    
    // UI组件将在实现中创建
    class QTreeWidget *m_shortcutTree;
    class QLineEdit *m_searchEdit;
    class QKeySequenceEdit *m_keySequenceEdit;
    class QComboBox *m_contextCombo;
    class QPushButton *m_resetButton;
    class QPushButton *m_importButton;
    class QPushButton *m_exportButton;
};

/**
 * 触摸优化界面
 * 为触摸设备优化的播放器界面
 */
class TouchOptimizedInterface : public QWidget {
    Q_OBJECT

public:
    explicit TouchOptimizedInterface(QWidget *parent = nullptr);
    
    // 触摸优化设置
    void setTouchTargetSize(int size);           // 触摸目标最小尺寸
    void setSwipeGesturesEnabled(bool enabled);  // 滑动手势
    void setPinchZoomEnabled(bool enabled);      // 缩放手势
    void setLongPressMenuEnabled(bool enabled);  // 长按菜单
    
    // 触摸反馈
    void setTouchFeedbackEnabled(bool enabled);
    void setTouchRippleEnabled(bool enabled);
    void setTouchSoundEnabled(bool enabled);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool event(QEvent *event) override;

private:
    void adjustLayoutForTouch();
    void createTouchFriendlyControls();
    
    int m_touchTargetSize;
    bool m_swipeGesturesEnabled;
    bool m_pinchZoomEnabled;
    bool m_longPressMenuEnabled;
    bool m_touchFeedbackEnabled;
    bool m_touchRippleEnabled;
    bool m_touchSoundEnabled;
    
    MusicPlayerGestureHandler *m_gestureHandler;
    AdvancedGestureRecognizer *m_gestureRecognizer;
};
