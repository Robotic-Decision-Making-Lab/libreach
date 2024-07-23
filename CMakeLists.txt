cmake_minimum_required(VERSION 3.8)
project(
    libreach
    VERSION 0.1.2
    LANGUAGES CXX
    DESCRIPTION "C++ library used to communicate with Reach Robotics devices"
)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include(GNUInstallDirs)

find_package(Boost REQUIRED COMPONENTS system)

add_library(libreach SHARED)
target_sources(
    libreach
    PRIVATE
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

target_include_directories(
    libreach
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(libreach PUBLIC cxx_std_20)
target_link_libraries(libreach PUBLIC Boost::boost PRIVATE Boost::system)
set_target_properties(libreach PROPERTIES PREFIX "")

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
    set_target_properties(
        ${example}
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/examples
    )
endforeach()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    TARGETS libreach
    EXPORT libreachTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/libreachConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    EXPORT libreachTargets
    FILE libreachTargets.cmake
    NAMESPACE libreach::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libreach
)

install(
    FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/libreachConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libreachConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/libreach"
)
