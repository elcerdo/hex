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

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

qt4_wrap_cpp(sources_moc_gui
    viewer.h
    game_qt.h
    player_qt.h
    )

add_executable(hex
    utils.cpp
    main.cpp
    board.cpp
    viewer.cpp
    game.cpp
    game_qt.cpp
    player.cpp
    player_qt.cpp
    player_random.cpp
    graph_uct.cpp
    loop_uct.cpp
    player_uct.cpp
    ${sources_moc_gui}
    )

target_link_libraries(hex
    ${LEMON_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    QtCore QtGui
    )

add_executable(test_uct
    utils.cpp
    test_uct.cpp
    board.cpp
    graph_uct.cpp
    loop_uct.cpp
    )

target_link_libraries(test_uct
    ${LEMON_LIBRARIES}
    )
