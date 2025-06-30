cmake_minimum_required(VERSION 3.10)
project(SokobanGame)

set(CMAKE_CXX_STANDARD 11)

# 源码文件
set(SOURCES
    main.cpp
    # Uncomment these lines when the files are created
    # game.cpp
    # map.cpp
    # ui.cpp
)

# 头文件目录
include_directories(${CMAKE_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

aux_source_directory(${CMAKE_SOURCE_DIR}/src SOURCES)

# EasyX 库路径（假设已正确安装 EasyX，且 easyx 头文件和库文件在默认路径）
# 如有自定义安装路径，请修改下方 include 和 link 路径
include_directories("D:/Game/easyX/include")
link_directories("D:/Game/easyX/lib64")

add_executable(SokobanGame ${SOURCES})

# 链接 EasyX 及 Windows 图形库
target_link_libraries(SokobanGame
    ${CMAKE_SOURCE_DIR}/easyx/libeasyx.a
    gdi32
)

# 资源文件夹（仅供参考，图片需手动放到可执行文件同目录下的 res/ 文件夹）
