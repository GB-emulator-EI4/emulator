#include <string>

using namespace std;

#include "log.hpp"

/*

    Constructors and Destructors

*/

Log::Log(const string &domain) : domain(domain), logger(Logger::getInstance()) {
    logger->log(LOG_LOG, domain, "Log Constructor");
}

Log::~Log() {
    // Nothing to do
}