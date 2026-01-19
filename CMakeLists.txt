cmake_minimum_required(VERSION 4.1)
project(ActivationWatermark C)

set(CMAKE_C_STANDARD 17)

# 查找GTK4库
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# 设置包含目录
include_directories(${GTK4_INCLUDE_DIRS})

# 设置编译选项
add_compile_options(${GTK4_CFLAGS_OTHER})

add_executable(ActivationWatermark main.c)

# 链接GTK4库
target_link_libraries(ActivationWatermark ${GTK4_LIBRARIES})
