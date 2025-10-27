#include <nova/logger/logger.hpp>
#include <thread>

int main() {
    Nova::Logger l("test");
    Nova::Logger _l("tester");
    l.info("test");
    l.info("test");
    l.info("test");
    l.info("test");
    l.error("Ttest");
    _l.info("Testter");
    return 0;
}