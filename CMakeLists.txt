cmake_minimum_required(VERSION 3.10.0)
project(Orange-Drum-Explorer VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

add_subdirectory(lib)

add_executable(Orange-Drum-Explorer main.cpp)
target_link_libraries(Orange-Drum-Explorer LINK_PUBLIC solver)



# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
