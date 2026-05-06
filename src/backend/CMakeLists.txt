cmake_minimum_required(VERSION 3.16)
project(ScadaForDiesel LANGUAGES CXX)

# Настройки проекта
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Зависимости
find_package(Qt6 REQUIRED COMPONENTS 
    Core 
    Network
    Test
)

# Modbus опционально
find_package(Qt6 COMPONENTS Modbus QUIET)
if(NOT Qt6Modbus_FOUND)
    message(WARNING "Qt6 Modbus not found - QtModbusBridge will be excluded from build")
    message(WARNING "Install: sudo apt install qt6-serial-dev (or equivalent)")
endif()

# Основная библиотека Backend
set(BACKEND_SOURCES
    backend_worker/backendworker.cpp
    backend_worker/backendcommunicator.cpp
    config_data/config_data.cpp
    data_processing/DataProcessor.cpp
    data_store/DataStore.cpp
    modbus_client/IModbusBridge.cpp
    modbus_client/MockModbusBridge.cpp
    state_machine/state_machine.cpp
)

# Добавляем QtModbusBridge только если Modbus доступен
if(Qt6Modbus_FOUND)
    list(APPEND BACKEND_SOURCES modbus_client/QtModbusBridge.cpp)
endif()

set(BACKEND_HEADERS
    backend_worker/backendworker.h
    backend_worker/backendcommunicator.h
    data_processor/DataProcessor.h
    data_store/DataStore.h
    modbus_client/IModbusBridge.h
    modbus_client/QtModbusBridge.h
    modbus_client/MockModbusBridge.h
)

add_library(backend_lib STATIC ${BACKEND_SOURCES})

# Включаемые директории для библиотеки
target_include_directories(backend_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/backend_worker
    ${CMAKE_CURRENT_SOURCE_DIR}/config_data
    ${CMAKE_CURRENT_SOURCE_DIR}/data_processing
    ${CMAKE_CURRENT_SOURCE_DIR}/data_store
    ${CMAKE_CURRENT_SOURCE_DIR}/modbus_client
    ${CMAKE_CURRENT_SOURCE_DIR}/state_machine
)

# Включаемые директории из include/
target_include_directories(backend_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/backend
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/backend/backend_worker
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/backend/data_store
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/backend/modbus_client
)

# Зависимости
target_link_libraries(backend_lib 
    PUBLIC 
        Qt6::Core
        Qt6::Network
        Qt6::Test
)

if(Qt6Modbus_FOUND)
    target_link_libraries(backend_lib PUBLIC Qt6::Modbus)
endif()

# Unit Tests
enable_testing()

# Макрос для создания тестов
macro(add_backend_test TEST_NAME TEST_FILE)
    add_executable(${TEST_NAME} unit_tests/${TEST_FILE})
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/backend_worker
        ${CMAKE_CURRENT_SOURCE_DIR}/config_data
        ${CMAKE_CURRENT_SOURCE_DIR}/data_processing
        ${CMAKE_CURRENT_SOURCE_DIR}/data_store
        ${CMAKE_CURRENT_SOURCE_DIR}/modbus_client
        ${CMAKE_CURRENT_SOURCE_DIR}/state_machine
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )
    target_link_libraries(${TEST_NAME} PRIVATE 
        Qt6::Core
        Qt6::Test
        Qt6::Network
        backend_lib
    )
    if(Qt6Modbus_FOUND)
        target_link_libraries(${TEST_NAME} PRIVATE Qt6::Modbus)
    endif()
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

# Добавляем тесты
add_backend_test(test_backend_worker backend_worker_test/backend_worker_test.h)
add_backend_test(test_data_processor data_processor_test/data_processor_test.h)
add_backend_test(test_data_store data_store_test/data_store_test.h)
add_backend_test(test_modbus_data_bridge modbus_data_bridge_test/modbus_data_bridge_test.h)
add_backend_test(test_state_machine state_machine_test/state_machine_test.h)

# Вывод информации о сборке
message(STATUS "Backend Library: backend_lib")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 Version: ${Qt6Core_VERSION}")
if(Qt6Modbus_FOUND)
    message(STATUS "Qt6 Modbus: FOUND (QtModbusBridge enabled)")
else()
    message(STATUS "Qt6 Modbus: NOT FOUND (using MockModbusBridge only)")
endif()
message(STATUS "Tests enabled")