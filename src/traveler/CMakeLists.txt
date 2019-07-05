cmake_minimum_required(VERSION 2.7)
project(traveler CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

include_directories(${CONAN_INCLUDE_DIRS})

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0 -DNDEBUG)

link_directories(${CONAN_LIB_DIRS})

add_library(traveler SHARED STATIC Network.cpp Utils.cpp)

add_definitions(${CONAN_DEFINES})

target_link_libraries(traveler ${CONAN_LIBS})


install(TARGETS traveler DESTINATION ${TRAVELER_INSTALL_LIB_DIR})
install(FILES Utils.h Network.h DESTINATION ${TRAVELER_INSTALL_INCLUDE_DIR}/traveler)