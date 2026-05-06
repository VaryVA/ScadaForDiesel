cmake_minimum_required(VERSION 3.16)

# Главный проект
project(ScadaForDiesel 
    LANGUAGES CXX
    DESCRIPTION "SCADA система стенда тестирования ДВС (дизельных двигателей)"
    VERSION 1.0.0
)

# Глобальные настройки
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Директория для собранных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Backend библиотека
add_subdirectory(src/backend)

# Информация о сборке
message(STATUS "")
message(STATUS "=== ScadaForDiesel ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
