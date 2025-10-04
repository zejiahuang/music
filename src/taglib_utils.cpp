#include "../include/taglib_utils.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

static QString convertString(TagLib::String s)
{
    const char* cstr = s.toCString(true);
    return QString::fromUtf8(cstr);
}

SongInfo readAudioMeta(const QString& filePath)
{
    SongInfo s;
    s.filePath = filePath;
    
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
    
    return s;
}
