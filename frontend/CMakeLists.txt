cmake_minimum_required(VERSION 3.22)

project(frontend VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)

set(RESOURCES
    resources/resources.qrc
)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow/mainwindow.cpp
    src/mainwindow/mainwindow.h
    src/mainwindow/mainwindow.ui
)

file(GLOB_RECURSE TAB_SOURCES
        src/tabs/*.cpp
        src/tabs/*.h
        src/tabs/*.ui
)

file(GLOB_RECURSE CORE_SOURCES
        src/core/*.cpp
        src/core/*.h
)

# Объединяем все источники
list(APPEND PROJECT_SOURCES ${TAB_SOURCES} ${CORE_SOURCES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(frontend
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${RESOURCES}
        src/core/data/trendcontroller.h src/core/data/trendcontroller.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET frontend APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(frontend SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(frontend
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(frontend PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
)

set_target_properties(frontend PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(frontend)
endif()
