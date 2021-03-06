project(hex)

cmake_minimum_required(VERSION 2.6)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_FLAGS "--std=c++11")

set(Boost_PYTHON_NAME python3)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS
    ${Boost_PYTHON_NAME}
    program_options
    REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
set(Boost_PYTHON_LIBRARY boost_${Boost_PYTHON_NAME})

find_package(PythonLibs 3.5 EXACT REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

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

set_target_properties(hex_uct PROPERTIES POSITION_INDEPENDENT_CODE ON)

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

add_library(hex_uct_ext SHARED
    hex_uct_ext.cpp
    )

target_link_libraries(hex_uct_ext
    ${Boost_PYTHON_LIBRARY}
    ${PYTHON_LIBRARY}
    hex_uct
    )

