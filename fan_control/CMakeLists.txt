add_executable(fan_control ${COMMON_SRC} control.cpp main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(fan_control PRIVATE Threads::Threads SDL2main SDL2 SDL2_ttf GL)

install(TARGETS fan_control DESTINATION bin)