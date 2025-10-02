#include "taglib_utils.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

SongInfo readAudioMeta(const QString &filePath) {
    SongInfo s;
    s.filePath = filePath;
    TagLib::FileRef f(filePath.toStdString().c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        s.title = QString::fromStdString(tag->title().to8Bit(true));
        s.artist = QString::fromStdString(tag->artist().to8Bit(true));
        s.album = QString::fromStdString(tag->album().to8Bit(true));
    }
    if (f.audioProperties())
        s.durationMs = f.audioProperties()->length() * 1000;
    
    // 只处理MP3文件的ID3v2标签
    if (filePath.endsWith(".mp3", Qt::CaseInsensitive)) {
        TagLib::MPEG::File mp3File(filePath.toStdString().c_str());
        if (mp3File.ID3v2Tag()) {
            auto *tag = mp3File.ID3v2Tag();
            auto lyricsFrame = tag->frameListMap()["USLT"];
            if (!lyricsFrame.isEmpty())
                s.lyrics = QString::fromStdString(lyricsFrame.front()->toString().to8Bit(true));
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
