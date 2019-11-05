cmake_minimum_required(VERSION 3.12)
project(sreqs_test)
set(CMAKE_CXX_STANDARD 14)

#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF) 
#find_package(Boost 1.71 REQUIRED COMPONENTS program_options filesystem)

#find_package(Poco 1.9 REQUIRED COMPONENTS Foundation)

add_library(model model.cpp)
#target_link_libraries(plugin Boost:boost)
