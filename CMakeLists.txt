cmake_minimum_required(VERSION 3.10)
project(FProc
    VERSION 3.7.0
    DESCRIPTION "Windows Process Monitoring Tool"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W3 /permissive-)
    add_definitions(
        -D_CRT_SECURE_NO_WARNINGS
        -DWIN32_LEAN_AND_MEAN
        -DNOMINMAX
    )
endif()

set(SOURCE_FILES
    src/main.cpp
    src/ProcessManager.cpp
    src/ProcessManager.h
    src/ConsoleUtils.cpp
    src/ConsoleUtils.h
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets/FProc.rc")
    set(SOURCE_FILES ${SOURCE_FILES} assets/FProc.rc)
    message(STATUS "Found resource file in assets/ folder")
endif()

include_directories(src)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} psapi kernel32)
endif()
