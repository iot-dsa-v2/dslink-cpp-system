cmake_minimum_required(VERSION 3.6.2)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(MACOSX TRUE)
endif ()

if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
        set(CMAKE_CXX_STANDARD 14)
else ()
        set(CMAKE_CXX_STANDARD 11)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DSA_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

if (CMAKE_COMPILER_IS_GNUCC AND NOT CYGWIN)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fstack-protector-all")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")
endif()

set(SOURCES system_main.cc)

#add_executable(systemdslink ${SOURCES})

add_subdirectory("${CMAKE_SOURCE_DIR}/deps/sdk-dslink-cpp")
add_subdirectory("./src")
# add_subdirectory("test")
