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
    game.h
    player_qt.h
    )

add_executable(hex
    main.cpp
    board.cpp
    viewer.cpp
    game.cpp
    player.cpp
    player_qt.cpp
    player_random.cpp
    ${sources_moc_gui}
    )

target_link_libraries(hex
    ${LEMON_LIBRARIES}
    ${Boost_REGEX_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    QtCore QtGui
    )

