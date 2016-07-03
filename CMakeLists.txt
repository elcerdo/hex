project(hex)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "--std=c++11")

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS
    program_options
    REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

find_package(LEMON
    REQUIRED)
include_directories(${LEMON_INCLUDE_DIRS})
link_directories(${LEMON_LIBRARY_DIRS})

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

add_library(hex_uct STATIC
    utils.cpp
    board.cpp
    game.cpp
    player.cpp
    player_random.cpp
    graph_uct.cpp
    loop_uct.cpp
    player_uct.cpp
    )

target_link_libraries(hex_uct
    ${LEMON_LIBRARIES}
    )

qt4_wrap_cpp(sources_moc_gui
    viewer.h
    game_qt.h
    player_qt.h
    )

add_executable(hex
    hex.cpp
    viewer.cpp
    game_qt.cpp
    player_qt.cpp
    ${sources_moc_gui}
    )

target_link_libraries(hex
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    QtCore QtGui
    hex_uct
    )

add_executable(test_uct
    test_uct.cpp
    )

target_link_libraries(test_uct
    hex_uct
    )

add_executable(stress_uct
    stress_uct.cpp
    )

target_link_libraries(stress_uct
    hex_uct
    )

