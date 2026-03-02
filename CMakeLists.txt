cmake_minimum_required(VERSION 3.21)

project(LiteRTExamples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

link_directories(
    ${CMAKE_BINARY_DIR}/external/litert/LiteRT/litert/c
)

add_executable(lrex
    main.cpp
    core/core.cpp
    gui/window.cpp
)

target_include_directories(lrex
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${CMAKE_SOURCE_DIR}/external/litert/LiteRT
    ${CMAKE_BINARY_DIR}/abseil-cpp
)

target_compile_options(lrex PRIVATE
    -w
)

target_link_libraries(lrex PRIVATE
    absl::die_if_null
    Qt6::Widgets
    opencv_core
    opencv_imgcodecs
    opencv_imgproc
    litert_runtime_c_api_shared_lib
)


add_subdirectory(external/litert)
