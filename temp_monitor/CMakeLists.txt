add_executable(temp_monitor main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(temp_monitor PRIVATE Threads::Threads)