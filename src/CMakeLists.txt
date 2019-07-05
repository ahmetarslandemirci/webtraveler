cmake_minimum_required(VERSION 2.7)
project(webtraveler CXX)

add_subdirectory(traveler)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0 -DNDEBUG)
link_directories(${CONAN_LIB_DIRS})
include_directories( ${CONAN_INCLUDE_DIRS} )
add_executable(webtraveler main.cpp)
add_definitions(${CONAN_DEFINES})
target_link_libraries(webtraveler traveler ${CONAN_LIBS} )

install(TARGETS webtraveler DESTINATION ${TRAVELER_INSTALL_BIN_DIR})
