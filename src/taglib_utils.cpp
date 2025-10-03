#include "taglib_utils.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>
#include <QDebug>

// Helper function: Convert TagLib::String to QString
QString TStringToQString(const TagLib::String& str) {
    return QString::fromUtf8(str.toCString(true));
}

SongInfo readAudioMeta(const QString& filePath) {
    SongInfo s;
    s.filePath = filePath;
    
    // Use TagLib to read audio metadata
    TagLib::FileRef f(filePath.toStdString().c_str());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag* tag = f.tag();
        s.title = TStringToQString(tag->title());
        s.artist = TStringToQString(tag->artist());
        s.album = TStringToQString(tag->album());
    }
    
    if (f.audioProperties()) {
        s.durationMs = f.audioProperties()->length() * 1000;
    }
    
    // Only process ID3v2 tags for MP3 files
    if (filePath.endsWith(".mp3", Qt::CaseInsensitive)) {
        TagLib::MPEG::File mp3File(filePath.toStdString().c_str());
        if (mp3File.ID3v2Tag()) {
            TagLib::ID3v2::Tag* tag = mp3File.ID3v2Tag();
            
            // Read lyrics
            TagLib::ID3v2::FrameList lyricsFrameList = tag->frameListMap()["USLT"];
            if (!lyricsFrameList.isEmpty()) {
                s.lyrics = TStringToQString(lyricsFrameList.front()->toString());
            }
            
            // Read cover image
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