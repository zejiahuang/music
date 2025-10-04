#pragma once
#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include <QMutex>
#include <QThread>
#include <QAudioFormat>
#include <QJsonObject>
#include <complex>

/**
 * 音频效果处理器基类
 * 所有音频效果的抽象基类
 */
class AudioEffect : public QObject {
    Q_OBJECT

public:
    enum EffectType {
        Reverb,         // 混响
        Echo,           // 回声
        Chorus,         // 合唱
        Flanger,        // 镶边器
        Phaser,         // 相位器
        Distortion,     // 失真
        Compression,    // 压缩
        Limiter,        // 限幅器
        NoiseGate,      // 噪音门
        Tremolo,        // 颤音
        Vibrato,        // 振音
        BitCrusher,     // 位压缩
        FilterLowPass,  // 低通滤波
        FilterHighPass, // 高通滤波
        FilterBandPass, // 带通滤波
        FilterNotch,    // 陷波滤波
        Equalizer,      // 均衡器
        Stereoizer,     // 立体声增强
        Spatializer     // 空间化
    };

    explicit AudioEffect(EffectType type, QObject *parent = nullptr);
    virtual ~AudioEffect() = default;

    // 基本接口
    virtual void process(QVector<float> &audioData, int channels, int sampleRate) = 0;
    virtual void reset() = 0;
    
    // 效果控制
    virtual void setEnabled(bool enabled);
    bool isEnabled() const { return m_enabled; }
    
    virtual void setParameter(const QString &name, float value);
    virtual float getParameter(const QString &name) const;
    virtual QStringList getParameterNames() const;
    
    // 预设管理
    virtual void loadPreset(const QString &presetName);
    virtual void savePreset(const QString &presetName);
    virtual QStringList getPresetNames() const;
    
    // 效果信息
    EffectType effectType() const { return m_effectType; }
    QString effectName() const;
    
    // 序列化
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject &json);

signals:
    void parameterChanged(const QString &name, float value);
    void enabledChanged(bool enabled);

protected:
    void setParameterInternal(const QString &name, float value);
    
    EffectType m_effectType;
    bool m_enabled;
    QMap<QString, float> m_parameters;
    QMap<QString, QJsonObject> m_presets;
};

/**
 * 混响效果器
 * 实现高质量的混响效果
 */
class ReverbEffect : public AudioEffect {
    Q_OBJECT

public:
    explicit ReverbEffect(QObject *parent = nullptr);
    
    void process(QVector<float> &audioData, int channels, int sampleRate) override;
    void reset() override;
    
    // 混响参数
    void setRoomSize(float size);        // 房间大小 0-1
    void setDamping(float damping);      // 阻尼 0-1
    void setWetLevel(float level);       // 湿信号电平 0-1
    void setDryLevel(float level);       // 干信号电平 0-1
    void setPreDelay(float delay);       // 预延迟 ms
    void setDecayTime(float time);       // 衰减时间 s

private:
    void initializeDelayLines();
    void processAllPass(QVector<float> &data, int channels);
    void processComb(QVector<float> &data, int channels);
    
    // 延迟线
    struct DelayLine {
        QVector<float> buffer;
        int writePos;
        int readPos;
        float feedback;
        float gain;
    };
    
    QVector<DelayLine> m_allPassDelays;
    QVector<DelayLine> m_combDelays;
    
    // 混响参数
    float m_roomSize;
    float m_damping;
    float m_wetLevel;
    float m_dryLevel;
    float m_preDelay;
    float m_decayTime;
    
    // 内部状态
    int m_sampleRate;
    QVector<float> m_preDelayBuffer;
    int m_preDelayPos;
};

/**
 * 回声效果器
 * 实现多重回声效果
 */
class EchoEffect : public AudioEffect {
    Q_OBJECT

public:
    explicit EchoEffect(QObject *parent = nullptr);
    
    void process(QVector<float> &audioData, int channels, int sampleRate) override;
    void reset() override;
    
    // 回声参数
    void setDelayTime(float time);       // 延迟时间 ms
    void setFeedback(float feedback);    // 反馈量 0-1
    void setWetLevel(float level);       // 湿信号电平 0-1
    void setDryLevel(float level);       // 干信号电平 0-1
    void setNumEchoes(int count);        // 回声数量
    void setEchoSpread(float spread);    // 回声间隔

private:
    struct EchoTap {
        QVector<float> delayBuffer;
        int bufferSize;
        int writePos;
        float delayTime;
        float gain;
    };
    
    QVector<EchoTap> m_echoTaps;
    
    float m_delayTime;
    float m_feedback;
    float m_wetLevel;
    float m_dryLevel;
    int m_numEchoes;
    float m_echoSpread;
    
    int m_sampleRate;
};

/**
 * 合唱效果器
 * 实现丰富的合唱效果
 */
class ChorusEffect : public AudioEffect {
    Q_OBJECT

public:
    explicit ChorusEffect(QObject *parent = nullptr);
    
    void process(QVector<float> &audioData, int channels, int sampleRate) override;
    void reset() override;
    
    // 合唱参数
    void setRate(float rate);            // LFO频率 Hz
    void setDepth(float depth);          // 调制深度 0-1
    void setDelay(float delay);          // 基础延迟 ms
    void setFeedback(float feedback);    // 反馈量 0-1
    void setWetLevel(float level);       // 湿信号电平 0-1
    void setDryLevel(float level);       // 干信号电平 0-1
    void setVoices(int voices);          // 声部数量

private:
    struct ChorusVoice {
        QVector<float> delayBuffer;
        int bufferSize;
        int writePos;
        float phase;
        float gain;
        float detune;
    };
    
    QVector<ChorusVoice> m_voices;
    
    float m_rate;
    float m_depth;
    float m_delay;
    float m_feedback;
    float m_wetLevel;
    float m_dryLevel;
    int m_numVoices;
    
    int m_sampleRate;
    float m_lfoPhase;
};

/**
 * 动态范围处理器
 * 包括压缩器、限幅器、门限器等
 */
class DynamicsProcessor : public AudioEffect {
    Q_OBJECT

public:
    enum ProcessorType {
        Compressor,
        Limiter,
        Gate,
        Expander
    };

    explicit DynamicsProcessor(ProcessorType type, QObject *parent = nullptr);
    
    void process(QVector<float> &audioData, int channels, int sampleRate) override;
    void reset() override;
    
    // 动态处理参数
    void setThreshold(float threshold);  // 阈值 dB
    void setRatio(float ratio);          // 压缩比
    void setAttack(float attack);        // 起音时间 ms
    void setRelease(float release);      // 释音时间 ms
    void setKnee(float knee);            // 拐点 dB
    void setMakeupGain(float gain);      // 补偿增益 dB
    void setLookahead(float time);       // 前瞻时间 ms

private:
    float calculateGainReduction(float inputLevel);
    float dbToLinear(float db);
    float linearToDb(float linear);
    
    ProcessorType m_processorType;
    
    float m_threshold;
    float m_ratio;
    float m_attack;
    float m_release;
    float m_knee;
    float m_makeupGain;
    float m_lookahead;
    
    // 内部状态
    float m_envelope;
    QVector<float> m_lookaheadBuffer;
    int m_lookaheadSamples;
    int m_lookaheadPos;
    int m_sampleRate;
};

/**
 * 多频段均衡器
 * 支持多个频段的精确控制
 */
class MultibandEqualizer : public AudioEffect {
    Q_OBJECT

public:
    struct EQBand {
        enum FilterType {
            LowPass,
            HighPass,
            BandPass,
            BandStop,
            LowShelf,
            HighShelf,
            Peak
        };
        
        FilterType type;
        float frequency;    // 中心频率 Hz
        float gain;         // 增益 dB
        float q;            // Q值
        bool enabled;
        
        QJsonObject toJson() const;
        static EQBand fromJson(const QJsonObject &json);
    };

    explicit MultibandEqualizer(QObject *parent = nullptr);
    
    void process(QVector<float> &audioData, int channels, int sampleRate) override;
    void reset() override;
    
    // 频段操作
    void addBand(const EQBand &band);
    void removeBand(int index);
    void updateBand(int index, const EQBand &band);
    EQBand getBand(int index) const;
    int getBandCount() const;
    
    // 预设频段配置
    void setupStandardBands();          // 标准10频段
    void setupGraphicEqualizer();       // 图形均衡器
    void setupParametricEqualizer();    // 参数均衡器

private:
    struct BiquadFilter {
        float b0, b1, b2;  // 前馈系数
        float a1, a2;      // 反馈系数
        float x1, x2;      // 输入延迟
        float y1, y2;      // 输出延迟
        
        void reset();
        float process(float input);
        void calculateCoefficients(const EQBand &band, int sampleRate);
    };
    
    QVector<EQBand> m_bands;
    QVector<QVector<BiquadFilter>> m_filters; // [band][channel]
    
    int m_sampleRate;
    int m_channels;
};

/**
 * 空间音效处理器
 * 实现3D定位和环绕声效果
 */
class SpatialAudioProcessor : public AudioEffect {
    Q_OBJECT

public:
    struct AudioSource {
        float x, y, z;          // 3D位置
        float gainLeft, gainRight; // 立体声增益
        float distance;         // 距离
        float azimuth;          // 方位角
        float elevation;        // 仰角
        bool enabled;
    };

    explicit SpatialAudioProcessor(QObject *parent = nullptr);
    
    void process(QVector<float> &audioData, int channels, int sampleRate) override;
    void reset() override;
    
    // 空间定位
    void setListenerPosition(float x, float y, float z);
    void setListenerOrientation(float yaw, float pitch, float roll);
    void setSourcePosition(float x, float y, float z);
    
    // HRTF处理
    void enableHRTF(bool enabled);
    void loadHRTFData(const QString &filePath);
    
    // 环绕声
    void setSurroundMode(bool enabled);
    void setRoomSimulation(bool enabled);

private:
    void calculateStereoPosition();
    void applyHRTF(QVector<float> &audioData);
    void applyCrossfeed(QVector<float> &audioData);
    
    AudioSource m_source;
    float m_listenerX, m_listenerY, m_listenerZ;
    float m_listenerYaw, m_listenerPitch, m_listenerRoll;
    
    bool m_hrtfEnabled;
    bool m_surroundEnabled;
    bool m_roomSimEnabled;
    
    QVector<QVector<float>> m_hrtfLeft;
    QVector<QVector<float>> m_hrtfRight;
    
    int m_sampleRate;
};

/**
 * 音频效果链管理器
 * 管理多个音频效果的处理链
 */
class AudioEffectChain : public QObject {
    Q_OBJECT

public:
    explicit AudioEffectChain(QObject *parent = nullptr);
    ~AudioEffectChain();
    
    // 效果链操作
    void addEffect(AudioEffect *effect);
    void insertEffect(int index, AudioEffect *effect);
    void removeEffect(int index);
    void removeEffect(AudioEffect *effect);
    void moveEffect(int from, int to);
    void clearEffects();
    
    // 效果控制
    void setEffectEnabled(int index, bool enabled);
    void setEffectParameter(int index, const QString &parameter, float value);
    
    // 链处理
    void processAudio(QVector<float> &audioData, int channels, int sampleRate);
    void reset();
    
    // 链管理
    int getEffectCount() const;
    AudioEffect* getEffect(int index) const;
    QVector<AudioEffect*> getAllEffects() const;
    
    // 预设管理
    void saveChainPreset(const QString &name);
    void loadChainPreset(const QString &name);
    QStringList getChainPresets() const;
    
    // 序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);

signals:
    void effectAdded(int index, AudioEffect *effect);
    void effectRemoved(int index);
    void effectMoved(int from, int to);
    void chainChanged();

private:
    QVector<AudioEffect*> m_effects;
    QMutex m_effectsMutex;
    QMap<QString, QJsonObject> m_chainPresets;
};
