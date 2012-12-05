project(PtServo)
cmake_minimum_required(VERSION 2.6)

add_subdirectory(ptproxy)

# ###################################################
# basic example
# ###################################################
add_executable(kbdspeed kbdspeed.cpp)
target_link_libraries(kbdspeed ptproxy)

# ###################################################
# Tracker using PS Move
# ###################################################
include(FindPkgConfig)
pkg_search_module(PSMOVEAPI psmoveapi)

find_package(OpenCV)

if(${PSMOVEAPI_FOUND} AND ${OpenCV_FOUND})
    include_directories(${PSMOVEAPI_INCLUDE_DIRS})
    add_executable(tracker tracker.cpp)
    target_link_libraries(tracker ${PSMOVEAPI_LIBRARIES} ${OpenCV_LIBS} ptproxy)
else(${PSMOVEAPI_FOUND} AND ${OpenCV_FOUND})
    if(NOT ${PSMOVEAPI_FOUND})
        message("PsMoveAPI not found, building without tracker.")
    elseif(NOT ${OpenCV_FOUND})
        message("OpenCV not found, building without tracker.")
    endif()  
endif(${PSMOVEAPI_FOUND} AND ${OpenCV_FOUND}) 