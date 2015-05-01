cmake_minimum_required(VERSION 3.1)
project(simpleton)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES main.cpp EndPoint.cpp EndPoint.h)
add_executable(simpleton ${SOURCE_FILES} Socket.cpp Socket.h BlockingQueue.cpp BlockingQueue.h Exceptions.h Exceptions.cpp ThreadPool.cpp ThreadPool.h Reactor.cpp Reactor.h Dispatcher.cpp Dispatcher.h Multiplexer.cpp Multiplexer.h Buffer.cpp Buffer.h Acceptor.cpp Acceptor.h TcpServer.cpp TcpServer.h TcpConnection.cpp TcpConnection.h)