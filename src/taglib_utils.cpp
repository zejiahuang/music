#include "../include/taglib_utils.h"

#if defined(ENABLE_TAGLIB)
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#endif

static QString convertString(
#if defined(ENABLE_TAGLIB)
    TagLib::String s
#endif
)
{
#if defined(ENABLE_TAGLIB)
    const char* cstr = s.toCString(true);
    return QString::fromUtf8(cstr);
#else
    return QString();
#endif
}

SongInfo readAudioMeta(const QString& filePath)
{
    SongInfo s;
    s.filePath = filePath;
#if defined(ENABLE_TAGLIB)
    TagLib::FileRef f(filePath.toStdString().c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag* tag = f.tag();
        s.title = convertString(tag->title());
        s.artist = convertString(tag->artist());
        s.album = convertString(tag->album());
    }
    if (f.audioProperties()) {
        s.durationMs = f.audioProperties()->lengthInMilliseconds();
    }
    if (filePath.endsWith(".mp3", Qt::CaseInsensitive)) {
        TagLib::MPEG::File mp3File(filePath.toStdString().c_str());
        if (mp3File.ID3v2Tag()) {
            TagLib::ID3v2::Tag* tag = mp3File.ID3v2Tag();
            TagLib::ID3v2::FrameList lyricsFrameList = tag->frameListMap()["USLT"];
            if (!lyricsFrameList.isEmpty()) {
                s.lyrics = convertString(lyricsFrameList.front()->toString());
            }
            TagLib::ID3v2::FrameList picFrames = tag->frameList("APIC");
            if (!picFrames.isEmpty()) {
                TagLib::ID3v2::AttachedPictureFrame* pic = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(picFrames.front());
                if (pic) {
                    QByteArray imgData(pic->picture().data(), pic->picture().size());
                    s.cover.loadFromData(imgData);
                }
            }
        }
    }
#else
    // 无 TagLib 时，保留最基础信息，避免运行时加载 tag.dll
    s.durationMs = 0;
#endif
    return s;
}
