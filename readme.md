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
## Usage
    cd src
    make
    ./server
## TODO
replace `std::mutex` with `std::shared_timed_mutex`

## Link
[30 dayas make cpp server](https://github.com/yuesong-feng/30dayMakeCppServer)

[tinyhttpd](https://github.com/cbsheng/tinyhttpd)