#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

#include "logger.hpp"

/*

    Get instance

*/

Logger* Logger::getInstance() {
    if(!instance) instance = new Logger();
    return instance;
}

Logger* Logger::instance = nullptr;

/*

    Constructors and Destructors

*/

Logger::Logger() {
    
}

Logger::~Logger() {
    instance = nullptr;
}

/*

    Functions

*/

Log* Logger::getLogger(const string &domain) {
    return new Log(domain);
}

void Logger::log(const char &level, const string &domain, const string &message) {
    // Check if logging is enabled
    if(!enableLogging) return;

    // Check if level is enabled
    if(find(enabledLevels.begin(), enabledLevels.end(), level) == enabledLevels.end()) return;

    // Check if domain is enabled
    if(find(enabledDomains.begin(), enabledDomains.end(), domain) == enabledDomains.end()) return;

    // Log, if domain is error then print in red
    if(level == LOG_ERROR) cout << "\033[1;31mError: " << domain << ": " << message << "\033[0m" << endl;
    else cout << "[" + domain + "] " << message << endl;
}

void Logger::setConfig(const bool &enableLogging, const vector<char> &enabledLevels, const vector<string> &enabledDomains) {
    this->enableLogging = enableLogging;
    this->enabledLevels = enabledLevels;
    this->enabledDomains = enabledDomains;
}