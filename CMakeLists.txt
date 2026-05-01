cmake_minimum_required(VERSION 3.16)
project(ScadaForDiesel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

# Библиотека для бэкенда
set(BACKEND_SOURCES
    src/backend_worker/backend_worker.cpp
    src/data_processor/data_processor.cpp
    src/data_store/data_store.cpp
    src/modbus_data_bridge/modbus_data_bridge.cpp
    src/state_machine/state_machine.cpp
)

add_library(backend_lib STATIC ${BACKEND_SOURCES})

# Указываем папки с заголовками
target_include_directories(backend_lib PUBLIC 
    src/backend_worker
    src/data_store
    src/data_processor
    src/modbus_data_bridge
    src/state_machine
)
target_link_libraries(backend_lib PUBLIC Qt6::Core)

enable_testing()

# Тест для BackendWorker
add_executable(test_backend_worker unit_tests/backend_worker_test/backend_worker_test.cpp)
target_link_libraries(test_backend_worker PRIVATE Qt6::Test backend_lib)
add_test(NAME BackendWorkerTest COMMAND test_backend_worker)

# Тест для DataProcessor
add_executable(test_data_processor unit_tests/data_processor_test/data_processor_test.cpp)
target_link_libraries(test_data_processor PRIVATE Qt6::Test backend_lib)
add_test(NAME DataProcessorTest COMMAND test_data_processor)

# Тест для DataStore
add_executable(test_data_store unit_tests/data_store_test/data_store_test.cpp)
target_link_libraries(test_data_store PRIVATE Qt6::Test backend_lib)
add_test(NAME DataStoreTest COMMAND test_data_store)

# Тест для ModbusDataBridge
add_executable(test_modbus_data_bridge unit_tests/modbus_data_bridge_test/modbus_data_bridge_test.cpp)
target_link_libraries(test_modbus_data_bridge PRIVATE Qt6::Test backend_lib)
add_test(NAME ModbusDataBridgeTest COMMAND test_modbus_data_bridge)

# Тест для StateMachine
add_executable(test_state_machine unit_tests/state_machine_test/state_machine_test.cpp)
target_link_libraries(test_state_machine PRIVATE Qt6::Test backend_lib)
add_test(NAME StateMachineTest COMMAND test_state_machine)