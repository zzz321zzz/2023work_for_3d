cmake_minimum_required(VERSION 3.23.1)

project(ZYJ3DCAD)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)

# 添加 opengl 目录到包含路径
set(GLUT_BASE ${PROJECT_SOURCE_DIR}/freeglut/include)

# 输出调试信息
message(STATUS "GLUT include directory: ${GLUT_BASE}")

# 添加源文件
file(GLOB_RECURSE SOURCES     
    ${PROJECT_SOURCE_DIR}/src/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.h
)

# 输出调试信息
message(STATUS "Source files: ${SOURCES}")

# 添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${GLUT_BASE})

# 添加库文件搜索路径
link_directories(${PROJECT_SOURCE_DIR}/freeglut/lib)

# 添加库
add_library(CAD STATIC ${SOURCES})

# 添加可执行文件
add_executable(THREED ${PROJECT_SOURCE_DIR}/src/main.cpp)

# 链接库
target_link_libraries(THREED CAD freeglut opengl32 glu32)
