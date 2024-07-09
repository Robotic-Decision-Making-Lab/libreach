include(CMakeFindDependencyMacro)

find_dependency(Boost REQUIRED COMPONENTS system)

include(${CMAKE_CURRENT_LIST_DIR}/libreachTargets.cmake)
