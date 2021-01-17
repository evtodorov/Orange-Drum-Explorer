add_library(solver Solver.cpp)
target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

enable_testing()
add_executable(test_solver test_solver.cpp)
target_link_libraries(test_solver LINK_PUBLIC solver)
add_test(NAME test_solver COMMAND test_solver)