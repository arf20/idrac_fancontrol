add_executable(temp_monitor main.cpp graph.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(temp_monitor PRIVATE Threads::Threads SDL2main SDL2)