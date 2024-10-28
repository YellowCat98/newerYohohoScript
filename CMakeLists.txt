cmake_minimum_required(VERSION 3.10)
project(yhs)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB SOURCES
    src/*.cpp
    src/*.hpp
    src/frontend/*.cpp
    src/frontend/*.hpp
    src/runtime/*.cpp
    src/runtime/*.hpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

include(FetchContent)

FetchContent_Declare(
  rift
  GIT_REPOSITORY https://github.com/EclipseMenu/rift
  GIT_TAG        main)
FetchContent_MakeAvailable(rift)

target_link_libraries(${PROJECT_NAME} PRIVATE rift)
