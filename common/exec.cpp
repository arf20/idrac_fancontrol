#include "exec.h"

#include <string>
#include <array>
#include <cstdio>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Execute command, wait for it to finish and return its stdout
std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}