cmake_minimum_required(VERSION 3.6.2)
project(dslink-cpp-system)

# add sources
set(SOURCES system_main.cc info/info_dslink_node.cc info/info.cc info/info_mac.cc info/info_win.cc process/process_handler.cc)

add_executable(${TARGET_NAME} ${SOURCES})
add_library(system_dslink_lib ${SOURCES})

foreach(TARGET_LINK_NAME ${TARGET_NAME} "system_dslink_lib")
	target_link_libraries(${TARGET_LINK_NAME} ${DSA_SDK_LIBRARY})
	target_link_libraries(${TARGET_LINK_NAME} ${Boost_LIBRARIES})
	target_link_libraries(${TARGET_LINK_NAME} ${OPENSSL_LIBRARIES})
	if (UNIX)
		target_link_libraries(${TARGET_LINK_NAME} dl pthread)
	endif ()

	if (NOT PREBUILT_SDK_PATH)
		add_dependencies(${TARGET_LINK_NAME} ${DSA_SDK_LIBRARY})
	endif ()
endforeach()