cmake_minimum_required(VERSION 3.0.0)
project(Orange-Drum-Explorer VERSION 0.1.0)

# include(CTest)
# enable_testing()

add_subdirectory(lib)

add_executable(Orange-Drum-Explorer main.cpp)
target_link_libraries(Orange-Drum-Explorer LINK_PUBLIC solver)

# add_executable(test_ODE test.cpp)
# add_test(NAME test_ODE COMMAND test_ODE)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
