#pragma once
#include <memory>
#include <string>
#include <spdlog/spdlog.h>

namespace Nova {

class Logger {
public:
    explicit Logger(const std::string& name);
    ~Logger();

    void info(const std::string& message) const;
    void warn(const std::string& message) const;
    void error(const std::string& message) const;
    void debug(const std::string& message) const;

    std::shared_ptr<spdlog::logger> get() const;

    static void shutdown(); // Global spdlog cleanup

private:
    std::shared_ptr<spdlog::logger> logger_;
};

} // namespace Nova
