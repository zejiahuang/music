#pragma once
#include <QString>
#include "songinfo.h"
#include <taglib/tstring.h>

QString TStringToQString(const TagLib::String& str);
SongInfo readAudioMeta(const QString& filePath);
