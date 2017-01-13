cmake_minimum_required(VERSION 2.6)
project(midas)

# Set an output directory for our binaries
set(BIN_DIR ${midas_BUILD_DIR}/)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++1y")
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
   if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
      string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
   else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
   endif()
endif()

# Use our modified FindSDL2* modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${midas_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(midas)
