#pragma once
#include <QString>
#include "songinfo.h"
#include <taglib/tstring.h>

SongInfo readAudioMeta(const QString& filePath);
