# COMPLETE CMAKE OVERHAUL
# Old CMake had GLFW problems so I fixed it. Kinda.
# Compilation will work the same-old way.

cmake_minimum_required(VERSION 3.14)
project(CHIPEmu VERSION 1.0 LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Default is Release, please specify if you want to compile as Debug." FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
    set(GLFW_LIB_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lib/libglfw3.a")
    add_library(local_glfw STATIC IMPORTED)
    set_target_properties(local_glfw PROPERTIES
        IMPORTED_LOCATION "${GLFW_LIB_FILE}"
    ) 
endif()
set(SRC_FILES
    main.cpp
    includes/glad/glad.c
    includes/imgui/imgui_draw.cpp
    includes/imgui/imgui_impl_glfw.cpp
    includes/imgui/imgui_impl_opengl3.cpp
    includes/imgui/imgui_tables.cpp
    includes/imgui/imgui_widgets.cpp
    includes/imgui/imgui.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE includes)

if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        local_glfw
        opengl32
        gdi32
        comdlg32
        ole32
        uuid 
    )
elseif(UNIX AND NOT APPLE)
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glfw    
        GL
        pthread
        dl
    )
endif()