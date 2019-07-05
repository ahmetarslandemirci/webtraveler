cmake_minimum_required(VERSION 2.7)
project(traveler_tests)


include_directories(${TRAVELER_HEADERS_DIR})

set(SOURCE_FILES test_main.cpp src/network_tests.cpp)

add_executable(traveler_tests ${SOURCE_FILES})

target_link_libraries(traveler_tests traveler ${CONAN_LIBS})

install(TARGETS traveler_tests DESTINATION bin)