# Added imgui support, now app needs imgui sources. Don't delete
cmake_minimum_required(VERSION 3.10)
project(CHIPemu VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD, True)

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
else()
    find_package(glfw3 REQUIRED)
    target_link_libraries(CHIPEmu PRIVATE glfw)
endif()