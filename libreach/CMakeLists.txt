cmake_minimum_required(VERSION 3.8)
project(libreach)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(Boost COMPONENTS system REQUIRED)

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
    ${Boost_INCLUDE_DIRS}
  PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(libreach PUBLIC cxx_std_20)
target_link_libraries(libreach
  PUBLIC
    ${Boost_LIBRARIES}
)

install(
  DIRECTORY include/
  DESTINATION include/libreach
)

install(
  TARGETS
    libreach
  EXPORT
    export_libreach
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  INCLUDES DESTINATION include
)
