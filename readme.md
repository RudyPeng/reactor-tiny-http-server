# a simple reactor http server based on tinyhttpd
## Introduction
A learning project when I was looking for an internship, this project can give you a better understanding of socket programming and C++ knowledge

Preparation:
* "TCP/IP Network Programming"
* familiar with tinyhttpd source code
* familiar with Reactor mode of Network programming

Requirement:
* Linux Platform
* C++ version >= 11
* google gflag (**Notice**: gflags compilation is not a shared library by default, you need to ensure that gflags is compiled into a shared library by using `-DBUILD_SHARED_LIBS=ON`)
* google glog
* Cmake >= 3.22
## Usage
    // in root dir
    mkdir build && cd build
    cmake ..
    make
    ./reactor_httpd
## TODO
* replace `std::mutex` with `std::shared_timed_mutex`
* add LOG
* ~~工欲善其事，必先利其器, use Cmake arrange this program~~

## Link
[30 dayas make cpp server](https://github.com/yuesong-feng/30dayMakeCppServer)

[tinyhttpd](https://github.com/cbsheng/tinyhttpd)

[google glog](http://senlinzhan.github.io/2017/10/07/glog/)