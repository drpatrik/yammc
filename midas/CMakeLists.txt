project(midas)

add_definitions(-DASSETS_FOLDER="${midas_SOURCE_DIR}/assets/" )

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(midas ${SourceFiles})

target_link_libraries(midas ${SDL2_LIBRARY})
target_link_libraries(midas ${SDL2_TTF_LIBRARIES})
target_link_libraries(midas ${SDL2_MIXER_LIBRARIES})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(midas -lc++)
  if (UNIX)
    target_link_libraries(midas -lm)
  endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(midas -lstdc++)
  target_link_libraries(midas -lm)
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET midas PROPERTY CXX_STANDARD 17)
endif()

# Build the test
include_directories(midas src/)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

add_executable(midas_test test/midas_test.cpp)
add_dependencies(midas_test catch)
target_link_libraries(midas_test ${SDL2_LIBRARY})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_link_libraries(midas_test -lc++)
  if (UNIX)
    target_link_libraries(midas_test -lm)
  endif()
endif()
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  target_link_libraries(midas_test -lstdc++)
  target_link_libraries(midas_test -lm)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET midas_test PROPERTY CXX_STANDARD 17)
endif()
