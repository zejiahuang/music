cmake_minimum_required(VERSION 3.14)
project(musicplayer)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 强制 vcpkg toolchain，并自动设置 CMAKE_PREFIX_PATH 以解决 find_package 问题
if(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "" FORCE)
    set(CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows" CACHE STRING "" FORCE)
endif()

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Taglib REQUIRED)
find_package(FFmpeg REQUIRED)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(musicplayer
    src/main.cpp
    src/playerwindow.cpp
    src/playlist.cpp
    src/taglib_utils.cpp
    src/ffmpeg_waveform.cpp
    src/playlistmanager.cpp
    src/ui/lyricsvisualwidget.cpp
)

target_link_libraries(musicplayer
    Qt5::Widgets
    Taglib::tag
    FFmpeg::avformat
    FFmpeg::avcodec
    FFmpeg::avutil
    FFmpeg::swresample
)

# Windows资源（图标）
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.rc")
if (WIN32)
    target_sources(musicplayer PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()
# Windows资源（图标）
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.rc")
if (WIN32)
    target_sources(musicplayer PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()
