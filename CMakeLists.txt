cmake_minimum_required(VERSION 2.7)
project(webtraveler CXX)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)

include_directories(${CONAN_INCLUDE_DIRS})
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
link_directories(${CONAN_LIB_DIRS})

add_executable(webtraveler main.cpp Utils.cpp )
add_definitions(${CONAN_DEFINES})
target_link_libraries(webtraveler ${CONAN_LIBS} )
