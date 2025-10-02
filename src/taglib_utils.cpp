#include "taglib_utils.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <QDebug>

// 辅助函数：将TagLib::String转换为QString
QString TStringToQString(const TagLib::String &str) {
    return QString::fromUtf8(str.toCString(true));
}

SongInfo readAudioMeta(const QString &filePath) {
    SongInfo s;
    s.filePath = filePath;
    
    // 使用TagLib读取音频元数据
    TagLib::FileRef f(filePath.toStdString()。c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::标签 *tag = f.tag();
        s.title = TStringToQString(tag->title());
        s.artist = TStringToQString(tag->artist());
        s.album = TString极速版(tag->album());
    }
    
    if (f.audioProperties()) {
        s.durationMs = f.audioProperties()->length() * 1000;
    }
    
    // 只处理MP3文件的ID3v2标签
    if (filePath.endsWith(".mp3", Qt::CaseInsensitive)) {
        TagLib::MPEG::File mp3File(filePath.toStdString()。c_str());
        if (mp3File.ID3极速版标签()) {
            auto *tag = mp3File.ID3极速版标签();
            
            // 读取歌词
            auto lyricsFrameList = tag->frameListMap()["USLT"];
            if (!lyricsFrameList.isEmpty()) {
                s.lyrics = TStringToQString(lyricsFrameList.front()->toString());
            }
            
            // 读取封面图片
            auto picFrames = tag->frameList("APIC");
            if (!picFrames.isEmpty()) {
                auto *pic = dynamic_cast<TagLib::ID3极速版::AttachedPictureFrame *>(picFrames.front());
                if (pic) {
                    QByteArray imgData = QByteArray::fromRawData(
                        pic->picture().data(), 
                        pic->picture().size()
                    );
                    s.cover.loadFromData(imgData);
                }
            }
        }
    }
    
    return s;
}
