if (UNIX)
    # Download and unpack googletest-download at configure time
    configure_file(CMakeLists.txt.in ${CMAKE_BINARY_DIR}/test/lib/googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/lib/googletest-download)
    if (result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif ()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test/lib/googletest-download)
    if (result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif ()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest-download directly to our build. This defines
    # the gtest and gtest_main targets.
    #    add_subdirectory(${CMAKE_BINARY_DIR}/test/lib/googletest-src
    #                     ${CMAKE_BINARY_DIR}/test/lib/googletest-build)

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    #if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
    #endif()
endif (UNIX)

if (WIN32)
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    set(TEST_LIBS ${GTEST_LIBRARIES})
endif (WIN32)

find_package(Boost 1.6 REQUIRED COMPONENTS system thread date_time filesystem program_options)
include_directories(${Boost_INCLUDE_DIRS})
set(TEST_LIBS ${TEST_LIBS} ${Boost_LIBRARIES})

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
set(TEST_LIBS ${TEST_LIBS} ${OPENSSL_LIBRARIES})

include_directories("../deps/sdk-dslink-cpp/src/sdk")
include_directories("../deps/sdk-dslink-cpp/src/broker")
include_directories("../deps/sdk-dslink-cpp/include")
include_directories("../deps/sdk-dslink-cpp/deps/beast/include")
include_directories("../src")
include_directories(".")


# TESTS
if (UNIX)
    set(TEST_LIBS gtest_main dsa_sdk ${TEST_LIBS} dl)
else ()
    set(TEST_LIBS gtest_main dsa_sdk ${TEST_LIBS})
endif ()

set(TARGET_NAME "test-system-dslink")

#add_subdirectory("mock_dslink_processes")

add_executable(${TARGET_NAME} system_test.cc 
"../deps/sdk-dslink-cpp/test/sdk/async_test.cc" 
../src/info/info.cc 
../src/info/info_win.cc 
../src/info/info_dslink_node.cc 
../src/process/process_handler.cc)
target_link_libraries(${TARGET_NAME} ${TEST_LIBS})
target_link_libraries(${TARGET_NAME} Threads::Threads)
if (WIN32)
target_link_libraries(${TARGET_NAME} pdh Wbemuuid)
endif ()
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})

#add_dependencies(${TARGET_NAME} hello_mock_link)
#add_dependencies(${TARGET_NAME} world_mock_link)
