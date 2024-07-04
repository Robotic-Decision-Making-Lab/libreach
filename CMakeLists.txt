cmake_minimum_required(VERSION 3.8)
project(
  libreach
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "C++ library used to communicate with Reach Robotics devices"
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Boost REQUIRED COMPONENTS system)

add_library(libreach SHARED
  src/client.cpp
  src/cobs.cpp
  src/crc.cpp
  src/driver.cpp
  src/packet.cpp
  src/serial_client.cpp
  src/serial_driver.cpp
  src/udp_client.cpp
  src/udp_driver.cpp
)

target_include_directories(libreach
  PUBLIC
    $<INSTALL_INTERFACE:include/libreach>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(libreach PUBLIC cxx_std_20)
target_link_libraries(libreach PUBLIC Boost::system)

set(EXAMPLES
  alpha5_example
  bravo7_example
  multiple_workers
  request_packets
  send_packets
)

foreach(example IN ITEMS ${EXAMPLES})
  add_executable(${example} examples/${example}.cpp)
  add_dependencies(${example} libreach)
  target_link_libraries(${example} PUBLIC libreach)
  set_target_properties(${example} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/examples
  )
endforeach()

install(
  DIRECTORY include/
  DESTINATION include/libreach
)

install(TARGETS libreach
  EXPORT libreachTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
)
