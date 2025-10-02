cmake_minimum_required(VERSION 3.14)
project(musicplayer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动处理Qt的moc、rcc和uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 查找Qt5组件
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# 查找TagLib和FFmpeg组件
find_package(PkgConfig REQUIRED)
pkg_check_modules(TAGLIB REQUIRED taglib)
pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libswresample)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TAGLIB_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

# 添加可执行文件
add_executable(musicplayer
    src/main.cpp
    src/playerwindow.cpp
    src/playlist.cpp
    src/taglib_utils.cpp
    src/ffmpeg_waveform.cpp
    src/playlistmanager.cpp
    src/ui/lyricsvisualwidget.cpp
    src/ffmpegplayer.cpp
)

# 链接库
target_link_libraries(musicplayer
    Qt5::Widgets
    ${TAGLIB_LIBRARIES}
    ${FFMPEG_LIBRARIES}
)

# Windows资源（图标）
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.rc")
if(WIN32)
    target_sources(musicplayer PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

# 安装目标
install(TARGETS musicplayer
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

# 安装依赖库（Windows）
if(WIN32)
    install(FILES
        ${TAGLIB_LIBRARY_DIRS}/tag.dll
        ${FFMPEG_LIBRARY_DIRS}/avformat-${FFMPEG_VERSION}.dll
        ${FFMPEG_LIBRARY_DIRS}/avcodec-${FFMPEG_VERSION}.dll
        ${FFMPEG_LIBRARY_DIRS}/avutil-${FFMPEG_VERSION}.dll
        ${FFMPEG_LIBRARY_DIRS}/swresample-${FFMPEG_VERSION}.dll
        DESTINATION bin
    )
endif()

# 添加资源文件
file(GLOB_RECURSE RESOURCE_FILES "src/resources/*")
foreach(RESOURCE ${RESOURCE_FILES})
    get_filename_component(RESOURCE_NAME ${RESOURCE} NAME)
    set_source_files_properties(${RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endforeach()

# 为GitHub Actions添加特殊处理
if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions environment")
    
    # 添加vcpkg工具链文件
    if(EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
    endif()
    
    # 设置额外的包含路径
    if(EXISTS "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
        include_directories("$ENV{VCPKG_ROOT}/installed/x64-windows/include")
    endif()
    
    # 设置额外的库路径
    if(EXISTS "$ENV{VCPKG_ROOT}/installed/x64-windows/lib")
        link_directories("$ENV{VCPKG_ROOT}/installed/x64-windows/lib")
    endif()
endif()
