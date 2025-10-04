cmake_minimum_required(VERSION 3.14)
project(musicplayer VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 自动处理Qt的moc、rcc和uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置输出目录 - 与测试脚本期望的路径一致
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 在Windows上使用静态运行时库
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 查找Qt5组件 - 渐进式查找，确保CI兼容性
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Network QUIET)
find_package(Qt5 COMPONENTS Multimedia QUIET)
find_package(Qt5 COMPONENTS OpenGL QUIET)
find_package(Qt5 COMPONENTS Svg QUIET)
find_package(Qt5 COMPONENTS Xml QUIET)
find_package(Qt5 COMPONENTS Concurrent QUIET)

# 查找TagLib和FFmpeg组件 - 使用vcpkg的CMake集成
find_package(taglib CONFIG REQUIRED)

# 查找FFmpeg - 使用vcpkg的CMake集成
find_package(PkgConfig QUIET)
set(FFMPEG_FOUND FALSE)
set(FFMPEG_LIBRARIES)
set(FFMPEG_INCLUDE_DIRS)
set(FFMPEG_LIBRARY_DIRS)

# 检查是否使用vcpkg工具链
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
if(CMAKE_TOOLCHAIN_FILE AND CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
    message(STATUS "Detected vcpkg toolchain - using vcpkg-specific search")
    set(USING_VCPKG TRUE)
else()
    set(USING_VCPKG FALSE)
endif()

# 调试信息：输出当前的库搜索路径
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "CMAKE_LIBRARY_PATH: ${CMAKE_LIBRARY_PATH}")
if(DEFINED ENV{VCPKG_ROOT})
    message(STATUS "VCPKG_ROOT: $ENV{VCPKG_ROOT}")
    set(VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows")
    message(STATUS "VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
endif()

# 优先尝试vcpkg方式（单独查找各个组件）
# 为vcpkg环境添加特定的搜索路径
if(USING_VCPKG AND DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_INCLUDE_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
    set(VCPKG_LIB_DIR "$ENV{VCPKG_ROOT}/installed/x64-windows/lib")
    
    message(STATUS "Searching for FFmpeg in vcpkg directories:")
    message(STATUS "  Include: ${VCPKG_INCLUDE_DIR}")
    message(STATUS "  Lib: ${VCPKG_LIB_DIR}")
    
    # 添加vcpkg路径到搜索路径
    list(APPEND CMAKE_PREFIX_PATH "$ENV{VCPKG_ROOT}/installed/x64-windows")
    
    # 使用更具体的路径查找各个FFmpeg组件
    find_path(AVFORMAT_INCLUDE_DIR 
        NAMES libavformat/avformat.h
        PATHS "${VCPKG_INCLUDE_DIR}"
        NO_DEFAULT_PATH
    )
    
    find_library(AVFORMAT_LIBRARY 
        NAMES avformat
        PATHS "${VCPKG_LIB_DIR}"
        NO_DEFAULT_PATH
    )
    
    find_library(AVCODEC_LIBRARY 
        NAMES avcodec
        PATHS "${VCPKG_LIB_DIR}"
        NO_DEFAULT_PATH
    )
    
    find_library(AVUTIL_LIBRARY 
        NAMES avutil
        PATHS "${VCPKG_LIB_DIR}"
        NO_DEFAULT_PATH
    )
    
    find_library(SWRESAMPLE_LIBRARY 
        NAMES swresample
        PATHS "${VCPKG_LIB_DIR}"
        NO_DEFAULT_PATH
    )
else()
    # 非vcpkg环境或没有VCPKG_ROOT，使用默认查找
    message(STATUS "Using standard library search (non-vcpkg or VCPKG_ROOT not set)")
    
    find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
    find_library(AVFORMAT_LIBRARY avformat)
    find_library(AVCODEC_LIBRARY avcodec)
    find_library(AVUTIL_LIBRARY avutil)
    find_library(SWRESAMPLE_LIBRARY swresample)
endif()

# 调试输出查找结果
message(STATUS "FFmpeg component search results:")
message(STATUS "  AVFORMAT_INCLUDE_DIR: ${AVFORMAT_INCLUDE_DIR}")
message(STATUS "  AVFORMAT_LIBRARY: ${AVFORMAT_LIBRARY}")
message(STATUS "  AVCODEC_LIBRARY: ${AVCODEC_LIBRARY}")
message(STATUS "  AVUTIL_LIBRARY: ${AVUTIL_LIBRARY}")
message(STATUS "  SWRESAMPLE_LIBRARY: ${SWRESAMPLE_LIBRARY}")

if(AVFORMAT_INCLUDE_DIR AND AVFORMAT_LIBRARY AND AVCODEC_LIBRARY AND AVUTIL_LIBRARY AND SWRESAMPLE_LIBRARY)
    set(FFMPEG_FOUND TRUE)
    set(FFMPEG_INCLUDE_DIRS ${AVFORMAT_INCLUDE_DIR})
    set(FFMPEG_LIBRARIES ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY})
    message(STATUS "✓ Found FFmpeg via vcpkg: ${FFMPEG_LIBRARIES}")
elseif(PkgConfig_FOUND)
    # 回退到pkg-config方式（用于非vcpkg环境）
    message(STATUS "Trying pkg-config as fallback...")
    pkg_check_modules(FFMPEG QUIET libavformat libavcodec libavutil libswresample)
    if(FFMPEG_FOUND)
        message(STATUS "✓ Found FFmpeg via pkg-config: ${FFMPEG_LIBRARIES}")
    endif()
else()
    message(STATUS "PkgConfig not found, skipping pkg-config fallback")
endif()

# 如果所有方法都失败，给出明确错误信息和调试建议
if(NOT FFMPEG_FOUND)
    message(STATUS "")
    message(STATUS "❌ FFmpeg not found. Debugging information:")
    message(STATUS "")
    if(DEFINED ENV{VCPKG_ROOT})
        message(STATUS "VCPKG environment detected:")
        message(STATUS "  VCPKG_ROOT: $ENV{VCPKG_ROOT}")
        file(GLOB vcpkg_libs "$ENV{VCPKG_ROOT}/installed/x64-windows/lib/av*.lib")
        if(vcpkg_libs)
            message(STATUS "  Found FFmpeg-related libraries:")
            foreach(lib ${vcpkg_libs})
                message(STATUS "    ${lib}")
            endforeach()
        else()
            message(STATUS "  ❌ No FFmpeg libraries found in $ENV{VCPKG_ROOT}/installed/x64-windows/lib/")
            message(STATUS "  Please run: vcpkg install ffmpeg[core,avcodec,avformat,avutil,swresample]:x64-windows")
        endif()
    else()
        message(STATUS "❌ VCPKG_ROOT environment variable not set")
        message(STATUS "Please ensure vcpkg is properly configured")
    endif()
    message(STATUS "")
    message(FATAL_ERROR "FFmpeg not found. Please ensure FFmpeg is installed via vcpkg.")
endif()

# 添加可执行文件 - 包含头文件以确保MOC正确处理
add_executable(musicplayer
    src/main.cpp
    src/playerwindow.cpp
    src/playlist.cpp
    src/taglib_utils.cpp
    src/ffmpeg_waveform.cpp
    src/playlistmanager.cpp
    src/ui/lyricsvisualwidget.cpp
    src/ffmpegplayer.cpp
    src/materialui_components.cpp
    
    # 包含Q_OBJECT宏的头文件，确保MOC处理
    include/playerwindow.h
    include/ffmpegplayer.h
    include/materialui_components.h
    src/ui/lyricsvisualwidget.h
)

# 包含目录
target_include_directories(musicplayer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# 添加FFmpeg包含目录
if(FFMPEG_INCLUDE_DIRS)
    target_include_directories(musicplayer PRIVATE ${FFMPEG_INCLUDE_DIRS})
endif()

# 链接库 - 渐进式链接，只链接可用的模块
target_link_libraries(musicplayer
    Qt5::Widgets
    TagLib::tag
)

# 条件性链接可选Qt模块
if(TARGET Qt5::Network)
    target_link_libraries(musicplayer Qt5::Network)
    message(STATUS "Linked Qt5::Network")
endif()

if(TARGET Qt5::Multimedia)
    target_link_libraries(musicplayer Qt5::Multimedia)
    message(STATUS "Linked Qt5::Multimedia")
endif()

if(TARGET Qt5::OpenGL)
    target_link_libraries(musicplayer Qt5::OpenGL)
    message(STATUS "Linked Qt5::OpenGL")
endif()

if(TARGET Qt5::Svg)
    target_link_libraries(musicplayer Qt5::Svg)
    message(STATUS "Linked Qt5::Svg")
endif()

if(TARGET Qt5::Xml)
    target_link_libraries(musicplayer Qt5::Xml)
    message(STATUS "Linked Qt5::Xml")
endif()

if(TARGET Qt5::Concurrent)
    target_link_libraries(musicplayer Qt5::Concurrent)
    message(STATUS "Linked Qt5::Concurrent")
endif()

# 链接FFmpeg库
if(FFMPEG_LIBRARIES)
    target_link_libraries(musicplayer ${FFMPEG_LIBRARIES})
    if(FFMPEG_LIBRARY_DIRS)
        target_link_directories(musicplayer PRIVATE ${FFMPEG_LIBRARY_DIRS})
    endif()
endif()

# 设置目标属性
set_target_properties(musicplayer PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# Windows资源（图标）- 暂时禁用直到提供有效的.ico文件
# set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.rc")
# if(WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources/icon.ico")
#     target_sources(musicplayer PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
# else()
#     message(STATUS "Skipping icon resource: icon.ico not found or not on Windows")
# endif()

# 安装目标
install(TARGETS musicplayer
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

# 安装依赖库（Windows）
if(WIN32)
    message(STATUS "Dependencies managed by vcpkg - no manual DLL copying needed")
endif()

# 添加资源文件
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/resources")
    file(GLOB_RECURSE RESOURCE_FILES "src/resources/*")
    foreach(RESOURCE ${RESOURCE_FILES})
        get_filename_component(RESOURCE_NAME ${RESOURCE} NAME)
        set_source_files_properties(${RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endforeach()
endif()

# 为GitHub Actions添加特殊处理
if(DEFINED ENV{GITHUB_ACTIONS})
    message(STATUS "Running in GitHub Actions environment")
    
    # 添加vcpkg工具链文件
    if(EXISTS "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Using vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")
    endif()
    
    # 设置额外的包含路径
    if(EXISTS "$ENV{VCPKG_ROOT}/installed/x64-windows/include")
        include_directories("$ENV{VCPKG_ROOT}/installed/x64-windows/include")
        message(STATUS "Added vcpkg include directory")
    endif()
    
    # 设置额外的库路径
    if(EXISTS "$ENV{VCPKG_ROOT}/installed/x64-windows/lib")
        link_directories("$ENV{VCPKG_ROOT}/installed/x64-windows/lib")
        message(STATUS "Added vcpkg library directory")
    endif()
endif()

# 添加自定义目标，用于检查所有源文件是否存在
add_custom_target(check_sources
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckSources.cmake
    COMMENT "Checking if all source files exist"
    VERBATIM
)

# 创建检查源文件的脚本
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckSources.cmake "
message(STATUS \"Checking source files...\")
foreach(src 
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/playerwindow.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/playlist.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/taglib_utils.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpeg_waveform.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/playlistmanager.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/ui/lyricsvisualwidget.cpp\"
    \"${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpegplayer.cpp\"
)
    if(NOT EXISTS \"\${src}\")
        message(FATAL_ERROR \"Source file \${src} does not exist!\")
    else()
        message(STATUS \"Found source file: \${src}\")
    endif()
endforeach()
message(STATUS \"All source files exist!\")
")

# 添加自定义目标，用于在构建前检查源文件
add_custom_command(TARGET musicplayer PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CheckSources.cmake
    COMMENT "Pre-build check: Verifying all source files exist"
    VERBATIM
)

# vcpkg自动管理依赖，不需要手动复制DLL
if(WIN32)
    message(STATUS "Dependencies will be automatically handled by vcpkg")
    
    # 为测试脚本创建兼容性路径
    add_custom_command(TARGET musicplayer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Release"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:musicplayer>" "${CMAKE_BINARY_DIR}/Release/"
        COMMENT "Creating compatibility path for test scripts"
        VERBATIM
    )
endif()

# 添加测试目标（可选）
enable_testing()
add_test(NAME musicplayer_test COMMAND musicplayer --test)

# 添加打包目标
add_custom_target(package
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target package
    COMMENT "Creating package"
    VERBATIM
)

# 添加清理目标
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build artifacts"
    VERBATIM
)

# 输出配置信息
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt5 Found: ${Qt5Widgets_FOUND}")
message(STATUS "TagLib Found: ${taglib_FOUND}")
message(STATUS "FFmpeg Found: ${FFMPEG_FOUND}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
