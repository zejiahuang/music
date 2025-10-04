#pragma once
#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include <QTimer>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../include/songinfo.h"

/**
 * 音乐特征分析器
 * 分析音频文件的各种特征用于推荐算法
 */
class MusicAnalyzer : public QObject {
    Q_OBJECT

public:
    struct AudioFeatures {
        double tempo;           // BPM
        double energy;          // 能量 0-1
        double valence;         // 情感倾向 0-1 (sad-happy)
        double danceability;    // 可舞性 0-1
        double acousticness;    // 声学性 0-1
        double instrumentalness; // 器乐性 0-1
        double loudness;        // 响度 dB
        double speechiness;     // 语音性 0-1
        QString key;            // 音调
        QString mode;           // 调式 (major/minor)
        int duration_ms;        // 时长毫秒
        
        QJsonObject toJson() const;
        static AudioFeatures fromJson(const QJsonObject &obj);
    };

    explicit MusicAnalyzer(QObject *parent = nullptr);
    
    // 分析音频文件特征
    AudioFeatures analyzeFile(const QString &filePath);
    
    // 批量分析
    void analyzeLibrary(const QStringList &filePaths);
    
    // 获取相似度 (0-1)
    double calculateSimilarity(const AudioFeatures &f1, const AudioFeatures &f2);

signals:
    void analysisCompleted(const QString &filePath, const AudioFeatures &features);
    void libraryAnalysisProgress(int current, int total);
    void libraryAnalysisCompleted();

private:
    QMap<QString, AudioFeatures> m_featuresCache;
    QMutex m_cacheMutex;
    
    // 内部分析方法
    static double extractTempo(const QString &filePath);
    static double extractEnergy(const QString &filePath);
    static double extractValence(const QString &filePath);
};

/**
 * 智能推荐引擎
 * 基于音乐特征、用户行为等进行推荐
 */
class RecommendationEngine : public QObject {
    Q_OBJECT

public:
    enum RecommendationType {
        SimilarSongs,       // 相似歌曲
        MoodBased,          // 基于心情
        GenreBased,         // 基于流派
        TimeBased,          // 基于时间
        ActivityBased,      // 基于活动
        DiscoveryMix        // 发现新音乐
    };

    struct UserPreference {
        QMap<QString, double> genreWeights;      // 流派偏好权重
        QMap<QString, double> artistWeights;     // 艺术家偏好权重
        QMap<QString, double> moodWeights;       // 心情偏好权重
        QMap<int, double> timeWeights;           // 时间偏好权重 (小时)
        double energyPreference;                 // 能量偏好
        double valencePreference;                // 情感偏好
        
        QJsonObject toJson() const;
        static UserPreference fromJson(const QJsonObject &obj);
    };

    struct PlayHistory {
        QString songId;
        qint64 timestamp;
        qint64 playDuration;    // 播放时长
        bool skipped;           // 是否跳过
        bool liked;             // 是否喜欢
        double completionRate;  // 完成率 0-1
        
        QJsonObject toJson() const;
        static PlayHistory fromJson(const QJsonObject &obj);
    };

    explicit RecommendationEngine(QObject *parent = nullptr);
    
    // 设置音乐分析器
    void setMusicAnalyzer(MusicAnalyzer *analyzer);
    
    // 学习用户偏好
    void updateUserPreference(const PlayHistory &history, const SongInfo &song);
    void learnFromPlaylist(const QVector<SongInfo> &playlist);
    
    // 生成推荐
    QVector<SongInfo> recommend(RecommendationType type, 
                               const SongInfo &baseSong = SongInfo(),
                               int count = 20);
    
    // 获取推荐播放列表
    QVector<SongInfo> generateSmartPlaylist(const QString &mood, 
                                           const QString &activity,
                                           int duration_minutes = 60);
    
    // 保存/加载用户偏好
    void saveUserPreference(const QString &filePath);
    void loadUserPreference(const QString &filePath);
    
    // 获取当前用户偏好
    UserPreference getUserPreference() const { return m_userPreference; }

signals:
    void recommendationReady(const QVector<SongInfo> &songs);
    void userPreferenceUpdated();

private:
    MusicAnalyzer *m_analyzer;
    UserPreference m_userPreference;
    QVector<PlayHistory> m_playHistory;
    QVector<SongInfo> m_musicLibrary;
    
    // 推荐算法
    QVector<SongInfo> recommendSimilar(const SongInfo &baseSong, int count);
    QVector<SongInfo> recommendByMood(const QString &mood, int count);
    QVector<SongInfo> recommendByTime(int count);
    QVector<SongInfo> recommendByActivity(const QString &activity, int count);
    QVector<SongInfo> recommendDiscovery(int count);
    
    // 评分计算
    double calculateSongScore(const SongInfo &song, const SongInfo &baseSong = SongInfo());
    double calculateMoodScore(const SongInfo &song, const QString &mood);
    double calculateTimeScore(const SongInfo &song);
    double calculateActivityScore(const SongInfo &song, const QString &activity);
    
    // 工具方法
    void updateGenreWeight(const QString &genre, double weight);
    void updateArtistWeight(const QString &artist, double weight);
    QString detectMood(const MusicAnalyzer::AudioFeatures &features);
    QString detectActivity(const MusicAnalyzer::AudioFeatures &features);
};

/**
 * 智能播放列表管理器
 * 自动生成和管理各种智能播放列表
 */
class SmartPlaylistManager : public QObject {
    Q_OBJECT

public:
    struct SmartPlaylist {
        QString id;
        QString name;
        QString description;
        QString iconPath;
        RecommendationEngine::RecommendationType type;
        QMap<QString, QString> parameters;  // 播放列表参数
        QVector<SongInfo> songs;
        qint64 lastUpdated;
        bool autoUpdate;
        
        QJsonObject toJson() const;
        static SmartPlaylist fromJson(const QJsonObject &obj);
    };

    explicit SmartPlaylistManager(QObject *parent = nullptr);
    
    // 设置推荐引擎
    void setRecommendationEngine(RecommendationEngine *engine);
    
    // 创建智能播放列表
    QString createSmartPlaylist(const QString &name, 
                               RecommendationEngine::RecommendationType type,
                               const QMap<QString, QString> &parameters = QMap<QString, QString>());
    
    // 预定义智能播放列表
    void createDefaultPlaylists();
    
    // 更新播放列表
    void updatePlaylist(const QString &playlistId);
    void updateAllPlaylists();
    
    // 获取播放列表
    QVector<SmartPlaylist> getAllPlaylists() const;
    SmartPlaylist getPlaylist(const QString &playlistId) const;
    
    // 播放列表操作
    void deletePlaylist(const QString &playlistId);
    void setAutoUpdate(const QString &playlistId, bool autoUpdate);
    
    // 保存/加载
    void savePlaylists(const QString &filePath);
    void loadPlaylists(const QString &filePath);

signals:
    void playlistCreated(const QString &playlistId);
    void playlistUpdated(const QString &playlistId);
    void playlistDeleted(const QString &playlistId);
    void allPlaylistsUpdated();

private slots:
    void onAutoUpdateTimer();

private:
    RecommendationEngine *m_engine;
    QMap<QString, SmartPlaylist> m_playlists;
    QTimer *m_autoUpdateTimer;
    
    // 生成播放列表ID
    QString generatePlaylistId();
    
    // 预定义播放列表创建方法
    void createDiscoveryWeekly();
    void createMoodPlaylists();
    void createActivityPlaylists();
    void createTimePlaylists();
};

/**
 * 在线音乐服务集成
 * 集成各种在线音乐服务API
 */
class OnlineMusicService : public QObject {
    Q_OBJECT

public:
    struct OnlineTrack {
        QString id;
        QString title;
        QString artist;
        QString album;
        QString previewUrl;
        QString albumArtUrl;
        int duration_ms;
        bool isPlayable;
        
        QJsonObject toJson() const;
        static OnlineTrack fromJson(const QJsonObject &obj);
    };

    enum ServiceType {
        Spotify,
        AppleMusic,
        YouTubeMusic,
        LastFM,
        Deezer
    };

    explicit OnlineMusicService(QObject *parent = nullptr);
    
    // 配置服务
    void configureService(ServiceType type, const QString &apiKey, const QString &secret = "");
    
    // 搜索音乐
    void searchTracks(const QString &query, int limit = 20);
    void searchArtist(const QString &artist);
    void searchAlbum(const QString &album);
    
    // 获取推荐
    void getRecommendations(const QStringList &seedTracks, int limit = 20);
    void getTrendingTracks(const QString &genre = "", int limit = 50);
    
    // 获取歌词
    void getLyrics(const QString &artist, const QString &title);
    
    // 获取专辑封面
    void getAlbumArt(const QString &artist, const QString &album, const QSize &size = QSize(300, 300));

signals:
    void searchCompleted(const QVector<OnlineTrack> &tracks);
    void recommendationsReady(const QVector<OnlineTrack> &tracks);
    void lyricsReceived(const QString &artist, const QString &title, const QString &lyrics);
    void albumArtReceived(const QString &artist, const QString &album, const QPixmap &art);
    void errorOccurred(const QString &error);

private slots:
    void onNetworkReply();

private:
    QNetworkAccessManager *m_networkManager;
    QMap<ServiceType, QString> m_apiKeys;
    QMap<ServiceType, QString> m_apiSecrets;
    QMap<QNetworkReply*, QString> m_pendingRequests;
    
    // API调用方法
    void makeApiRequest(const QString &url, const QMap<QString, QString> &headers = QMap<QString, QString>());
    QString buildSpotifyUrl(const QString &endpoint, const QMap<QString, QString> &params = QMap<QString, QString>());
    QString buildLastFMUrl(const QString &method, const QMap<QString, QString> &params = QMap<QString, QString>());
    
    // 数据解析
    QVector<OnlineTrack> parseSpotifyTracks(const QJsonObject &response);
    QString parseLastFMLyrics(const QJsonObject &response);
};
