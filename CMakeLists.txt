cmake_minimum_required (VERSION 2.8.11)
project(idrac_fancontrol)

set(COMMON_SRC
    "../common/watch.cpp"
    "../common/graph.cpp"
    "../common/exec.cpp"
)

if (NOT WIN32)
    set(COMMON_SRC "../common/server.cpp")
endif()

add_subdirectory(temp_monitor)

if (NOT WIN32)
	add_subdirectory(fan_control)
endif()

install(FILES FSEX300.ttf DESTINATION /usr/share/fonts/truetype/fixedsys/)