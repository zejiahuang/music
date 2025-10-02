#include "taglib_utils.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

// 辅助函数：将TagLib::String转换为QString
QString TStringToQString(const TagLib::String &str) {
    return QString::fromUtf8(str.toCString(true));
}

SongInfo readAudioMeta(const QString &filePath) {
    SongInfo s;
    s.filePath = filePath;
    TagLib::FileRef f(filePath.toStdString()。c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::标签 *tag = f.tag();
        s.title = TStringToQString(tag->title());
        s.artist = TStringToQString(tag->artist());
        s.album = TStringToQString(tag->album());
    }
    if (f.audioProperties())
        s.durationMs = f.audioProperties()->length() * 1000;
    
    // 只处理MP3文件的ID3v2标签
    if (filePath.endsWith(".mp3", Qt::CaseInsensitive)) {
        TagLib::MPEG::File mp3File(filePath.toStdString()。c_str());
        if (mp3File.ID3v2Tag()) {
            auto *tag = mp3File.ID3v2Tag();
            auto lyricsFrame = tag->frameListMap()["USLT"];
            if (!lyricsFrame.isEmpty())
                s.lyrics = TStringToQString(lyricsFrame.front()->toString());
            auto picFrames = tag->frameList("APIC");
            if (!picFrames.isEmpty()) {
                auto *pic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame *>(picFrames.front());
                QByteArray imgData = QByteArray::fromRawData(pic->picture().data(), pic->picture().size());
                s.cover.loadFromData(imgData);
            }
        }
    }
    return s;
}
