cmake_minimum_required(VERSION 3.15)
project(jtok LANGUAGES C)

set(CMAKE_C_STANDARD 99) # Sets the C standard to C99

# Options to build extras
option(JTOK_BUILD_TESTS "Build unit tests" OFF)
option(JTOK_BUILD_EXAMPLES "Build usage examples" OFF)

# Library target
add_library(jtok STATIC
        src/tokenizer.c
)

target_include_directories(jtok PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Tests
if(JTOK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(JTOK_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

install(TARGETS jtok
        EXPORT jtokTargets
        ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT jtokTargets
        FILE jtokTargets.cmake
        NAMESPACE jtok::
        DESTINATION lib/cmake/jtok
)