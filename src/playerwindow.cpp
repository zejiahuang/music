#include "../include/playerwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMimeData>
#include <QUrl>
#include <QApplication>
#include <QScreen>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QDebug>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QFrame>
#include <QSplitter>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QStandardPaths>
#include <QListWidgetItem>
#include "../include/ffmpeg_waveform.h"
#include "../include/taglib_utils.h"
#include "../include/materialui_components.h"

PlayerWindow::PlayerWindow(QWidget *parent) 
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , mainSplitter(nullptr)
    , leftPanel(nullptr)
    , rightPanel(nullptr)
    , lyricsVisualWidget(nullptr)
    , searchEdit(nullptr)
    , playlistWidget(nullptr)
    , albumCoverLabel(nullptr)
    , songTitleLabel(nullptr)
    , artistLabel(nullptr)
    , albumLabel(nullptr)
    , controlsFrame(nullptr)
    , playPauseButton(nullptr)
    , previousButton(nullptr)
    , nextButton(nullptr)
    , playModeButton(nullptr)
    , volumeButton(nullptr)
    , equalizerButton(nullptr)
    , themeButton(nullptr)
    , progressSlider(nullptr)
    , volumeSlider(nullptr)
    , currentTimeLabel(nullptr)
    , totalTimeLabel(nullptr)
    , volumeFrame(nullptr)
    , equalizerWindow(nullptr)
    , player(nullptr)
    , progressTimer(nullptr)
    , currentPlayMode(PlayMode::Sequential)
    , isDarkTheme(false)
    , currentTrackIndex(-1)
    , totalDuration(0)
    , volumeAnimation(nullptr)
    , shadowEffect(nullptr)
    , playlistCard(nullptr)
    , nowPlayingCard(nullptr)
    , controlsCard(nullptr)
    , materialPlayButton(nullptr)
    , materialNextButton(nullptr)
    , materialPrevButton(nullptr)
    , materialVolumeButton(nullptr)
    , materialThemeButton(nullptr)
    , materialProgressBar(nullptr)
    , materialVolumeSlider(nullptr)
    , cardAnimation(nullptr)
    , glowAnimation(nullptr)
    , layoutAnimation(nullptr)
{
    // 启用拖放功能
    setAcceptDrops(true);
    
    // 初始化核心组件
    player = new FFmpegPlayer(this);
    progressTimer = new QTimer(this);
    progressTimer->setInterval(1000); // 每秒更新一次
    
    setupUi();
    setupAnimations();
    setupConnections();
    
    // 设置窗口属性在applyEnhancedMaterialStyle中实现
}

void PlayerWindow::setupUi() {
    // 创建中央控件
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // 创建主分割器
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    
    // 设置主布局
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 创建Material Design卡片布局
    setupMaterialCards();
    setupLeftPanel();
    setupRightPanel();
    setupMaterialControls();
    // setupControls(); // 暂时禁用传统控件，使用Material Design组件
    
    // 组装布局
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(20);
    
    contentLayout->addWidget(mainSplitter, 1);
    contentLayout->addWidget(controlsCard);
    
    mainLayout->addLayout(contentLayout);
    
    // 设置分割器比例和属性
    mainSplitter->setSizes({350, 850});
    mainSplitter->setCollapsible(0, false);
    mainSplitter->setCollapsible(1, false);
    mainSplitter->setHandleWidth(1);
    
    // 应用现代样式
    applyEnhancedMaterialStyle();
}

void PlayerWindow::setupMaterialCards() {
    // 创建播放列表卡片
    playlistCard = new MaterialCard();
    playlistCard->setElevation(4);
    playlistCard->setCornerRadius(16);
    playlistCard->setCardColor(QColor(255, 255, 255, 250));
    
    // 创建正在播放卡片
    nowPlayingCard = new MaterialCard();
    nowPlayingCard->setElevation(6);
    nowPlayingCard->setCornerRadius(20);
    nowPlayingCard->setCardColor(QColor(255, 255, 255, 245));
    
    // 创建控制面板卡片
    controlsCard = new MaterialCard();
    controlsCard->setElevation(8);
    controlsCard->setCornerRadius(24);
    controlsCard->setCardColor(QColor(255, 255, 255, 250));
    controlsCard->setFixedHeight(120);
    
    // 设置动画效果
    cardAnimation = new QPropertyAnimation(controlsCard, "elevation");
    cardAnimation->setDuration(200);
    cardAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void PlayerWindow::setupMaterialControls() {
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsCard);
    controlsLayout->setContentsMargins(30, 20, 30, 20);
    controlsLayout->setSpacing(20);
    
    // 左侧控制按钮组
    QFrame *leftControls = new QFrame();
    QHBoxLayout *leftLayout = new QHBoxLayout(leftControls);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);
    
    // Material Design 按钮
    materialPrevButton = new MaterialButton("", MaterialButton::Floating);
    materialPrevButton->setIcon(QIcon(":icons/previous.png"));
    materialPrevButton->setFixedSize(48, 48);
    materialPrevButton->setAccentColor(QColor(103, 58, 183));
    
    materialPlayButton = new MaterialButton("", MaterialButton::Floating);
    materialPlayButton->setIcon(QIcon(":icons/play.png"));
    materialPlayButton->setFixedSize(64, 64);
    materialPlayButton->setAccentColor(QColor(103, 58, 183));
    materialPlayButton->setShadowElevation(8);
    
    materialNextButton = new MaterialButton("", MaterialButton::Floating);
    materialNextButton->setIcon(QIcon(":icons/next.png"));
    materialNextButton->setFixedSize(48, 48);
    materialNextButton->setAccentColor(QColor(103, 58, 183));
    
    leftLayout->addWidget(materialPrevButton);
    leftLayout->addWidget(materialPlayButton);
    leftLayout->addWidget(materialNextButton);
    
    // 中间进度区域
    QFrame *progressFrame = new QFrame();
    QVBoxLayout *progressLayout = new QVBoxLayout(progressFrame);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(8);
    
    // Material Design 进度条
    materialProgressBar = new AdvancedProgressBar();
    materialProgressBar->setAccentColor(QColor(103, 58, 183));
    materialProgressBar->setTrackColor(QColor(230, 230, 230));
    materialProgressBar->setBufferColor(QColor(200, 200, 200));
    materialProgressBar->setAnimationEnabled(true);
    materialProgressBar->setFixedHeight(6);
    
    // 时间显示
    QFrame *timeFrame = new QFrame();
    QHBoxLayout *timeLayout = new QHBoxLayout(timeFrame);
    timeLayout->setContentsMargins(0, 0, 0, 0);
    
    currentTimeLabel = new QLabel("00:00");
    currentTimeLabel->setStyleSheet("color: #666; font-family: 'Segoe UI'; font-size: 12px;");
    totalTimeLabel = new QLabel("00:00");
    totalTimeLabel->setStyleSheet("color: #666; font-family: 'Segoe UI'; font-size: 12px;");
    
    timeLayout->addWidget(currentTimeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(totalTimeLabel);
    
    progressLayout->addWidget(materialProgressBar);
    progressLayout->addWidget(timeFrame);
    
    // 右侧功能按钮
    QFrame *rightControls = new QFrame();
    QHBoxLayout *rightLayout = new QHBoxLayout(rightControls);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);
    
    // 音量控件
    materialVolumeButton = new MaterialButton("", MaterialButton::Flat);
    materialVolumeButton->setIcon(QIcon(":icons/volume.png"));
    materialVolumeButton->setFixedSize(40, 40);
    materialVolumeButton->setAccentColor(QColor(103, 58, 183));
    
    materialVolumeSlider = new QSlider(Qt::Horizontal);
    materialVolumeSlider->setRange(0, 100);
    materialVolumeSlider->setValue(70);
    materialVolumeSlider->setFixedWidth(100);
    
    // 主题切换按钮
    materialThemeButton = new MaterialButton("", MaterialButton::Outlined);
    materialThemeButton->setIcon(QIcon(":icons/theme.png"));
    materialThemeButton->setFixedSize(40, 40);
    materialThemeButton->setAccentColor(QColor(103, 58, 183));
    
    rightLayout->addWidget(materialVolumeButton);
    rightLayout->addWidget(materialVolumeSlider);
    rightLayout->addWidget(materialThemeButton);
    
    // 组装所有控件
    controlsLayout->addWidget(leftControls);
    controlsLayout->addWidget(progressFrame, 1);
    controlsLayout->addWidget(rightControls);
    
    // 连接信号
    connect(materialPlayButton, &MaterialButton::clicked, this, &PlayerWindow::playPause);
    connect(materialNextButton, &MaterialButton::clicked, this, &PlayerWindow::nextTrack);
    connect(materialPrevButton, &MaterialButton::clicked, this, &PlayerWindow::previousTrack);
    connect(materialThemeButton, &MaterialButton::clicked, this, &PlayerWindow::toggleTheme);
    connect(materialProgressBar, &AdvancedProgressBar::clicked, [this](qreal position) {
        if (totalDuration > 0) {
            qint64 seekPosition = qint64(position * totalDuration);
            seekToPosition(seekPosition);
        }
    });
}

void PlayerWindow::setupLeftPanel() {
    // 将播放列表组件添加到Material Card中
    QVBoxLayout *cardLayout = new QVBoxLayout(playlistCard);
    cardLayout->setContentsMargins(24, 24, 24, 24);
    cardLayout->setSpacing(20);
    
    // 标题区域
    QLabel *titleLabel = new QLabel("🎵 音乐库");
    titleLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "color: #333;"
        "font-family: 'Microsoft YaHei';"
        "margin-bottom: 10px;"
    );
    
    // 现代化搜索框
    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("🔍 搜索音乐...");
    searchEdit->setStyleSheet(
        "QLineEdit {"
        "    background: rgba(255, 255, 255, 0.8);"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 24px;"
        "    padding: 12px 20px;"
        "    font-size: 14px;"
        "    color: #333;"
        "    font-family: 'Microsoft YaHei';"
        "}"
        "QLineEdit:focus {"
        "    border-color: #673ab7;"
        "    background: rgba(255, 255, 255, 1.0);"
        "    outline: none;"
        "}"
        "QLineEdit::placeholder {"
        "    color: #999;"
        "}"
    );
    
    // 现代化播放列表
    playlistWidget = new QListWidget();
    playlistWidget->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "    outline: none;"
        "    font-family: 'Microsoft YaHei';"
        "    font-size: 14px;"
        "}"
        "QListWidget::item {"
        "    background: rgba(255, 255, 255, 0.6);"
        "    border: none;"
        "    border-radius: 12px;"
        "    padding: 16px;"
        "    margin: 4px 0;"
        "    color: #333;"
        "}"
        "QListWidget::item:hover {"
        "    background: rgba(103, 58, 183, 0.1);"
        "    transform: translateY(-2px);"
        "}"
        "QListWidget::item:selected {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "        stop:0 rgba(103, 58, 183, 0.8), stop:1 rgba(103, 58, 183, 0.6));"
        "    color: white;"
        "    font-weight: bold;"
        "}"
    );
    
    // 统计信息区域
    QFrame *statsFrame = new QFrame();
    QHBoxLayout *statsLayout = new QHBoxLayout(statsFrame);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *statsLabel = new QLabel("📊 0 首歌曲");
    statsLabel->setStyleSheet(
        "color: #666;"
        "font-size: 12px;"
        "font-family: 'Microsoft YaHei';"
    );
    
    statsLayout->addWidget(statsLabel);
    statsLayout->addStretch();
    
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(searchEdit);
    cardLayout->addWidget(playlistWidget, 1);
    cardLayout->addWidget(statsFrame);
    
    mainSplitter->addWidget(playlistCard);
    
    // 连接搜索信号
    connect(searchEdit, &QLineEdit::textChanged, this, &PlayerWindow::onSearchTextChanged);
    connect(playlistWidget, &QListWidget::itemClicked, this, &PlayerWindow::onPlaylistItemClicked);
}

void PlayerWindow::setupRightPanel() {
    // 将右侧面板组件添加到Material Card中
    QVBoxLayout *cardLayout = new QVBoxLayout(nowPlayingCard);
    cardLayout->setContentsMargins(32, 32, 32, 32);
    cardLayout->setSpacing(24);
    
    // 专辑封面和信息区域
    QFrame *nowPlayingFrame = new QFrame();
    nowPlayingFrame->setStyleSheet(
        "QFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "        stop:0 rgba(103, 58, 183, 0.05), stop:1 rgba(156, 77, 204, 0.05));"
        "    border-radius: 20px;"
        "    border: 1px solid rgba(103, 58, 183, 0.1);"
        "}"
    );
    
    QHBoxLayout *nowPlayingLayout = new QHBoxLayout(nowPlayingFrame);
    nowPlayingLayout->setContentsMargins(24, 24, 24, 24);
    nowPlayingLayout->setSpacing(24);
    
    // 专辑封面容器
    QFrame *albumContainer = new QFrame();
    albumContainer->setFixedSize(160, 160);
    albumContainer->setStyleSheet(
        "QFrame {"
        "    background: white;"
        "    border-radius: 16px;"
        "    border: 3px solid rgba(103, 58, 183, 0.2);"
        "}"
    );
    
    QVBoxLayout *albumLayout = new QVBoxLayout(albumContainer);
    albumLayout->setContentsMargins(8, 8, 8, 8);
    
    // 专辑封面
    albumCoverLabel = new QLabel();
    albumCoverLabel->setFixedSize(140, 140);
    albumCoverLabel->setScaledContents(true);
    albumCoverLabel->setAlignment(Qt::AlignCenter);
    albumCoverLabel->setStyleSheet(
        "QLabel {"
        "    border-radius: 12px;"
        "    background: #f5f5f5;"
        "}"
    );
    albumCoverLabel->setPixmap(createDefaultAlbumCover());
    
    albumLayout->addWidget(albumCoverLabel);
    
    // 歌曲信息区域
    QFrame *infoFrame = new QFrame();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoFrame);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(12);
    
    // 歌曲标题
    songTitleLabel = new QLabel("暂无播放");
    songTitleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: #333;"
        "    font-family: 'Microsoft YaHei';"
        "}"
    );
    songTitleLabel->setWordWrap(true);
    
    // 艺术家
    artistLabel = new QLabel("未知艺术家");
    artistLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 16px;"
        "    color: #666;"
        "    font-family: 'Microsoft YaHei';"
        "}"
    );
    
    // 专辑
    albumLabel = new QLabel("未知专辑");
    albumLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    color: #999;"
        "    font-family: 'Microsoft YaHei';"
        "}"
    );
    
    // 歌曲状态指示器
    QLabel *statusIcon = new QLabel("🎵");
    statusIcon->setStyleSheet(
        "QLabel {"
        "    font-size: 20px;"
        "    color: #673ab7;"
        "}"
    );
    
    infoLayout->addWidget(songTitleLabel);
    infoLayout->addWidget(artistLabel);
    infoLayout->addWidget(albumLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(statusIcon);
    
    nowPlayingLayout->addWidget(albumContainer);
    nowPlayingLayout->addWidget(infoFrame, 1);
    
    // 歌词可视化控件区域
    QFrame *visualizerFrame = new QFrame();
    visualizerFrame->setStyleSheet(
        "QFrame {"
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "        stop:0 rgba(255, 255, 255, 0.9), stop:1 rgba(240, 240, 255, 0.9));"
        "    border-radius: 16px;"
        "    border: 1px solid rgba(103, 58, 183, 0.1);"
        "}"
    );
    
    QVBoxLayout *visualizerLayout = new QVBoxLayout(visualizerFrame);
    visualizerLayout->setContentsMargins(20, 20, 20, 20);
    
    // 可视化标题
    QLabel *visualizerTitle = new QLabel("🎨 音频可视化");
    visualizerTitle->setStyleSheet(
        "QLabel {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: #673ab7;"
        "    font-family: 'Microsoft YaHei';"
        "    margin-bottom: 10px;"
        "}"
    );
    
    // 歌词可视化控件
    lyricsVisualWidget = new LyricsVisualWidget();
    lyricsVisualWidget->setStyleSheet(
        "LyricsVisualWidget {"
        "    background: transparent;"
        "    border: none;"
        "}"
    );
    
    visualizerLayout->addWidget(visualizerTitle);
    visualizerLayout->addWidget(lyricsVisualWidget, 1);
    
    cardLayout->addWidget(nowPlayingFrame);
    cardLayout->addWidget(visualizerFrame, 1);
    
    mainSplitter->addWidget(nowPlayingCard);
}

void PlayerWindow::setupControls() {
    controlsFrame = new QFrame();
    controlsFrame->setObjectName("controlsFrame");
    controlsFrame->setFixedHeight(100);
    
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsFrame);
    controlsLayout->setContentsMargins(20, 10, 20, 10);
    controlsLayout->setSpacing(15);
    
    // 左侧控制按钮
    QFrame *transportFrame = new QFrame();
    QHBoxLayout *transportLayout = new QHBoxLayout(transportFrame);
    transportLayout->setContentsMargins(0, 0, 0, 0);
    transportLayout->setSpacing(10);
    
    // 播放模式按钮
    playModeButton = new QPushButton();
    playModeButton->setObjectName("playModeButton");
    playModeButton->setFixedSize(40, 40);
    playModeButton->setToolTip("播放模式");
    
    // 上一首
    previousButton = new QPushButton();
    previousButton->setObjectName("previousButton");
    previousButton->setFixedSize(40, 40);
    previousButton->setToolTip("上一首");
    
    // 播放/暂停
    playPauseButton = new QPushButton();
    playPauseButton->setObjectName("playPauseButton");
    playPauseButton->setFixedSize(50, 50);
    playPauseButton->setToolTip("播放/暂停");
    
    // 下一首
    nextButton = new QPushButton();
    nextButton->setObjectName("nextButton");
    nextButton->setFixedSize(40, 40);
    nextButton->setToolTip("下一首");
    
    transportLayout->addWidget(playModeButton);
    transportLayout->addWidget(previousButton);
    transportLayout->addWidget(playPauseButton);
    transportLayout->addWidget(nextButton);
    
    // 中间进度区域
    QFrame *progressFrame = new QFrame();
    QVBoxLayout *progressLayout = new QVBoxLayout(progressFrame);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(5);
    
    // 进度条
    progressSlider = new QSlider(Qt::Horizontal);
    progressSlider->setObjectName("progressSlider");
    progressSlider->setRange(0, 100);
    progressSlider->setValue(0);
    
    // 时间显示
    QFrame *timeFrame = new QFrame();
    QHBoxLayout *timeLayout = new QHBoxLayout(timeFrame);
    timeLayout->setContentsMargins(0, 0, 0, 0);
    timeLayout->setSpacing(10);
    
    currentTimeLabel = new QLabel("00:00");
    currentTimeLabel->setObjectName("timeLabel");
    totalTimeLabel = new QLabel("00:00");
    totalTimeLabel->setObjectName("timeLabel");
    
    timeLayout->addWidget(currentTimeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(totalTimeLabel);
    
    progressLayout->addWidget(progressSlider);
    progressLayout->addWidget(timeFrame);
    
    // 右侧功能按钮
    QFrame *functionsFrame = new QFrame();
    QHBoxLayout *functionsLayout = new QHBoxLayout(functionsFrame);
    functionsLayout->setContentsMargins(0, 0, 0, 0);
    functionsLayout->setSpacing(10);
    
    // 音量控件
    volumeButton = new QPushButton();
    volumeButton->setObjectName("volumeButton");
    volumeButton->setFixedSize(35, 35);
    volumeButton->setToolTip("音量");
    
    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setObjectName("volumeSlider");
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeSlider->setFixedWidth(80);
    
    // 均衡器按钮
    equalizerButton = new QPushButton();
    equalizerButton->setObjectName("equalizerButton");
    equalizerButton->setFixedSize(35, 35);
    equalizerButton->setToolTip("音频均衡器");
    
    // 主题切换按钮
    themeButton = new QPushButton();
    themeButton->setObjectName("themeButton");
    themeButton->setFixedSize(35, 35);
    themeButton->setToolTip("切换主题");
    
    functionsLayout->addWidget(volumeButton);
    functionsLayout->addWidget(volumeSlider);
    functionsLayout->addWidget(equalizerButton);
    functionsLayout->addWidget(themeButton);
    
    // 组装所有控件
    controlsLayout->addWidget(transportFrame);
    controlsLayout->addWidget(progressFrame, 1);
    controlsLayout->addWidget(functionsFrame);
    
    // 更新按钮图标
    updatePlayModeIcon();
    updateVolumeIcon(70);
}

void PlayerWindow::setupAnimations() {
    // 音量滑块动画
    volumeAnimation = new QPropertyAnimation(volumeSlider, "geometry");
    volumeAnimation->setDuration(300);
    volumeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 添加阴影效果
    shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(20);
    shadowEffect->setOffset(0, 2);
    shadowEffect->setColor(QColor(0, 0, 0, 30));
    controlsFrame->setGraphicsEffect(shadowEffect);
}

void PlayerWindow::setupConnections() {
    // 播放器信号连接
    connect(player, &FFmpegPlayer::positionChanged, this, &PlayerWindow::updateProgress);
    connect(player, &FFmpegPlayer::durationChanged, [this](qint64 duration) {
        totalTimeLabel->setText(formatTime(duration));
        // Material Design 进度条使用 0-1 的进度值
        // 存储最大时长供后续使用
        totalDuration = duration;
    });
    
    // Material Design 控制按钮连接（已在 setupMaterialControls 中设置）
    // 这里添加额外的连接
    connect(materialVolumeSlider, &QSlider::valueChanged, this, &PlayerWindow::setVolume);
    
    // 播放列表连接
    connect(playlistWidget, &QListWidget::itemClicked, this, &PlayerWindow::onPlaylistItemClicked);
    connect(searchEdit, &QLineEdit::textChanged, this, &PlayerWindow::onSearchTextChanged);
    
    // 进度定时器
    connect(progressTimer, &QTimer::timeout, [this]() {
        // 使用 Material Design 进度条
        if (!materialProgressBar->isEnabled()) return; // 简单检查
        qint64 position = player->position();
        if (totalDuration > 0) {
            qreal progress = qreal(position) / qreal(totalDuration);
            materialProgressBar->setProgress(progress);
        }
        currentTimeLabel->setText(formatTime(position));
    });
}

// 播放控制方法
void PlayerWindow::playPause() {
    // TODO: 实现播放/暂停逻辑
    // 这里需要根据 FFmpegPlayer 的实际API调整
}

void PlayerWindow::previousTrack() {
    if (currentTrackIndex > 0) {
        currentTrackIndex--;
        loadSong(playlist[currentTrackIndex]);
    }
}

void PlayerWindow::nextTrack() {
    if (currentTrackIndex < playlist.size() - 1) {
        currentTrackIndex++;
        loadSong(playlist[currentTrackIndex]);
    }
}

void PlayerWindow::seekToPosition(int position) {
    player->seek(position);
}

void PlayerWindow::setVolume(int volume) {
    // TODO: 实现音量设置
    updateVolumeIcon(volume);
}

void PlayerWindow::changePlayMode() {
    switch (currentPlayMode) {
        case PlayMode::Sequential:
            currentPlayMode = PlayMode::Random;
            break;
        case PlayMode::Random:
            currentPlayMode = PlayMode::SingleLoop;
            break;
        case PlayMode::SingleLoop:
            currentPlayMode = PlayMode::Sequential;
            break;
    }
    updatePlayModeIcon();
}

void PlayerWindow::updateProgress() {
    // 使用 Material Design 进度条
    if (!materialProgressBar->isEnabled()) return; // 简单检查
    qint64 position = player->position();
    if (totalDuration > 0) {
        qreal progress = qreal(position) / qreal(totalDuration);
        materialProgressBar->setProgress(progress);
    }
    currentTimeLabel->setText(formatTime(position));
    
    // 更新歌词位置
    lyricsVisualWidget->updatePosition(position);
}

void PlayerWindow::onTrackChanged() {
    // TODO: 处理曲目变化
}

void PlayerWindow::onPlaylistItemClicked() {
    int index = playlistWidget->currentRow();
    if (index >= 0 && index < playlist.size()) {
        currentTrackIndex = index;
        loadSong(playlist[index]);
    }
}

void PlayerWindow::onSearchTextChanged() {
    QString searchText = searchEdit->text().toLower();
    for (int i = 0; i < playlistWidget->count(); i++) {
        QListWidgetItem *item = playlistWidget->item(i);
        bool visible = item->text().toLower().contains(searchText);
        item->setHidden(!visible);
    }
}

void PlayerWindow::showEqualizer() {
    if (!equalizerWindow) {
        setupEqualizer();
    }
    equalizerWindow->show();
    equalizerWindow->raise();
    equalizerWindow->activateWindow();
}

void PlayerWindow::toggleTheme() {
    isDarkTheme = !isDarkTheme;
    switchTheme(isDarkTheme);
}

// 拖放事件处理
void PlayerWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void PlayerWindow::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList audioFiles;
        QStringList supportedFormats = {"mp3", "wav", "flac", "ogg", "m4a", "aac"};
        
        foreach (const QUrl &url, mimeData->urls()) {
            QString filePath = url.toLocalFile();
            QString extension = QFileInfo(filePath).suffix().toLower();
            if (supportedFormats.contains(extension)) {
                audioFiles << filePath;
            }
        }
        
        if (!audioFiles.isEmpty()) {
            addFilesToPlaylist(audioFiles);
        }
    }
    event->acceptProposedAction();
}

void PlayerWindow::applyEnhancedMaterialStyle() {
    // 主窗口样式
    QString enhancedStyle = R"(
    /* 主窗口现代化样式 */
    QMainWindow {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
            stop:0 #f8f9fa, stop:0.3 #e9ecef, stop:0.7 #dee2e6, stop:1 #f1f3f5);
        color: #212529;
        font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;
    }
    
    /* 分割器样式 */
    QSplitter::handle {
        background: rgba(103, 58, 183, 0.1);
        border-radius: 1px;
    }
    
    QSplitter::handle:hover {
        background: rgba(103, 58, 183, 0.3);
    }
    
    /* 滑块现代化样式 */
    QSlider::groove:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #e9ecef, stop:1 #f8f9fa);
        height: 8px;
        border-radius: 4px;
        border: 1px solid #dee2e6;
    }
    
    QSlider::handle:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #673ab7, stop:1 #9c4dcc);
        width: 20px;
        height: 20px;
        border-radius: 10px;
        margin-top: -6px;
        margin-bottom: -6px;
        border: 2px solid white;
        box-shadow: 0 2px 4px rgba(0,0,0,0.2);
    }
    
    QSlider::handle:horizontal:hover {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
            stop:0 #7c4dff, stop:1 #b388ff);
        transform: scale(1.1);
    }
    
    QSlider::sub-page:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
            stop:0 #673ab7, stop:1 #9c4dcc);
        border-radius: 4px;
    }
    
    /* 工具提示样式 */
    QToolTip {
        background: rgba(33, 33, 33, 0.95);
        color: white;
        border: none;
        border-radius: 8px;
        padding: 8px 12px;
        font-family: 'Microsoft YaHei';
        font-size: 12px;
    }
    
    /* 滚动条样式 */
    QScrollBar:vertical {
        background: rgba(240, 240, 240, 0.5);
        width: 12px;
        border-radius: 6px;
        margin: 0;
    }
    
    QScrollBar::handle:vertical {
        background: rgba(103, 58, 183, 0.6);
        border-radius: 6px;
        min-height: 20px;
    }
    
    QScrollBar::handle:vertical:hover {
        background: rgba(103, 58, 183, 0.8);
    }
    
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        border: none;
        background: none;
    }
    )";
    
    setStyleSheet(enhancedStyle);
    
    // 设置窗口属性
    setWindowTitle("🎵 现代音乐播放器 - Material Design");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // 添加窗口阴影效果
    QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect();
    windowShadow->setBlurRadius(20);
    windowShadow->setColor(QColor(0, 0, 0, 60));
    windowShadow->setOffset(0, 4);
    centralWidget->setGraphicsEffect(windowShadow);
    
    // 启用现代化动画效果
    setupEnhancedAnimations();
}

void PlayerWindow::setupEnhancedAnimations() {
    // 卡片悬停动画
    QPropertyAnimation *cardHoverAnimation = new QPropertyAnimation();
    cardHoverAnimation->setDuration(200);
    cardHoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // 布局切换动画组
    layoutAnimation = new QParallelAnimationGroup(this);
    
    // 播放按钮发光效果
    glowAnimation = new QPropertyAnimation();
    glowAnimation->setDuration(1000);
    glowAnimation->setEasingCurve(QEasingCurve::InOutSine);
    glowAnimation->setLoopCount(-1);
    
    // 为所有Material按钮添加悬停效果
    if (materialPlayButton) {
        connect(materialPlayButton, &MaterialButton::clicked, [this]() {
            startButtonGlowEffect(materialPlayButton);
        });
    }
    
    // 进度条动画效果
    if (materialProgressBar) {
        materialProgressBar->setAnimationEnabled(true);
    }
}

void PlayerWindow::startButtonGlowEffect(QWidget* button) {
    QPropertyAnimation *glowEffect = new QPropertyAnimation(button, "windowOpacity");
    glowEffect->setDuration(150);
    glowEffect->setStartValue(1.0);
    glowEffect->setEndValue(0.8);
    glowEffect->setEasingCurve(QEasingCurve::OutCubic);
    
    QPropertyAnimation *glowBack = new QPropertyAnimation(button, "windowOpacity");
    glowBack->setDuration(150);
    glowBack->setStartValue(0.8);
    glowBack->setEndValue(1.0);
    glowBack->setEasingCurve(QEasingCurve::InCubic);
    
    QSequentialAnimationGroup *glowSequence = new QSequentialAnimationGroup();
    glowSequence->addAnimation(glowEffect);
    glowSequence->addAnimation(glowBack);
    glowSequence->start(QAbstractAnimation::DeleteWhenStopped);
}
    QLabel#timeLabel {
        font-size: 12px;
        color: #6c757d;
        font-family: 'Courier New', monospace;
    }
    
    /* 歌词可视化控件 */
    LyricsVisualWidget#lyricsWidget {
        background: white;
        border: 1px solid #e9ecef;
        border-radius: 10px;
    }
    )";
    
    setStyleSheet(lightStyle);
    applyModernStyle();
}

void PlayerWindow::switchTheme(bool dark) {
    if (dark) {
        QString darkStyle = R"(
        QMainWindow {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #1a1a1a, stop:1 #2d2d2d);
            color: #ffffff;
        }
        
        QFrame#leftPanel {
            background: rgba(40, 40, 40, 0.95);
            border-right: 1px solid #404040;
        }
        
        QFrame#rightPanel {
            background: rgba(35, 35, 35, 0.9);
        }
        
        QLineEdit#searchEdit {
            background: #404040;
            border: 2px solid #555555;
            color: #ffffff;
        }
        QLineEdit#searchEdit:focus {
            border-color: #bb86fc;
        }
        
        QListWidget#playlistWidget {
            background: #404040;
            border: 1px solid #555555;
            color: #ffffff;
            selection-background-color: #bb86fc;
        }
        QListWidget#playlistWidget::item {
            border-bottom: 1px solid #555555;
        }
        QListWidget#playlistWidget::item:hover {
            background: #4a4a4a;
        }
        
        QFrame#nowPlayingFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #4a4a4a, stop:1 #6a6a6a);
        }
        
        QFrame#controlsFrame {
            background: rgba(40, 40, 40, 0.95);
            border-top: 1px solid #555555;
        }
        
        QPushButton#playPauseButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
                stop:0 #bb86fc, stop:1 #9c4dcc);
        }
        
        QPushButton#previousButton, QPushButton#nextButton,
        QPushButton#playModeButton, QPushButton#volumeButton,
        QPushButton#equalizerButton, QPushButton#themeButton {
            background: rgba(187, 134, 252, 0.1);
            border: 2px solid rgba(187, 134, 252, 0.3);
            color: #bb86fc;
        }
        
        QSlider#progressSlider::groove:horizontal {
            background: #555555;
        }
        QSlider#progressSlider::handle:horizontal {
            background: #bb86fc;
        }
        QSlider#progressSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                stop:0 #bb86fc, stop:1 #cf6679);
        }
        
        QSlider#volumeSlider::groove:horizontal {
            background: #555555;
        }
        QSlider#volumeSlider::handle:horizontal {
            background: #bb86fc;
        }
        QSlider#volumeSlider::sub-page:horizontal {
            background: #bb86fc;
        }
        
        QLabel#timeLabel {
            color: #aaaaaa;
        }
        
        LyricsVisualWidget#lyricsWidget {
            background: #404040;
            border: 1px solid #555555;
        }
        )";
        
        setStyleSheet(darkStyle);
    } else {
        setupStyle();
    }
    
    // 更新歌词可视化控件主题
    if (lyricsVisualWidget) {
        if (dark) {
            lyricsVisualWidget->setDynamicTheme(
                QColor(64, 64, 64),   // 背景色
                QColor(187, 134, 252), // 高亮色
                QColor(255, 255, 255)  // 普通文本色
            );
        } else {
            lyricsVisualWidget->setDynamicTheme(
                QColor(245, 247, 250), // 背景色
                QColor(111, 66, 193),   // 高亮色
                QColor(33, 33, 33)      // 普通文本色
            );
        }
    }
}
void PlayerWindow::loadSong(const QString &audioPath) {
    if (audioPath.isEmpty()) return;
    
    // 加载音频文件
    bool loaded = player->load(audioPath);
    if (!loaded) {
        qDebug() << "Failed to load audio file:" << audioPath;
        return;
    }
    
    // 读取元数据
    SongInfo songInfo = readAudioMeta(audioPath);
    
    // 更新界面信息
    songTitleLabel->setText(songInfo.title.isEmpty() ? QFileInfo(audioPath).completeBaseName() : songInfo.title);
    artistLabel->setText(songInfo.artist.isEmpty() ? "未知艺术家" : songInfo.artist);
    albumLabel->setText(songInfo.album.isEmpty() ? "未知专辑" : songInfo.album);
    
    // 设置专辑封面
    if (!songInfo.cover.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(songInfo.cover);
        albumCoverLabel->setPixmap(pixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        albumCoverLabel->setPixmap(createDefaultAlbumCover());
    }
    
    // 加载歌词
    QString lrcPath = QFileInfo(audioPath).absolutePath() + "/" + QFileInfo(audioPath).completeBaseName() + ".lrc";
    lyricsVisualWidget->loadLrc(lrcPath);
    
    // 生成波形数据
    QVector<float> waveform = extractWaveformFFmpeg(audioPath, 256);
    lyricsVisualWidget->setAudioWaveform(waveform);
    
    // 更新进度条和时间
    totalTimeLabel->setText(formatTime(songInfo.durationMs));
    progressSlider->setRange(0, songInfo.durationMs);
    progressSlider->setValue(0);
    currentTimeLabel->setText("00:00");
    
    // 开始播放
    player->play();
    progressTimer->start();
}

void PlayerWindow::updatePlayModeIcon() {
    QString iconText;
    QString tooltip;
    
    switch (currentPlayMode) {
        case PlayMode::Sequential:
            iconText = "➡️"; // 箭头向右
            tooltip = "顺序播放";
            break;
        case PlayMode::Random:
            iconText = "🔀"; // 随机
            tooltip = "随机播放";
            break;
        case PlayMode::SingleLoop:
            iconText = "🔁"; // 单曲循环
            tooltip = "单曲循环";
            break;
    }
    
    playModeButton->setText(iconText);
    playModeButton->setToolTip(tooltip);
}

void PlayerWindow::updateVolumeIcon(int volume) {
    QString iconText;
    if (volume == 0) {
        iconText = "🔇"; // 静音
    } else if (volume < 30) {
        iconText = "🔈"; // 低音量
    } else if (volume < 70) {
        iconText = "🔉"; // 中音量
    } else {
        iconText = "🔊"; // 高音量
    }
    volumeButton->setText(iconText);
}

void PlayerWindow::addFilesToPlaylist(const QStringList &files) {
    foreach (const QString &file, files) {
        SongInfo info = readAudioMeta(file);
        QString displayName = QString("%1 - %2")
            .arg(info.artist.isEmpty() ? "未知艺术家" : info.artist)
            .arg(info.title.isEmpty() ? QFileInfo(file).completeBaseName() : info.title);
        
        QListWidgetItem *item = new QListWidgetItem(displayName);
        item->setData(Qt::UserRole, file);
        playlistWidget->addItem(item);
        playlist << file;
    }
}

void PlayerWindow::showVolumeSlider(bool show) {
    volumeSlider->setVisible(show);
}

QString PlayerWindow::formatTime(qint64 timeMs) {
    qint64 seconds = timeMs / 1000;
    qint64 minutes = seconds / 60;
    seconds = seconds % 60;
    
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

QPixmap PlayerWindow::createDefaultAlbumCover() {
    QPixmap pixmap(140, 140);
    pixmap.fill(Qt::transparent);
    
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建现代渐变背景
    QLinearGradient gradient(0, 0, 140, 140);
    gradient.setColorAt(0, QColor(103, 58, 183, 200));
    gradient.setColorAt(0.5, QColor(156, 77, 204, 180));
    gradient.setColorAt(1, QColor(187, 134, 252, 160));
    
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0, 0, 140, 140, 12, 12);
    
    // 添加现代音乐图标
    painter.setPen(QPen(Qt::white, 2));
    painter.setFont(QFont("Segoe UI Emoji", 36, QFont::Bold));
    painter.drawText(QRect(0, 0, 140, 140), Qt::AlignCenter, "🎵");
    
    // 添加光晕效果
    QRadialGradient highlight(70, 35, 40);
    highlight.setColorAt(0, QColor(255, 255, 255, 100));
    highlight.setColorAt(1, QColor(255, 255, 255, 0));
    
    painter.setBrush(highlight);
    painter.drawEllipse(30, 10, 80, 50);
    
    return pixmap;
}

void PlayerWindow::setupStyle() {
    // 设置默认（浅色）主题样式
    QString lightStyle = R"(
    /* 主窗口样式 */
    QMainWindow {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
            stop:0 #f8f9fa, stop:1 #e9ecef);
        color: #212529;
    }
    
    /* 左侧面板 */
    QFrame#leftPanel {
        background: rgba(255, 255, 255, 0.95);
        border-right: 1px solid #dee2e6;
    }
    
    /* 右侧面板 */
    QFrame#rightPanel {
        background: rgba(255, 255, 255, 0.9);
    }
    
    /* 搜索框 */
    QLineEdit#searchEdit {
        background: white;
        border: 2px solid #e9ecef;
        border-radius: 20px;
        padding: 10px 15px;
        font-size: 14px;
        color: #495057;
    }
    QLineEdit#searchEdit:focus {
        border-color: #6f42c1;
        outline: none;
    }
    
    /* 播放列表 */
    QListWidget#playlistWidget {
        background: white;
        border: 1px solid #e9ecef;
        border-radius: 10px;
        outline: none;
    }
    QListWidget#playlistWidget::item {
        border-bottom: 1px solid #f1f3f5;
        padding: 12px;
        font-size: 14px;
    }
    QListWidget#playlistWidget::item:hover {
        background: rgba(111, 66, 193, 0.1);
    }
    QListWidget#playlistWidget::item:selected {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
            stop:0 #6f42c1, stop:1 #8b5fbf);
        color: white;
    }
    
    /* 正在播放区域 */
    QFrame#nowPlayingFrame {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
            stop:0 #ffffff, stop:1 #f8f9fa);
        border-radius: 15px;
        border: 1px solid #e9ecef;
    }
    
    /* 控制面板 */
    QFrame#controlsFrame {
        background: rgba(255, 255, 255, 0.95);
        border-top: 1px solid #e9ecef;
    }
    
    /* 播放控制按钮 */
    QPushButton#playPauseButton {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
            stop:0 #6f42c1, stop:1 #5a32a3);
        border: none;
        border-radius: 25px;
        color: white;
        font-size: 16px;
        font-weight: bold;
    }
    QPushButton#playPauseButton:hover {
        background: qlineargradient(x1:0, y1:0, x2:0, y2:1, 
            stop:0 #7952cc, stop:1 #6439b8);
    }
    QPushButton#playPauseButton:pressed {
        background: #5a32a3;
    }
    
    QPushButton#previousButton, QPushButton#nextButton,
    QPushButton#playModeButton, QPushButton#volumeButton,
    QPushButton#equalizerButton, QPushButton#themeButton {
        background: rgba(111, 66, 193, 0.1);
        border: 2px solid rgba(111, 66, 193, 0.3);
        border-radius: 20px;
        color: #6f42c1;
        font-size: 14px;
    }
    QPushButton#previousButton:hover, QPushButton#nextButton:hover,
    QPushButton#playModeButton:hover, QPushButton#volumeButton:hover,
    QPushButton#equalizerButton:hover, QPushButton#themeButton:hover {
        background: rgba(111, 66, 193, 0.2);
        border-color: #6f42c1;
    }
    
    /* 进度滑块 */
    QSlider#progressSlider {
        height: 8px;
    }
    QSlider#progressSlider::groove:horizontal {
        background: #e9ecef;
        height: 6px;
        border-radius: 3px;
    }
    QSlider#progressSlider::handle:horizontal {
        background: #6f42c1;
        width: 16px;
        height: 16px;
        border-radius: 8px;
        margin-top: -5px;
        margin-bottom: -5px;
    }
    QSlider#progressSlider::sub-page:horizontal {
        background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
            stop:0 #6f42c1, stop:1 #8b5fbf);
        border-radius: 3px;
    }
    
    /* 音量滑块 */
    QSlider#volumeSlider {
        height: 6px;
    }
    QSlider#volumeSlider::groove:horizontal {
        background: #e9ecef;
        height: 4px;
        border-radius: 2px;
    }
    QSlider#volumeSlider::handle:horizontal {
        background: #6f42c1;
        width: 12px;
        height: 12px;
        border-radius: 6px;
        margin-top: -4px;
        margin-bottom: -4px;
    }
    QSlider#volumeSlider::sub-page:horizontal {
        background: #6f42c1;
        border-radius: 2px;
    }
    
    /* 时间标签 */
    QLabel#timeLabel {
        font-size: 12px;
        color: #6c757d;
        font-family: 'Courier New', monospace;
    }
    
    /* 歌词可视化控件 */
    LyricsVisualWidget#lyricsWidget {
        background: white;
        border: 1px solid #e9ecef;
        border-radius: 10px;
    }
    )";
    
    setStyleSheet(lightStyle);
    applyModernStyle();
}

void PlayerWindow::applyModernStyle() {
    // 现代化样式将在 setupStyle 中实现
}

void PlayerWindow::setupEqualizer() {
    // 创建均衡器窗口
    equalizerWindow = new QWidget();
    equalizerWindow->setWindowTitle("音频均衡器");
    equalizerWindow->setFixedSize(400, 300);
    equalizerWindow->setWindowModality(Qt::ApplicationModal);
    
    QVBoxLayout *layout = new QVBoxLayout(equalizerWindow);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);
    
    // 均衡器标题
    QLabel *titleLabel = new QLabel("10频段均衡器");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #333;");
    layout->addWidget(titleLabel);
    
    // 均衡器滑块容器
    QFrame *eqFrame = new QFrame();
    QHBoxLayout *eqLayout = new QHBoxLayout(eqFrame);
    eqLayout->setContentsMargins(10, 10, 10, 10);
    eqLayout->setSpacing(10);
    
    // 创建10个频段滑块
    QStringList frequencies = {"32Hz", "64Hz", "125Hz", "250Hz", "500Hz", 
                              "1kHz", "2kHz", "4kHz", "8kHz", "16kHz"};
    
    for (int i = 0; i < 10; i++) {
        QVBoxLayout *sliderLayout = new QVBoxLayout();
        sliderLayout->setSpacing(5);
        
        QLabel *freqLabel = new QLabel(frequencies[i]);
        freqLabel->setAlignment(Qt::AlignCenter);
        freqLabel->setStyleSheet("font-size: 10px; color: #666;");
        
        eqSliders[i] = new QSlider(Qt::Vertical);
        eqSliders[i]->setRange(-12, 12);
        eqSliders[i]->setValue(0);
        eqSliders[i]->setFixedHeight(150);
        
        QLabel *valueLabel = new QLabel("0dB");
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setStyleSheet("font-size: 10px; color: #333;");
        
        // 连接滑块值变化信号
        connect(eqSliders[i], &QSlider::valueChanged, [valueLabel](int value) {
            valueLabel->setText(QString("%1dB").arg(value));
        });
        
        sliderLayout->addWidget(freqLabel);
        sliderLayout->addWidget(eqSliders[i]);
        sliderLayout->addWidget(valueLabel);
        
        eqLayout->addLayout(sliderLayout);
    }
    
    layout->addWidget(eqFrame);
    
    // 预设和控制按钮
    QFrame *controlFrame = new QFrame();
    QHBoxLayout *controlLayout = new QHBoxLayout(controlFrame);
    
    QPushButton *resetButton = new QPushButton("重置");
    QPushButton *closeButton = new QPushButton("关闭");
    
    connect(resetButton, &QPushButton::clicked, [this]() {
        for (int i = 0; i < 10; i++) {
            eqSliders[i]->setValue(0);
        }
    });
    
    connect(closeButton, &QPushButton::clicked, [this]() {
        equalizerWindow->hide();
    });
    
    controlLayout->addWidget(resetButton);
    controlLayout->addStretch();
    controlLayout->addWidget(closeButton);
    
    layout->addWidget(controlFrame);
}
