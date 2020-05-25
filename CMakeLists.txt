cmake_minimum_required(VERSION 3.5)

project(streamTracer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)
find_package(VTK)

add_executable(streamTracer
  main.cpp
)
target_link_libraries(streamTracer Qt5::Core ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS streamTracer
  MODULES ${VTK_LIBRARIES}
  )
