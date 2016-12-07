project(midas)

add_definitions(-DASSETS_FOLDER="${midas_SOURCE_DIR}/assets/" )

file(GLOB_RECURSE SourceFiles src/*.cpp)
add_executable(midas ${SourceFiles})

target_link_libraries(midas ${SDL2_LIBRARY})
target_link_libraries(midas ${SDL2_TTF_LIBRARIES})
install(TARGETS midas RUNTIME DESTINATION ${BIN_DIR})

enable_testing()
set(GTEST_ROOT ../gtest-1.7.0)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(midas src/)
add_executable(midas_test test/midas_test.cpp)
target_link_libraries(midas_test ${SDL2_LIBRARY})
target_link_libraries(midas_test ${GTEST_BOTH_LIBRARIES})
add_test(AllTestsInMidasTest midas_test)
