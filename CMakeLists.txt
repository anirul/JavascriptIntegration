cmake_minimum_required(VERSION 3.14)

project(JavascriptIntegration)

if(WIN32)
	add_compile_options("/std:c++latest")
	add_compile_options("/D _WIN32_WINNT=0x0601")
endif(WIN32)
set(CMAKE_CXX_STANDARD 17)
enable_testing()

find_package(duktape CONFIG REQUIRED)
find_package(Gtest CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${DUKTAPE_INCLUDE_DIRS})

add_library(javascript_lib
	${CMAKE_CURRENT_SOURCE_DIR}/javascript/javascript.h
	${CMAKE_CURRENT_SOURCE_DIR}/javascript/javascript.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/javascript/duktape_impl.h
	${CMAKE_CURRENT_SOURCE_DIR}/javascript/duktape_impl.cpp
)

target_link_libraries(javascript_lib 
  PRIVATE
	${DUKTAPE_LIBRARY}
)

add_executable(javascript_test
	${CMAKE_CURRENT_SOURCE_DIR}/javascript_test/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/javascript_test/javascript_test.h
	${CMAKE_CURRENT_SOURCE_DIR}/javascript_test/javascript_test.cpp
)

target_link_libraries(javascript_test 
  PRIVATE
	GTest::gtest
	GTest::gtest_main
	GTest::gmock
	GTest::gmock_main
	javascript_lib 
	${DUKTAPE_LIBRARY}
)

add_executable(javascript_cmd
	${CMAKE_CURRENT_SOURCE_DIR}/javascript/main.cpp
)

target_link_libraries(javascript_cmd 
  PRIVATE
	javascript_lib
	${DUKTAPE_LIBRARY}
)
