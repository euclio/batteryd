cmake_minimum_required(VERSION 2.8.11)

project(batteryd)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Werror -O3 ${CMAKE_CXX_FLAGS}")

find_package(Libnotify REQUIRED)
include_directories(${LIBNOTIFY_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR} ${GTK2_INCLUDE_DIRS})

add_executable(batteryd batteryd.cpp)

target_link_libraries(batteryd LINK_PUBLIC ${LIBNOTIFY_LIBRARIES} ${GLIB2_LIBRARIES} ${GTK2_LIBRARIES})

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/batteryd DESTINATION bin)
