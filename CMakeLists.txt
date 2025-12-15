cmake_minimum_required(VERSION 3.20)

project(LumaSort)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)

add_executable(LumaSort
    src/main.cpp
    src/app.cpp
    src/app.h
    src/graphics/renderer.cpp
    src/graphics/renderer.h
    src/ui/gui_layer.cpp
    src/ui/gui_layer.h
    src/graphics/texture.cpp
    src/graphics/texture.h
    src/graphics/canvas.cpp
    src/graphics/canvas.h
    src/core/sorter.cpp
    src/core/sorter.h
    src/core/particle.h
    src/core/flow_field.cpp
    src/core/flow_field.h
)

target_link_libraries(LumaSort PRIVATE
    ${OpenCV_LIBS}
    glad::glad
    glfw
    glm::glm
    imgui::imgui
    nfd::nfd
)

if(UNIX)
    target_link_libraries(LumaSort PRIVATE pthread dl)
endif()
