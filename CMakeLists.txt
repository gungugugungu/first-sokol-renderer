cmake_minimum_required(VERSION 3.22)
project(sokol_renderer)

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED COMPONENTS Xi)

# Find additional X11 components
find_package(PkgConfig REQUIRED)
pkg_check_modules(XCURSOR REQUIRED xcursor)

add_executable(sokol_renderer main.cpp)

# Link against the required libraries
target_link_libraries(sokol_renderer
    PRIVATE
    OpenGL::GL
        X11::Xi
        ${X11_LIBRARIES}
    ${XCURSOR_LIBRARIES}
    dl
    pthread
)

# Include directories
target_include_directories(sokol_renderer
    PRIVATE
    ${X11_INCLUDE_DIR}
    ${XCURSOR_INCLUDE_DIRS}
)