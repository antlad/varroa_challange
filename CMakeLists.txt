cmake_minimum_required(VERSION 3.0)

project(vazora_challange)


add_definitions(-DBOOST_LOG_DYN_LINK=1)
set(OpenCV_STATIC ON)

find_package(OpenCV 3.1 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system program_options thread log log_setup date_time thread)
find_package(OpenMP)

if (OPENMP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

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
extract_background.cpp
extract_background.hpp
json.hpp
)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin)

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${Boost_LIBRARIES}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set (VERSION "0.1")
	set(CPACK_PACKAGE_VERSION ${VERSION})
	set(CPACK_DEBIAN_PACKAGE_VERSION ${VERSION})
	set(CPACK_PACKAGE_NAME "vazora_challange")
	set(CPACK_GENERATOR "DEB")
	set(CPACK_DEBIAN_PACKAGE_DEBUG ON)
	set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Vladislav Troinich")

	set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

	set(CPACK_PACKAGE_DESCRIPTION "create_route short description")
	set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "create_route long description")
	set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
	include(CPack)
endif()
