#include <string>

using namespace std;

#include "log.hpp"

/*

    Constructors and Destructors

*/

Log::Log(const string &domain) : domain(domain), logger(Logger::getInstance()) {
    logger->log(LOG_LOG ,domain, "Log Constructor");
}

Log::~Log() {
    logger->log(LOG_ERROR, domain, "Log Destructor");
}

/*

    Functions

*/

void Log::log(const string &message) {
    Logger::getInstance()->log(LOG_LOG, domain, message);
}

void Log::error(const string &message) {
    Logger::getInstance()->log(LOG_ERROR, domain, message);
}