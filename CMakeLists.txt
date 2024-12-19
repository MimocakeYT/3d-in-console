cmake_minimum_required(VERSION 3.16)

project(console_3d LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(console_3d main.cpp)

include(GNUInstallDirs)
install(
  TARGETS console_3d
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
