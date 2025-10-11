#include <nova/logger/logger.hpp>
#include <thread>

int main() {
    Nova::Logger::init("MyApp");
    Nova::Logger::error("First error");
    Nova::Logger::error("Second error");
    Nova::Logger::error("Third error");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    Nova::Logger::error("Fourth error after 1 second");
    Nova::Logger::info("Info message");
    Nova::Logger::shutdown();
    return 0;
}