cmake_minimum_required(VERSION 2.7)
project(traveler CXX)

add_library(traveler SHARED STATIC Network.cpp Utils.cpp)

target_link_libraries(traveler ${CONAN_LIBS} ${TRAVELER_LIBS})

install(TARGETS traveler DESTINATION ${TRAVELER_INSTALL_LIB_DIR})
install(FILES Utils.h Network.h DESTINATION ${TRAVELER_INSTALL_INCLUDE_DIR}/traveler)