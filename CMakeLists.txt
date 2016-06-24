project(hex)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "--std=c++11")

find_package(Boost COMPONENTS
    program_options
    REQUIRED)

find_package(LEMON
    REQUIRED)

include_directories(${LEMON_INCLUDE_DIRS})
link_directories(${LEMON_LIBRARY_DIRS})

add_executable(hex
    main.cpp
    )

target_link_libraries(hex
    ${LEMON_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    )

