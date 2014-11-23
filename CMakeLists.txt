cmake_minimum_required(VERSION 2.8.11)

project(batteryd)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -O3 ${CMAKE_CXX_FLAGS}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTK2 REQUIRED)
include_directories(${GTK2_INCLUDE_DIRS})

find_package(Libnotify REQUIRED)
include_directories(${LIBNOTIFY_INCLUDE_DIR})

find_package(GLIB2 REQUIRED)
include_directories(${GLIB2_INCLUDE_DIR})

add_executable(batteryd batteryd.cpp)

target_link_libraries(batteryd LINK_PUBLIC ${GTK2_LIBRARIES} ${LIBNOTIFY_LIBRARIES} ${GLIB2_LIBRARIES})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/batteryd DESTINATION bin)
