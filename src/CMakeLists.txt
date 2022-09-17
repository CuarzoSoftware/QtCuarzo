cmake_minimum_required(VERSION 3.0)

project(platformthemeplugin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui CONFIG REQUIRED Private)

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

target_compile_definitions(cuarzo
    PRIVATE
        "QT_NO_FOREACH"
        "LIB_FM_QT_SONAME=\"${LIB_FM_QT_SONAME}\""
)

target_link_libraries(cuarzo PRIVATE
    Qt6::GuiPrivate
    Qt6::Widgets
    Qt6::Core
    Heaven-Client
)


# execute the command "qmake -query QT_INSTALL_PLUGINS" to get the path of plugins dir.
execute_process(COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT_PLUGINS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(QT_PLUGINS_DIR)
    message(STATUS "Qt plugin directory:" "${QT_PLUGINS_DIR}")
else()
    message(FATAL_ERROR "Qt plugin directory cannot be detected.")
endif()

install(TARGETS cuarzo LIBRARY DESTINATION "${QT_PLUGINS_DIR}/platformthemes")
