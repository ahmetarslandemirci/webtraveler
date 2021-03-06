cmake_minimum_required(VERSION 2.7)
project(webtraveler CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

include_directories(${CONAN_INCLUDE_DIRS})

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0 -DNDEBUG)


set(TRAVELER_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TRAVELER_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(TRAVELER_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(TRAVELER_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/traveler)

include_directories(${TRAVELER_INSTALL_INCLUDE_DIR})
include_directories(${TRAVELER_HEADERS_DIR})

add_definitions(${CONAN_DEFINES})
link_directories(${CONAN_LIB_DIRS})
include_directories(${CONAN_INCLUDE_DIRS})

IF (USE_MXE)
    IF(DEFINED ENV{MXE_LIB_DIRS})
        set(TRAVELER_LIBS "-liphlpapi")
        link_directories("$ENV{MXE_LIB_DIRS}")
    ELSE()
        message(FATAL_ERROR "Please set env variable for mxe lib dirs which the env name is MXE_LIB_DIRS")
    ENDIF()
ELSE()
    set(TRAVELER_LIBS "")
ENDIF()


add_subdirectory(src)
add_subdirectory(test)