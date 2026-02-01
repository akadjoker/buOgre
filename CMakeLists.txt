cmake_minimum_required(VERSION 3.20)
cmake_policy(SET CMP0072 NEW)

project(buOgre)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

 
add_subdirectory(libbu/libbu)

add_subdirectory(main)
 

 

 