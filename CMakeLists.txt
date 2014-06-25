cmake_minimum_required(VERSION 2.8)
project(marp C)

# Set project version information.
set(VERSION_FULL 0.0.1)

# Set configuration options.
option(CAPSICUM "Capsicum sandboxing (FreeBSD 10+)" ON)

# Set compiler to use C99 standard.
set(CMAKE_C_FLAGS "-std=c99")

# Set standard installation paths.
include(GNUInstallDirs)

# Look for required dependencies.
find_package(PkgConfig REQUIRED)
pkg_check_modules(HAMLIB REQUIRED hamlib)

# Check if Capsicum is available.
include(CheckIncludeFiles)

if (CAPSICUM)
    check_include_files(sys/capability.h HAVE_CAPSICUM)
else()
    set(HAVE_CAPSICUM FALSE)
endif()

# Write configuration to header file.
configure_file(
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

# Tell the compiler where to find header and library files.
include_directories(
    ${PROJECT_BINARY_DIR}
    ${HAMLIB_INCLUDE_DIRS}
)

link_directories(
    ${HAMLIB_LIBRARY_DIRS}
)

add_executable(marp
    data.c
    main.c
    receiver.c
    rotator.c
    source_manual.c
    tests.c
)

target_link_libraries(marp ${HAMLIB_LIBRARIES})
