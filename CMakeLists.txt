# I REMADE THE CMAKE BECAUSE IT SIMPLY HAD ALOT OF PROBLEMS
# Added:
#   Fix for Windows and Linux libraries
#   Set default CMAKE_BUILD_TYPE to Release
cmake_minimum_required(VERSION 3.13) # Updated the version to 3.13 because some CMake functions require 3.13
project(CHIPemu VERSION 1.0)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Default is Release, please specify if you want to compile as Debug." FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # I don't know what I was thinking while coding this part

add_executable(CHIPEmu
    main.cpp
    includes/glad/glad.c
    includes/imgui/imgui_draw.cpp
    includes/imgui/imgui_impl_glfw.cpp
    includes/imgui/imgui_impl_opengl3.cpp
    includes/imgui/imgui_tables.cpp
    includes/imgui/imgui_widgets.cpp
    includes/imgui/imgui.cpp
)

target_include_directories(CHIPEmu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
#target_link_libraries(CHIPEmu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a) # Moved for linux support

if (WIN32)
    target_link_libraries(CHIPEmu PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a gdi32)
    target_link_options(CHIPEmu PRIVATE -static-libgcc -static-libstdc++ -static)
else()
    find_package(Threads REQUIRED)
    find_library(GLFW_STATIC_LIB NAMES libglfw3.a glfw3)

    if(GLFW_STATIC_LIB)
        target_link_libraries(CHIPEmu PRIVATE ${GLFW_STATIC_LIB} GL X11 pthread rt m dl Threads::Threads)
    else()
        find_package(glfw3 REQUIRED)
        target_link_libraries(CHIPEmu PRIVATE glfw)
    endif()
endif()