cmake_minimum_required(VERSION 3.0)

project(vazora_challange)

add_definitions(-DBOOST_LOG_DYN_LINK=1)

find_package(OpenCV 3.1 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system program_options thread log log_setup date_time)

option(ENABLE_VISUAL_DEBUG "Enable visaul debuging mode " OFF)

if(ENABLE_VISUAL_DEBUG)
	add_definitions("-DCVVISUAL_DEBUGMODE")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(
${Boost_INCLUDE_DIRS}
${OpenCV_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
main.cpp
varozacounter.cpp
varozacounter.hpp
log.hpp
log.cpp
common.hpp
common.cpp
)

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${Boost_LIBRARIES}
)
