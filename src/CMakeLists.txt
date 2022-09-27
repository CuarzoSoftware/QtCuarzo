cmake_minimum_required(VERSION 3.0)

project(platformthemeplugin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 COMPONENTS Core)

if (NOT Qt5_FOUND)
    find_package(Qt6Core REQUIRED)
    find_package(Qt6Widgets REQUIRED)
    find_package(Qt6Gui CONFIG REQUIRED Private)
endif()

if (Qt5_FOUND)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Gui CONFIG REQUIRED Private)
endif()



set (SRCS
    main.cpp
    PlatformTheme.h
    PlatformTheme.cpp
    HNMenuBar.h
    HNMenuBar.cpp
    HNMenu.h
    HNMenu.cpp
    HNMenuItem.h
    HNMenuItem.cpp
    Common.h
)

add_library(cuarzo MODULE ${SRCS})

set_property(TARGET cuarzo PROPERTY CXX_STANDARD 11)

target_compile_definitions(cuarzo
    PRIVATE
        "QT_NO_FOREACH"
        "LIB_FM_QT_SONAME=\"${LIB_FM_QT_SONAME}\""
)

if (NOT Qt5_FOUND)
    target_link_libraries(cuarzo PRIVATE
        Qt6::GuiPrivate
        Qt6::Widgets
        Qt6::Core
        Heaven-Client
    )
endif()

if (Qt5_FOUND)
    target_link_libraries(cuarzo PRIVATE
        Qt5::GuiPrivate
        Qt5::Widgets
        Qt5::Core
        Heaven-Client
    )
endif()

target_include_directories(cuarzo PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Heaven/src/lib>
)

set(CMAKE_INSTALL_RPATH "${HEAVEN_LIBRARY_PATH}")

# execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
#execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
#    OUTPUT_VARIABLE QT_PLUGINS_DIR
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#)
#if(QT_PLUGINS_DIR)
#    message(STATUS "Qt plugin directory:" "${QT_PLUGINS_DIR}")
#else()
#    message(FATAL_ERROR "Qt plugin directory cannot be detected.")
#endif()


set(QT_PLUGINS_DIR /usr/lib/x86_64-linux-gnu/qt5/plugins)
install(TARGETS cuarzo LIBRARY DESTINATION "${QT_PLUGINS_DIR}/platformthemes")
