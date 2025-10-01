cmake_minimum_required(VERSION 3.14)
project(musicplayer)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswresample)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${TAGLIB_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

link_directories(
    ${TAGLIB_LIBRARY_DIRS}
    ${FFMPEG_LIBRARY_DIRS}
)

add_executable(musicplayer
    src/main.cpp
    src/playerwindow.cpp
    src/playlist.cpp
    src/taglib_utils.cpp
    src/ffmpeg_waveform.cpp
    src/playlistmanager.cpp
    src/ui/lyricsvisualwidget.cpp
    include/songinfo.h
    include/playlist.h
    include/taglib_utils.h
    include/ffmpeg_waveform.h
    include/playlistmanager.h
    src/playerwindow.h
    src/ui/lyricsvisualwidget.h
)

target_link_libraries(musicplayer
    Qt5::Widgets
    ${TAGLIB_LIBRARIES}
    ${FFMPEG_LIBRARIES}
)

# Resources
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.rc")
if (WIN32)
    target_sources(musicplayer PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()