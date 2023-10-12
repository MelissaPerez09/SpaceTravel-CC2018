cmake_minimum_required(VERSION 3.10)

# set the project name
project(SR)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Release)

# set the profiling flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# Find SDL2 package
find_package(SDL2 REQUIRED)

# find all .cpp files in the src directory
file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/main.cpp)

# add the executable
add_executable(SR ${SOURCE_FILES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC /opt/homebrew/Cellar/sdl2/2.28.1/include/
    PUBLIC /opt/homebrew/Cellar/glm/0.9.9.8/include
    PUBLIC /opt/homebrew/Cellar/tbb/2021.10.0/include
    PUBLIC /opt/homebrew/Cellar/sdl2_image/2.6.3_2/include/
)

target_link_libraries(${PROJECT_NAME} 
    ${SDL2_LIBRARIES}  # Link with SDL2 libraries
)
