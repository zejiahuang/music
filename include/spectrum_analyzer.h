#pragma once
#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QColor>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsEffect>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <complex>

/**
 * 高级频谱分析器
 * 支持多种可视化效果和实时音频分析
 */
class SpectrumAnalyzer : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
    Q_PROPERTY(qreal intensity READ intensity WRITE setIntensity)

public:
    enum VisualizationType {
        Bars,           // 传统条形图
        Waves,          // 波形
        Circular,       // 圆形频谱
        Particles,      // 粒子效果
        Galaxy,         // 星系效果
        Liquid,         // 液体效果
        Neural,         // 神经网络风格
        Matrix          // 矩阵雨效果
    };

    enum ColorScheme {
        Classic,        // 经典
        Neon,          // 霓虹
        Fire,          // 火焰
        Ocean,         // 海洋
        Aurora,        // 极光
        Cyberpunk,     // 赛博朋克
        Gradient       // 渐变
    };

    explicit SpectrumAnalyzer(QWidget *parent = nullptr);
    ~SpectrumAnalyzer();

    // 设置可视化类型
    void setVisualizationType(VisualizationType type);
    VisualizationType visualizationType() const { return m_visualizationType; }

    // 设置颜色方案
    void setColorScheme(ColorScheme scheme);
    ColorScheme colorScheme() const { return m_colorScheme; }

    // 设置频谱数据
    void setSpectrumData(const QVector<float> &data);
    void setWaveformData(const QVector<float> &data);
    
    // 强度控制
    void setIntensity(qreal intensity);
    qreal intensity() const { return m_intensity; }
    
    // 设置频段数量
    void setBandCount(int count);
    int bandCount() const { return m_bandCount; }
    
    // 平滑设置
    void setSmoothingEnabled(bool enabled);
    void setSmoothingFactor(qreal factor);
    
    // 3D效果
    void set3DEnabled(bool enabled);
    void setRotationSpeed(qreal speed);
    
    // 响应性设置
    void setResponseSpeed(qreal speed);
    void setPeakDecayRate(qreal rate);

public slots:
    void startAnimation();
    void stopAnimation();
    void resetAnimation();

signals:
    void beatDetected(qreal intensity);
    void frequencyPeakDetected(int frequency, qreal amplitude);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    
private slots:
    void updateAnimation();
    void updateSpectrum();

private:
    // OpenGL相关
    void initializeShaders();
    void setupBuffers();
    void cleanupGL();
    
    // 渲染方法
    void renderBars();
    void renderWaves();
    void renderCircular();
    void renderParticles();
    void renderGalaxy();
    void renderLiquid();
    void renderNeural();
    void renderMatrix();
    
    // 工具方法
    void updatePeaks();
    void applySmoothing();
    void detectBeats();
    QVector<QColor> generateColorGradient(ColorScheme scheme, int count);
    
    // 成员变量
    VisualizationType m_visualizationType;
    ColorScheme m_colorScheme;
    
    QVector<float> m_spectrumData;
    QVector<float> m_waveformData;
    QVector<float> m_smoothedData;
    QVector<float> m_peakData;
    QVector<float> m_previousData;
    
    qreal m_intensity;
    int m_bandCount;
    bool m_smoothingEnabled;
    qreal m_smoothingFactor;
    bool m_3dEnabled;
    qreal m_rotationSpeed;
    qreal m_responseSpeed;
    qreal m_peakDecayRate;
    
    // 动画
    QTimer *m_animationTimer;
    QTimer *m_spectrumTimer;
    qreal m_animationPhase;
    qreal m_rotationAngle;
    
    // OpenGL资源
    QOpenGLShaderProgram *m_shaderProgram;
    QOpenGLBuffer *m_vertexBuffer;
    QOpenGLBuffer *m_colorBuffer;
    unsigned int m_VAO;
    
    // 效果参数
    QVector<QVector3D> m_particles;
    QVector<QVector3D> m_particleVelocities;
    QVector<QColor> m_colors;
    
    // 节拍检测
    qreal m_lastBeatTime;
    qreal m_energySum;
    QVector<qreal> m_energyHistory;
};

/**
 * 3D音频可视化器
 * 使用OpenGL实现高性能3D音频可视化
 */
class Audio3DVisualizer : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    enum Effect3D {
        Tunnel,         // 隧道效果
        Sphere,         // 球体
        Cube,          // 立方体
        Torus,         // 环面
        Helix,         // 螺旋
        Mandala,       // 曼陀罗
        DNA,           // DNA螺旋
        Fractal        // 分形
    };

    explicit Audio3DVisualizer(QWidget *parent = nullptr);
    ~Audio3DVisualizer();

    void setEffect(Effect3D effect);
    void setAudioData(const QVector<float> &spectrum, const QVector<float> &waveform);
    
    // 相机控制
    void setCameraDistance(float distance);
    void setCameraRotation(float pitch, float yaw);
    void setAutoRotation(bool enabled);
    
    // 材质设置
    void setMaterialProperties(float metallic, float roughness, float emission);
    void enableBloom(bool enabled);
    void enableParticles(bool enabled);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void setupMatrices();
    void renderEffect();
    void updateLighting();
    
    Effect3D m_currentEffect;
    QVector<float> m_spectrumData;
    QVector<float> m_waveformData;
    
    // 相机参数
    float m_cameraDistance;
    float m_cameraPitch;
    float m_cameraYaw;
    bool m_autoRotation;
    
    // 交互参数
    QPoint m_lastMousePos;
    bool m_mousePressed;
    
    // 材质参数
    float m_metallic;
    float m_roughness;
    float m_emission;
    bool m_bloomEnabled;
    bool m_particlesEnabled;
    
    // OpenGL资源
    QOpenGLShaderProgram *m_shader;
    unsigned int m_framebuffer;
    unsigned int m_colorTexture;
    unsigned int m_depthTexture;
};

/**
 * 实时音频处理器
 * 处理音频信号并提取频谱数据
 */
class RealTimeAudioProcessor : public QObject {
    Q_OBJECT

public:
    explicit RealTimeAudioProcessor(QObject *parent = nullptr);
    ~RealTimeAudioProcessor();

    // 设置音频参数
    void setAudioFormat(int sampleRate, int channels, int sampleSize);
    
    // 处理音频数据
    void processAudioData(const QByteArray &data);
    
    // FFT参数
    void setFFTSize(int size);
    void setWindowFunction(int type); // 0=Hanning, 1=Hamming, 2=Blackman
    
    // 频率范围
    void setFrequencyRange(int minFreq, int maxFreq);
    
    // 获取处理结果
    QVector<float> getSpectrum() const { return m_spectrum; }
    QVector<float> getWaveform() const { return m_waveform; }
    
    // 特征提取
    float getCurrentRMS() const { return m_currentRMS; }
    float getCurrentPeak() const { return m_currentPeak; }
    float getZeroCrossingRate() const { return m_zeroCrossingRate; }
    float getSpectralCentroid() const { return m_spectralCentroid; }

signals:
    void spectrumReady(const QVector<float> &spectrum);
    void waveformReady(const QVector<float> &waveform);
    void audioFeaturesReady(float rms, float peak, float zcr, float centroid);

private:
    void performFFT();
    void extractFeatures();
    void applyWindowFunction();
    
    // 音频参数
    int m_sampleRate;
    int m_channels;
    int m_sampleSize;
    
    // FFT参数
    int m_fftSize;
    int m_windowType;
    int m_minFreq;
    int m_maxFreq;
    
    // 处理缓冲区
    QVector<float> m_audioBuffer;
    QVector<std::complex<float>> m_fftBuffer;
    QVector<float> m_windowFunction;
    
    // 输出数据
    QVector<float> m_spectrum;
    QVector<float> m_waveform;
    
    // 音频特征
    float m_currentRMS;
    float m_currentPeak;
    float m_zeroCrossingRate;
    float m_spectralCentroid;
    
    // 处理状态
    bool m_initialized;
    int m_bufferPos;
};

/**
 * 可视化控制面板
 * 控制各种可视化参数的UI面板
 */
class VisualizerControlPanel : public QWidget {
    Q_OBJECT

public:
    explicit VisualizerControlPanel(QWidget *parent = nullptr);

    void setSpectrumAnalyzer(SpectrumAnalyzer *analyzer);
    void setAudio3DVisualizer(Audio3DVisualizer *visualizer);

signals:
    void visualizationTypeChanged(SpectrumAnalyzer::VisualizationType type);
    void colorSchemeChanged(SpectrumAnalyzer::ColorScheme scheme);
    void effect3DChanged(Audio3DVisualizer::Effect3D effect);
    void intensityChanged(qreal intensity);
    void smoothingChanged(bool enabled, qreal factor);

private slots:
    void onVisualizationTypeChanged();
    void onColorSchemeChanged();
    void onEffect3DChanged();
    void onIntensitySliderChanged();
    void onSmoothingToggled();
    void onSmoothingFactorChanged();

private:
    void setupUI();
    
    SpectrumAnalyzer *m_spectrumAnalyzer;
    Audio3DVisualizer *m_audio3DVisualizer;
    
    // UI控件将在实现中创建
    class QComboBox *m_visualizationCombo;
    class QComboBox *m_colorSchemeCombo;
    class QComboBox *m_effect3DCombo;
    class QSlider *m_intensitySlider;
    class QCheckBox *m_smoothingCheckBox;
    class QSlider *m_smoothingSlider;
    class QCheckBox *m_3dCheckBox;
    class QSlider *m_rotationSlider;
};
