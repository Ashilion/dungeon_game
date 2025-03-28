cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include SFML using FetchContent
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Add your source files
set(SOURCES
    src/main.cpp
    src/Maze.cpp
    src/SkillTree.cpp
)

# Add your header files (optional, but good practice)
set(HEADERS
    src/Maze.h
)

# Create the executable
add_executable(main ${SOURCES} ${HEADERS})

# Set C++ standard
target_compile_features(main PRIVATE cxx_std_17)

# Link SFML and other libraries
target_link_libraries(main PRIVATE SFML::Graphics)