cmake_minimum_required(VERSION 3.1)
project(simpleton)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp EndPoint.cpp EndPoint.h)
add_executable(simpleton ${SOURCE_FILES} Socket.cpp Socket.h BlockingQueue.cpp BlockingQueue.h Exceptions.h Exceptions.cpp ThreadPool.cpp ThreadPool.h)