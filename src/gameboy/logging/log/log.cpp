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
    
}

/*

    Functions

*/

void Log::log(const string &message) {
    this->logger->log(LOG_LOG, domain, message);
}

void Log::error(const string &message) {
    this->logger->log(LOG_ERROR, domain, message);
}

void Log::warning(const string &message) {
    this->logger->log(LOG_WARNING, domain, message);
}
