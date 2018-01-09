cmake_minimum_required(VERSION 3.6.2)
project(dslink-cpp-system)

## OpenSSL v1.0.2l
if (MACOSX)
	set(OPENSSL_ROOT_DIR "/usr/local/Cellar/openssl/1.0.2l")
elseif (UNIX)
	set(OPENSSL_ROOT_DIR "/usr/local/ssl")
endif ()
find_package(OpenSSL REQUIRED)
include_directories(SYSTEM ${OPENSSL_INCLUDE_DIR})

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(DSA_SDK_SOURCE ${PROJECT_SOURCE_DIR}/../deps/sdk-dslink-cpp/src)
set(DSA_SDK_INCLUDE ${PROJECT_SOURCE_DIR}/../deps/sdk-dslink-cpp/include)
set(DSA_SDK_DEPS ${PROJECT_SOURCE_DIR}/../deps/sdk-dslink-cpp/deps)

include_directories(${PROJECT_INCLUDE_DIR})
include_directories(${DSA_SDK_DEPS}/msgpack/include)
include_directories(${DSA_SDK_DEPS}/jansson-build/include)

## Boost Library v1.66
find_package(Threads)
set(EXTRA_LIBS ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})
find_package(Boost 1.66 REQUIRED COMPONENTS system thread date_time program_options filesystem)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

find_package(Threads REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

set(SOURCES system_main.cc info/info_dslink_node.cc info/info.cc process/process_handler.cc)

include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("./include")
include_directories("${DSA_SDK_SOURCE}/sdk")
include_directories("${DSA_SDK_INCLUDE}")

add_executable(system_dslink ${SOURCES})
add_library(system_dslink_lib ${SOURCES})


target_link_libraries(system_dslink dsa_sdk)
target_link_libraries(system_dslink Threads::Threads)
target_link_libraries(system_dslink ${Boost_LIBRARIES})
target_link_libraries(system_dslink crypto)

add_dependencies(system_dslink broker)

