cmake_minimum_required(VERSION 3.14)

project(xdpcard_server LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS HttpServer)
add_executable(xdpcard_server
  main.cpp
  HttpServer.cpp
  HttpServer.h
)
target_link_libraries(xdpcard_server PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(xdpcard_server PRIVATE Qt6::HttpServer)


include(GNUInstallDirs)
install(TARGETS xdpcard_server
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
