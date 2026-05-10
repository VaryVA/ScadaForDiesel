cmake_minimum_required(VERSION 3.16)

# Настройки проекта
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Зависимости
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Network
    Test
    SerialBus
)

# Исходники
set(BACKEND_SOURCES
    backend_worker/backendworker.cpp
    backend_worker/backendcommunicator.cpp
    config_data/config_data.cpp
    data_processing/DataProcessor.cpp
    data_store/DataStore.cpp
    modbus_client/IModbusBridge.cpp
    modbus_client/QtModbusBridge.cpp
    modbus_client/MockModbusBridge.cpp
    modbus_client/RegisterConverter.cpp
    state_machine/state_machine.cpp
)

# Заголовки
set(BACKEND_HEADERS
    ${PROJECT_SOURCE_DIR}/include/backend/backend_worker/backendworker.h
    ${PROJECT_SOURCE_DIR}/include/backend/backend_worker/backendcommunicator.h
    ${PROJECT_SOURCE_DIR}/include/backend/data_processing/DataProcessor.h
    ${PROJECT_SOURCE_DIR}/include/backend/data_store/DataStore.h
    ${PROJECT_SOURCE_DIR}/include/backend/DataTypes.h
    ${PROJECT_SOURCE_DIR}/include/backend/BackendGlobal.h
    ${PROJECT_SOURCE_DIR}/include/backend/config_data/config_data.h
    ${PROJECT_SOURCE_DIR}/include/backend/modbus_client/IModbusBridge.h
    ${PROJECT_SOURCE_DIR}/include/backend/modbus_client/QtModbusBridge.h
    ${PROJECT_SOURCE_DIR}/include/backend/modbus_client/MockModbusBridge.h
    ${PROJECT_SOURCE_DIR}/include/backend/modbus_client/RegisterConverter.h
    ${PROJECT_SOURCE_DIR}/include/backend/state_machine/state_machine.h
)

# Библиотека
add_library(backend_lib SHARED)

target_compile_definitions(backend_lib PRIVATE BACKEND_LIBRARY)

target_sources(backend_lib
    PRIVATE
        ${BACKEND_SOURCES}
        ${BACKEND_HEADERS}
)

# Include paths для компиляции
target_include_directories(backend_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Зависимости
target_link_libraries(backend_lib
    PUBLIC
        Qt6::Core
        Qt6::Network
        Qt6::Test
        Qt6::SerialBus
)

# Группировка файлов для QtCreator
# Это убирает попадание хедеров в <Other Locations>
source_group(TREE "${PROJECT_SOURCE_DIR}/include" PREFIX "include" FILES ${BACKEND_HEADERS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "src" FILES ${BACKEND_SOURCES})

# Unit Tests
# enable_testing()

# # Макрос для создания тестов
# macro(add_backend_test TEST_NAME)
#     add_executable(${TEST_NAME} ${ARGN})

#     target_include_directories(${TEST_NAME} PRIVATE
#         ${PROJECT_SOURCE_DIR}/include
#         ${CMAKE_CURRENT_SOURCE_DIR}
#     )

#     target_link_libraries(${TEST_NAME} PRIVATE
#         Qt6::Core
#         Qt6::Test
#         Qt6::Network
#         Qt6::SerialBus
#         backend_lib
#     )

#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
# endmacro()

# # Добавляем тесты
# add_backend_test(test_backend_worker
#     unit_tests/backend_worker_test/backend_worker_test.cpp
#     unit_tests/backend_worker_test/backend_worker_test.h
# )

# add_backend_test(test_data_processor
#     unit_tests/data_processor_test/data_processor_test.cpp
#     unit_tests/data_processor_test/data_processor_test.h
# )

# add_backend_test(test_data_store
#     unit_tests/data_store_test/data_store_test.cpp
#     unit_tests/data_store_test/data_store_test.h
# )

# add_backend_test(test_modbus_data_bridge
#     unit_tests/modbus_data_bridge_test/modbus_data_bridge_test.cpp
#     unit_tests/modbus_data_bridge_test/modbus_data_bridge_test.h
# )

# add_backend_test(test_state_machine
#     unit_tests/state_machine_test/state_machine_test.cpp
#     unit_tests/state_machine_test/state_machine_test.h
# )

# Вывод информации о сборке
message(STATUS "Backend Library: backend_lib")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 Version: ${Qt6Core_VERSION}")

message(STATUS "Tests enabled")