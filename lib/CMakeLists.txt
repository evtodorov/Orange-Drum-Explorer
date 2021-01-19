add_library(solver Solver.cpp)
target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(solver PUBLIC ext/adept)
# Use bundled version of Eigen
target_include_directories(solver PUBLIC ext/eigen)
# Uncomment below to use an installed version of Eigen
#find_package (Eigen3 3.0 REQUIRED NO_MODULE)
#target_link_libraries (solver Eigen3::Eigen)


enable_testing()
add_executable(test_solver test_solver.cpp)
target_link_libraries(test_solver LINK_PUBLIC solver)
add_test(NAME test_solver COMMAND test_solver)

add_executable(test_external test_external.cpp)
target_include_directories(test_external PUBLIC ext/adept)
add_compile_definitions("ADEPT_RECORDING_PAUSABLE")
# Use bundled version of Eigen
target_include_directories(test_external PUBLIC ext/eigen)
# Uncomment below to use an installed version of Eigen
#find_package (Eigen3 3.0 REQUIRED NO_MODULE)
#target_link_libraries (test_external Eigen3::Eigen)

add_test(NAME test_external COMMAND test_external)