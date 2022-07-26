cmake_minimum_required(VERSION 3.22)

project(reactor_httpd)

include_directories(./include)

aux_source_directory(./src SRC_DIR)


# don't use find_* before setting project
# https://stackoverflow.com/questions/12264299/cmake-on-linux-target-platform-does-not-support-dynamic-linking
find_package (glog)


add_executable(${PROJECT_NAME} ${SRC_DIR})


target_link_libraries (${PROJECT_NAME} glog::glog)
