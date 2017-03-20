project(midas)

add_definitions(-DASSETS_FOLDER="${midas_SOURCE_DIR}/assets/" )

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(midas ${SourceFiles})

target_link_libraries(midas ${SDL2_LIBRARY})
target_link_libraries(midas ${SDL2_TTF_LIBRARIES})
target_link_libraries(midas ${SDL2_MIXER_LIBRARIES})

# Build the test
include_directories(midas src/)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

add_executable(midas_test test/midas_test.cpp)
add_dependencies(midas_test catch)
target_link_libraries(midas_test ${SDL2_LIBRARY})
