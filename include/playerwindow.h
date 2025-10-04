#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QSplitter>
#include <QComboBox>
#include <QProgressBar>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QStackedWidget>
#include <QScrollArea>
#include <QGroupBox>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QDrag>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include "../src/ui/lyricsvisualwidget.h"
#include "ffmpegplayer.h"
#include "materialui_components.h"

enum class PlayMode {
    Sequential,
    Random,
    SingleLoop
};

class PlayerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit PlayerWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void playPause();
    void previousTrack();
    void nextTrack();
    void seekToPosition(int position);
    void setVolume(int volume);
    void changePlayMode();
    void updateProgress();
    void onTrackChanged();
    void onPlaylistItemClicked();
    void onSearchTextChanged();
    void showEqualizer();
    void toggleTheme();

private:
    // UI Components
    QWidget *centralWidget;
    QSplitter *mainSplitter;
    
    // Left Panel - Playlist and Controls
    QFrame *leftPanel;
    QLineEdit *searchEdit;
    QListWidget *playlistWidget;
    
    // Right Panel - Now Playing and Visualizer
    QFrame *rightPanel;
    LyricsVisualWidget *lyricsVisualWidget;
    
    // Album Cover and Info
    QLabel *albumCoverLabel;
    QLabel *songTitleLabel;
    QLabel *artistLabel;
    QLabel *albumLabel;
    
    // Transport Controls
    QFrame *controlsFrame;
    QPushButton *playPauseButton;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QPushButton *playModeButton;
    QPushButton *volumeButton;
    QPushButton *equalizerButton;
    QPushButton *themeButton;
    
    // Progress and Volume
    QSlider *progressSlider;
    QSlider *volumeSlider;
    QLabel *currentTimeLabel;
    QLabel *totalTimeLabel;
    QFrame *volumeFrame;
    
    // Equalizer Window
    QWidget *equalizerWindow;
    QSlider *eqSliders[10];
    
    // Core functionality
    FFmpegPlayer *player;
    QTimer *progressTimer;
    PlayMode currentPlayMode;
    bool isDarkTheme;
    int currentTrackIndex;
    QStringList playlist;
    
    // Animation and Effects
    QPropertyAnimation *volumeAnimation;
    QGraphicsDropShadowEffect *shadowEffect;
    
    // Material Design Components
    MaterialCard *playlistCard;
    MaterialCard *nowPlayingCard;
    MaterialCard *controlsCard;
    MaterialButton *materialPlayButton;
    MaterialButton *materialNextButton;
    MaterialButton *materialPrevButton;
    MaterialButton *materialVolumeButton;
    MaterialButton *materialThemeButton;
    AdvancedProgressBar *materialProgressBar;
    
    // Layout and Visual Effects
    QPropertyAnimation *cardAnimation;
    QPropertyAnimation *glowAnimation;
    QParallelAnimationGroup *layoutAnimation;
    
    // Setup methods
    void setupUi();
    void setupMaterialCards();
    void setupLeftPanel();
    void setupRightPanel();
    void setupMaterialControls();
    void setupControls();
    void setupEqualizer();
    void setupStyle();
    void setupAnimations();
    void setupConnections();
    void applyEnhancedMaterialStyle();
    void setupEnhancedAnimations();
    void startButtonGlowEffect(QWidget* button);
    
    // Utility methods
    void loadSong(const QString &audioPath);
    void switchTheme(bool dark);
    void updatePlayModeIcon();
    void updateVolumeIcon(int volume);
    void addFilesToPlaylist(const QStringList &files);
    void showVolumeSlider(bool show);
    QString formatTime(qint64 timeMs);
    void applyModernStyle();
    QPixmap createDefaultAlbumCover();
};
