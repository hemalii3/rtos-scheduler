cmake_minimum_required(VERSION 3.16)
project(RTOSScheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

# Main engine library (reused by tests + server)
add_library(scheduler_lib
  src/Task.cpp
  src/RMSScheduler.cpp
  src/EDFScheduler.cpp
  src/SocketServer.cpp
)
target_include_directories(scheduler_lib PUBLIC include)

# Main executable (TCP server)
add_executable(scheduler_server src/main.cpp)
target_link_libraries(scheduler_server scheduler_lib pthread)

# Test executable
add_executable(scheduler_tests tests/test_scheduler.cpp)
target_link_libraries(scheduler_tests scheduler_lib GTest::gtest_main pthread)

enable_testing()
add_test(NAME SchedulerTests COMMAND scheduler_tests)
