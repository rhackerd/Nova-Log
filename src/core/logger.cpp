#include "nova/logger/logger.hpp"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#include <chrono>
#include <iostream>
#include <sstream>

// ======================= Custom Formatters =======================

class custom_level_formatter : public spdlog::custom_flag_formatter {
public:
    void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override {
        std::string level_str;
        switch (msg.level) {
            case spdlog::level::info:  level_str = " INFO "; break;
            case spdlog::level::warn:  level_str = " WARN "; break;
            case spdlog::level::err:   level_str = " EROR "; break;
            case spdlog::level::debug: level_str = " DEBG "; break;
            default: level_str = " UNKN "; break;
        }
        dest.append(level_str.data(), level_str.data() + level_str.size());
    }

    std::unique_ptr<custom_flag_formatter> clone() const override {
        return spdlog::details::make_unique<custom_level_formatter>();
    }
};

// ======================= Custom Sink =======================

class multiline_sink : public spdlog::sinks::stdout_color_sink_mt {
private:
    spdlog::level::level_enum last_level_ = spdlog::level::off;
    std::string last_logger_name_;
    std::time_t last_time_ = 0;

public:
    void log(const spdlog::details::log_msg& msg) override {
        std::string payload(msg.payload.data(), msg.payload.size());
        std::time_t current_time = std::chrono::system_clock::to_time_t(msg.time);

        bool same_level = (msg.level == last_level_ && 
                          std::string(msg.logger_name.data(), msg.logger_name.size()) == last_logger_name_);
        bool same_time = (current_time == last_time_);

        last_level_ = msg.level;
        last_logger_name_ = std::string(msg.logger_name.data(), msg.logger_name.size());
        last_time_ = current_time;

        if (payload.find('\n') != std::string::npos) {
            std::istringstream stream(payload);
            std::string line;
            bool first_line = true;

            while (std::getline(stream, line)) {
                if (first_line && !same_level) {
                    spdlog::details::log_msg new_msg = msg;
                    new_msg.payload = spdlog::string_view_t(line.data(), line.size());
                    spdlog::sinks::stdout_color_sink_mt::log(new_msg);
                } else if (first_line && same_level && !same_time) {
                    char time_buf[32];
                    std::tm tm_time;
                    localtime_r(&current_time, &tm_time);
                    std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm_time);
                    std::cout << "\x1b[90m[\x1b[0m\x1b[90m" << time_buf
                              << "\x1b[0m\x1b[90m]\x1b[0m "
                              << std::string(msg.logger_name.size() + 2 + 1 + 6 + 1, ' ')
                              << line << std::endl;
                } else {
                    std::string indent(10 + 2 + msg.logger_name.size() + 2 + 6 + 1, ' ');
                    std::cout << indent << line << std::endl;
                }
                first_line = false;
            }
        } else {
            if (same_level && same_time) {
                std::string indent(10 + 2 + msg.logger_name.size() + 2 + 6 + 1, ' ');
                std::cout << indent << payload << std::endl;
            } else if (same_level && !same_time) {
                char time_buf[32];
                std::tm tm_time;
                localtime_r(&current_time, &tm_time);
                std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm_time);
                std::cout << "\x1b[90m[\x1b[0m\x1b[90m" << time_buf
                          << "\x1b[0m\x1b[90m]\x1b[0m "
                          << std::string(msg.logger_name.size() + 2 + 1 + 6 + 1, ' ')
                          << payload << std::endl;
            } else {
                spdlog::sinks::stdout_color_sink_mt::log(msg);
            }
        }
    }
};

// ======================= Logger Class =======================

namespace Nova {

Logger::Logger(const std::string& name) {
    static std::shared_ptr<multiline_sink> shared_sink = []() {
        auto sink = std::make_shared<multiline_sink>();
        sink->set_color(spdlog::level::info, "\x1b[42m\x1b[30m");
        sink->set_color(spdlog::level::warn, "\x1b[43m\x1b[30m");
        sink->set_color(spdlog::level::err, "\x1b[41m\x1b[37m");
        sink->set_color(spdlog::level::debug, "\x1b[46m\x1b[30m");
        return sink;
    }();

    logger_ = std::make_shared<spdlog::logger>(name, shared_sink);
    logger_->set_level(spdlog::level::debug);

    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<custom_level_formatter>('*').set_pattern(
        "\x1b[90m[\x1b[0m\x1b[90m%H:%M:%S\x1b[0m\x1b[90m]\x1b[0m "
        "\x1b[37m[\x1b[0m\x1b[1m\x1b[37m%n\x1b[0m\x1b[37m]\x1b[0m "
        "%^%*%$ %v"
    );
    logger_->set_formatter(std::move(formatter));

    spdlog::register_logger(logger_);
}

Logger::~Logger() {
    spdlog::drop(logger_->name());
}

void Logger::info(const std::string& message) const { logger_->info(message); }
void Logger::warn(const std::string& message) const { logger_->warn(message); }
void Logger::error(const std::string& message) const { logger_->error(message); }
void Logger::debug(const std::string& message) const { logger_->debug(message); }

std::shared_ptr<spdlog::logger> Logger::get() const { return logger_; }

void Logger::shutdown() {
    spdlog::shutdown();
}

} // namespace Nova
