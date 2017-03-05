project(midas)

add_definitions(-DASSETS_FOLDER="${midas_SOURCE_DIR}/assets/" )

include_directories(${SDL2_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIRS})

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(midas ${SourceFiles})

target_link_libraries(midas ${SDL2_LIBRARY})
target_link_libraries(midas ${SDL2_TTF_LIBRARIES})
target_link_libraries(midas ${SDL2_MIXER_LIBRARIES})

install(TARGETS midas RUNTIME DESTINATION ${BIN_DIR})

# Remove everything below if you do not want to
# setup gtest and do not want any tests
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(midas src/)
add_executable(midas_test test/midas_test.cpp)
target_link_libraries(midas_test ${SDL2_LIBRARY})
target_link_libraries(midas_test ${GTEST_BOTH_LIBRARIES})
add_test(AllTestsInMidasTest midas_test)
