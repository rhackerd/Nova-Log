#include "nova/logger/logger.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/pattern_formatter.h>
#include <memory>
#include <sstream>
#include <chrono>
#include <iostream>

namespace Nova::Logger {

static std::shared_ptr<spdlog::logger> s_logger = nullptr;

// Custom formatter flag for 4-character level names
class custom_level_formatter : public spdlog::custom_flag_formatter {
public:
    void format(const spdlog::details::log_msg &msg, const std::tm &, spdlog::memory_buf_t &dest) override {
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

// Custom sink that handles multi-line messages and groups consecutive same-level logs
class multiline_sink : public spdlog::sinks::stdout_color_sink_mt {
private:
    spdlog::level::level_enum last_level_ = spdlog::level::off;
    std::string last_logger_name_;
    std::time_t last_time_ = 0;
    
public:
    void log(const spdlog::details::log_msg &msg) override {
        std::string payload(msg.payload.data(), msg.payload.size());
        
        // Get current time
        std::time_t current_time = std::chrono::system_clock::to_time_t(msg.time);
        
        bool same_level = (msg.level == last_level_ && 
                          std::string(msg.logger_name.data(), msg.logger_name.size()) == last_logger_name_);
        bool same_time = (current_time == last_time_);
        
        // Update tracking
        last_level_ = msg.level;
        last_logger_name_ = std::string(msg.logger_name.data(), msg.logger_name.size());
        last_time_ = current_time;
        
        // Check if message contains newlines
        if (payload.find('\n') != std::string::npos) {
            std::istringstream stream(payload);
            std::string line;
            bool first_line = true;
            
            while (std::getline(stream, line)) {
                if (first_line && !same_level) {
                    // Different level: print full header
                    spdlog::details::log_msg new_msg = msg;
                    new_msg.payload = spdlog::string_view_t(line.data(), line.size());
                    spdlog::sinks::stdout_color_sink_mt::log(new_msg);
                    first_line = false;
                } else if (first_line && same_level && !same_time) {
                    // Same level, different time: print only time + message
                    char time_buf[32];
                    std::tm tm_time;
                    localtime_r(&current_time, &tm_time);
                    std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm_time);
                    // Time is [HH:MM:SS] which is 10 chars, then space, then we skip [Name] LEVEL which is name.size()+2+1+6+1
                    std::cout << "\x1b[90m[\x1b[0m\x1b[90m" << time_buf << "\x1b[0m\x1b[90m]\x1b[0m "
                              << std::string(msg.logger_name.size() + 2 + 1 + 6 + 1, ' ') << line << std::endl;
                    first_line = false;
                } else {
                    // Continuation line: print with full indent
                    std::string indent(10 + 2 + msg.logger_name.size() + 2 + 6 + 1, ' ');
                    std::cout << indent << line << std::endl;
                    first_line = false;
                }
            }
        } else {
            if (same_level && same_time) {
                // Same level and time: indent this message
                std::string indent(10 + 2 + msg.logger_name.size() + 2 + 6 + 1, ' ');
                std::cout << indent << payload << std::endl;
            } else if (same_level && !same_time) {
                // Same level, different time: show time + message
                char time_buf[32];
                std::tm tm_time;
                localtime_r(&current_time, &tm_time);
                std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", &tm_time);
                // Time is [HH:MM:SS] which is 10 chars, then space, then we skip [Name] LEVEL which is name.size()+2+1+6+1
                std::cout << "\x1b[90m[\x1b[0m\x1b[90m" << time_buf << "\x1b[0m\x1b[90m]\x1b[0m "
                          << std::string(msg.logger_name.size() + 2 + 1 + 6 + 1, ' ') << payload << std::endl;
            } else {
                // Different level: print full header
                spdlog::sinks::stdout_color_sink_mt::log(msg);
            }
        }
    }
};

std::shared_ptr<spdlog::logger> get() {
    return s_logger;
}

void init(const std::string& name) {
    // Create custom multiline sink with color support
    auto console_sink = std::make_shared<multiline_sink>();
    
    // Create logger
    s_logger = std::make_shared<spdlog::logger>(name, console_sink);
    s_logger->set_level(spdlog::level::debug);
    
    // Create custom formatter and add our custom flag
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<custom_level_formatter>('*').set_pattern(
        "\x1b[90m[\x1b[0m\x1b[90m%H:%M:%S\x1b[0m\x1b[90m]\x1b[0m "
        "\x1b[37m[\x1b[0m\x1b[1m\x1b[37m%n\x1b[0m\x1b[37m]\x1b[0m "
        "%^%*%$ "
        "%v"
    );
    s_logger->set_formatter(std::move(formatter));
    
    // Set custom colors for levels - inverted (background + foreground)
    console_sink->set_color(spdlog::level::info, "\x1b[42m\x1b[30m");    // Green background, black text
    console_sink->set_color(spdlog::level::warn, "\x1b[43m\x1b[30m");    // Yellow background, black text
    console_sink->set_color(spdlog::level::err, "\x1b[41m\x1b[37m");     // Red background, white text
    console_sink->set_color(spdlog::level::debug, "\x1b[46m\x1b[30m");   // Cyan background, black text
    
    spdlog::register_logger(s_logger);
}

void shutdown() {
    spdlog::shutdown();
}

void info(const std::string& message) {
    if (s_logger) {
        s_logger->info(message);
    }
}

void warn(const std::string& message) {
    if (s_logger) {
        s_logger->warn(message);
    }
}

void error(const std::string& message) {
    if (s_logger) {
        s_logger->error(message);
    }
}

void debug(const std::string& message) {
    if (s_logger) {
        s_logger->debug(message);
    }
}

bool is_initialized() {
    return static_cast<bool>(s_logger);
}

}




// Yes, this is made using AI. I was lazy and making this for Graphics engine.