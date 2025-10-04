#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QSplitter>
#include <QTabWidget>
#include <QSystemTrayIcon>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsEffect>
#include <QNetworkAccessManager>
#include <QJsonObject>

// 引入我们创建的高级组件
#include "materialui_components.h"
#include "smart_playlist.h"
#include "spectrum_analyzer.h"
#include "audio_effects.h"
#include "gesture_system.h"
#include "../include/ffmpegplayer.h"
#include "../include/songinfo.h"

/**
 * 增强版音乐播放器主窗口
 * 集成所有先进功能的下一代播放器界面
 */
class EnhancedPlayerWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EnhancedPlayerWindow(QWidget *parent = nullptr);
    ~EnhancedPlayerWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    // 播放控制
    void playPause();
    void nextTrack();
    void previousTrack();
    void seekToPosition(qreal position);
    void setVolume(int volume);
    void toggleMute();
    void changePlayMode();
    
    // 界面切换
    void switchToVisualizerMode();
    void switchToLibraryMode();
    void switchToPlaylistMode();
    void switchToMiniMode();
    void toggleFullscreen();
    
    // 音频效果
    void showEffectsPanel();
    void toggleEqualizer();
    void applyAudioEffect(const QString &effectName);
    
    // 智能功能
    void generateSmartPlaylist();
    void showRecommendations();
    void searchMusic();
    void showMusicStats();
    
    // 在线服务
    void searchOnlineMusic();
    void downloadLyrics();
    void downloadAlbumArt();
    void syncWithCloud();
    
    // 系统托盘
    void minimizeToTray();
    void restoreFromTray();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    
    // 手势处理
    void onGestureAction(MusicPlayerGestureHandler::MusicAction action, const QVariant &parameter);
    
    // 主题和外观
    void switchTheme();
    void customizeInterface();
    void loadSkin(const QString &skinPath);
    
    // 设置和配置
    void showSettings();
    void showShortcutConfig();
    void showAdvancedSettings();

private:
    // UI初始化
    void setupUI();
    void setupMainInterface();
    void setupSidePanels();
    void setupVisualizerPanel();
    void setupControlPanel();
    void setupSystemTray();
    void setupAnimations();
    void setupGestures();
    void setupShortcuts();
    
    // 布局模式
    void setupStandardLayout();
    void setupCompactLayout();
    void setupVisualizerLayout();
    void setupMiniPlayerLayout();
    
    // 样式和主题
    void applyMaterialDesign();
    void applyCustomTheme(const QJsonObject &theme);
    void updateColors();
    void createGlowEffects();
    
    // 功能集成
    void initializeAudioEngine();
    void initializeSmartFeatures();
    void initializeOnlineServices();
    void initializeVisualization();
    
    // 状态管理
    void saveWindowState();
    void restoreWindowState();
    void updatePlaybackInfo();
    void updateVisualization();
    
    // 工具方法
    void showNotification(const QString &title, const QString &message);
    void startGlowAnimation(QWidget *widget);
    void createFloatingAction(const QString &text, const QIcon &icon, const QPoint &position);
    
    // === 核心组件 ===
    FFmpegPlayer *m_player;
    
    // === UI组件 ===
    QStackedWidget *m_mainStack;
    QSplitter *m_mainSplitter;
    QTabWidget *m_sideTabWidget;
    
    // Material Design 组件
    MaterialCard *m_nowPlayingCard;
    MaterialCard *m_playlistCard;
    MaterialCard *m_visualizerCard;
    MaterialButton *m_playButton;
    MaterialButton *m_nextButton;
    MaterialButton *m_prevButton;
    AdvancedProgressBar *m_progressBar;
    
    // 可视化组件
    SpectrumAnalyzer *m_spectrumAnalyzer;
    Audio3DVisualizer *m_3dVisualizer;
    VisualizerControlPanel *m_visualizerControls;
    RealTimeAudioProcessor *m_audioProcessor;
    
    // 音频效果
    AudioEffectChain *m_effectChain;
    ReverbEffect *m_reverbEffect;
    EchoEffect *m_echoEffect;
    ChorusEffect *m_chorusEffect;
    MultibandEqualizer *m_equalizer;
    SpatialAudioProcessor *m_spatialProcessor;
    
    // 智能功能
    MusicAnalyzer *m_musicAnalyzer;
    RecommendationEngine *m_recommendationEngine;
    SmartPlaylistManager *m_smartPlaylistManager;
    OnlineMusicService *m_onlineService;
    
    // 手势和交互
    AdvancedGestureRecognizer *m_gestureRecognizer;
    MusicPlayerGestureHandler *m_gestureHandler;
    GlobalShortcutManager *m_shortcutManager;
    TouchOptimizedInterface *m_touchInterface;
    
    // 系统集成
    QSystemTrayIcon *m_trayIcon;
    QNetworkAccessManager *m_networkManager;
    
    // === 状态变量 ===
    enum InterfaceMode {
        StandardMode,
        VisualizerMode,
        LibraryMode,
        MiniMode,
        FullscreenMode
    };
    
    InterfaceMode m_currentMode;
    bool m_isPlaying;
    bool m_isMuted;
    bool m_isFullscreen;
    bool m_isDarkTheme;
    int m_currentVolume;
    qint64 m_currentPosition;
    qint64 m_totalDuration;
    
    // 播放列表和库
    QVector<SongInfo> m_currentPlaylist;
    int m_currentTrackIndex;
    QStringList m_musicLibrary;
    
    // 动画效果
    QPropertyAnimation *m_fadeAnimation;
    QPropertyAnimation *m_slideAnimation;
    QParallelAnimationGroup *m_transitionAnimation;
    QGraphicsEffect *m_glowEffect;
    
    // 配置和设置
    QJsonObject m_settings;
    QString m_currentSkinPath;
    QMap<QString, QVariant> m_shortcuts;
    
    // 在线服务状态
    bool m_onlineServicesEnabled;
    QString m_lastFmApiKey;
    QString m_spotifyApiKey;
    QString m_lyricsApiKey;
    
    // 性能监控
    QTimer *m_performanceTimer;
    int m_frameRate;
    double m_cpuUsage;
    double m_memoryUsage;
};

/**
 * 设置对话框
 * 管理播放器的所有配置选项
 */
class EnhancedSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit EnhancedSettingsDialog(EnhancedPlayerWindow *parent);

private slots:
    void onTabChanged(int index);
    void onApplySettings();
    void onResetToDefaults();
    void onImportSettings();
    void onExportSettings();

private:
    void setupUI();
    void setupGeneralTab();
    void setupAudioTab();
    void setupVisualizationTab();
    void setupOnlineTab();
    void setupAdvancedTab();
    void setupShortcutsTab();
    
    EnhancedPlayerWindow *m_playerWindow;
    QTabWidget *m_tabWidget;
};

/**
 * 迷你播放器窗口
 * 紧凑的迷你播放器界面
 */
class MiniPlayerWindow : public QWidget {
    Q_OBJECT

public:
    explicit MiniPlayerWindow(EnhancedPlayerWindow *parent);
    
    void updateTrackInfo(const SongInfo &song);
    void updatePlaybackState(bool playing);
    void updateProgress(qreal progress);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onPlayPauseClicked();
    void onNextClicked();
    void onPreviousClicked();
    void onCloseClicked();
    void onExpandClicked();

signals:
    void playPauseRequested();
    void nextTrackRequested();
    void previousTrackRequested();
    void expandRequested();
    void closeRequested();

private:
    void setupUI();
    void updateControlsVisibility();
    
    EnhancedPlayerWindow *m_mainWindow;
    bool m_isHovered;
    bool m_isDragging;
    QPoint m_dragStartPos;
    
    // Mini player controls
    MaterialButton *m_playButton;
    MaterialButton *m_nextButton;
    MaterialButton *m_prevButton;
    MaterialButton *m_closeButton;
    MaterialButton *m_expandButton;
    AdvancedProgressBar *m_progressBar;
    
    // Track info
    QLabel *m_trackTitle;
    QLabel *m_artistName;
    QLabel *m_albumArt;
    
    // Current track data
    SongInfo m_currentSong;
    bool m_isPlaying;
    qreal m_progress;
};
