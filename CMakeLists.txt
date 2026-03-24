cmake_minimum_required(VERSION 3.27)
project(Chip-8)

add_subdirectory(vendor/SDL)


include_directories(vendor/imgui)
include_directories(vendor/ImGuiFileDialog)
file(GLOB IMGUI_SRC vendor/imgui/*.cpp)
set(CMAKE_CXX_STANDARD 23)


add_executable(${PROJECT_NAME}
        main.cpp
        Chip8.cpp
        Chip8.h
        vendor/ImGuiFileDialog/ImGuiFileDialog.cpp
        vendor/ImGuiFileDialog/ImGuiFileDialog.h
        ${IMGUI_SRC}

)

target_link_directories(${PROJECT_NAME} PUBLIC SDL3::SDL3)
target_link_libraries(${PROJECT_NAME} PUBLIC  SDL3::SDL3)

# Enlazar estáticamente las librerías estándar de C++ de MinGW y evitar errores de definición múltiple
target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ "-Wl,--allow-multiple-definition")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
)
