add_executable(temp_monitor ${COMMON_SRC} client.cpp main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(temp_monitor PRIVATE Threads::Threads SDL2main SDL2 SDL2_ttf GL)

install(TARGETS temp_monitor DESTINATION bin)