cmake_minimum_required(VERSION 3.10)
project(QuickSideScroller CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SDL2 REQUIRED)

add_executable(qss
  main.cpp
  Application.cpp
  Camera.cpp
  InputDevice.cpp
  PhysBody.cpp
  QuadTree.cpp
  Timer.cpp
  Utils.cpp)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
  target_link_libraries(qss PRIVATE OpenMP::OpenMP_CXX)
endif()

if(TARGET SDL2::SDL2)
  target_link_libraries(qss PRIVATE SDL2::SDL2)
  if(TARGET SDL2::SDL2main)
    target_link_libraries(qss PRIVATE SDL2::SDL2main)
  endif()
else()
  target_include_directories(qss PRIVATE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(qss PRIVATE ${SDL2_LIBRARIES})
endif()
