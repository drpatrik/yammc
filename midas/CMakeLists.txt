project(midas)

add_definitions(-DASSETS_FOLDER="${midas_SOURCE_DIR}/assets/" )

# midas
add_executable(midas src/midas.cpp src/board.cpp src/asset_manager.cpp)
target_link_libraries(midas ${SDL2_LIBRARY})
target_link_libraries(midas -lSDL2_ttf)
install(TARGETS midas RUNTIME DESTINATION ${BIN_DIR})

# midas_test
set(GTEST_ROOT ../gtest-1.7.0)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(midas src/)
add_executable(midas_test test/midas_test.cpp)
target_link_libraries(midas_test ${GTEST_BOTH_LIBRARIES})
add_test(AllTestsInMidasTest midas_test)
