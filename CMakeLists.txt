cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_STANDARD 17)
project(term)

set(CMAKE_BUILD_TYPE RelWithDebInfo)


add_executable(term pwm4term.cpp)


find_package(PkgConfig REQUIRED)
pkg_check_modules(VTE REQUIRED "vte-2.91")

target_link_libraries(term ${VTE_LIBRARIES})
target_include_directories(term PUBLIC ${VTE_INCLUDE_DIRS})
target_compile_options(term PUBLIC ${VTE_CFLAGS_OTHER})
