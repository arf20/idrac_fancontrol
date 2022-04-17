cmake_minimum_required (VERSION 2.8.11)
project(idrac_fancontrol)

set(COMMON_SRC
    "../common/watch.cpp"
    "../common/graph.cpp"
    "../common/exec.cpp"
    "../common/server.cpp"
)

add_subdirectory(temp_monitor)
add_subdirectory(fan_control)
