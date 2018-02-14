cmake_minimum_required(VERSION 3.6.2)
set(TEST_NAME system_test)

# add sources
set(TEST_SOURCES system_test.cc
        ../deps/sdk-dslink-cpp/test/sdk/async_test.cc
        ../src/info/info.cc
        ../src/info/info_win.cc
        ../src/info/info_mac.cc
        ../src/info/info_dslink_node.cc
        ../src/process/process_handler.cc)

set(TEST_LIBS ${TEST_LIBS} ${GTEST_BOTH_LIBRARIES})
set(TEST_LIBS ${TEST_LIBS} ${Boost_LIBRARIES})
set(TEST_LIBS ${TEST_LIBS} ${OPENSSL_LIBRARIES})
set(TEST_LIBS ${TEST_LIBS} ${DSA_SDK_LIBRARY})

add_executable(${TEST_NAME} ${TEST_SOURCES})

target_link_libraries(${TEST_NAME} ${TEST_LIBS})

if (NOT PREBUILT_SDK_PATH)
    add_dependencies(${TEST_NAME} ${DSA_SDK_LIBRARY})
endif ()