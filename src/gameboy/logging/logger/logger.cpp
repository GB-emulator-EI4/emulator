#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

#include "../../../constants/constants.hpp"

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
    if(!ENABLE_LOGGING) return;

    // Check if level is enabled
    if(find(enabledLevels.begin(), enabledLevels.end(), level) == enabledLevels.end()) return;

    // Check if domain is enabled
    if(find(enabledDomains.begin(), enabledDomains.end(), domain) == enabledDomains.end()) return;

    // Check if message contains any word from the word filter
    for(const auto &word : wordFilter) {
        if(message.find(word) != string::npos) return;
    }

    // Log, if domain is error then print in red
    if(level == LOG_ERROR) cout << "\033[1;31mError: " << domain << ": " << message << "\033[0m" << endl;
    else if(level == LOG_WARNING) cout << "\033[1;33mWarning: " << domain << ": " << message << "\033[0m" << endl;
    else cout << "[" + domain + "] " << message << endl;
}

void Logger::setConfig(const vector<char> &enabledLevels, const vector<string> &enabledDomains, const vector<string> &wordFilter) {
    this->enabledLevels = enabledLevels;
    this->enabledDomains = enabledDomains;
    this->wordFilter = wordFilter;
}