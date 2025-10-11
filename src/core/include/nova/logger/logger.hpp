#pragma once
#include <spdlog/spdlog.h>

namespace Nova::Logger {
    std::shared_ptr<spdlog::logger> get();

    void init(const std::string& name);
    void shutdown();

    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

    
};