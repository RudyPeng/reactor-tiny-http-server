cmake_minimum_required(VERSION 3.22)

project(reactor_httpd)

aux_source_directory(./src SRC_DIR)

add_executable(${PROJECT_NAME} ${SRC_DIR})

include_directories(./include)