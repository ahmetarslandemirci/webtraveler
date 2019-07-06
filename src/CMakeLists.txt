cmake_minimum_required(VERSION 2.7)
project(webtraveler CXX)

add_subdirectory(traveler)

add_executable(webtraveler main.cpp)

target_link_libraries(webtraveler traveler ${CONAN_LIBS} ${TRAVELER_LIBS})

install(TARGETS webtraveler DESTINATION ${TRAVELER_INSTALL_BIN_DIR})
