cmake_minimum_required(VERSION 3.10)

project(IPC_Communication)

# Use C++14 for auto return type deduction
set(CMAKE_CXX_STANDARD 14)

# Find Boost package with required components
find_package(Boost 1.66 REQUIRED COMPONENTS system thread)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/../deps/libserial/build/install/include
    ${Boost_INCLUDE_DIRS}
)

# Link directories for libserial
link_directories(
    ${CMAKE_SOURCE_DIR}/../deps/libserial/build/install/lib
)

add_executable(ipc_comm 
    src/main.cpp
    src/serial_handler.cpp
    src/websocket_handler.cpp
    src/ipc_manager.cpp
)

# Link libraries (use libserial instead of libserial_static)
target_link_libraries(ipc_comm serial ${Boost_LIBRARIES})