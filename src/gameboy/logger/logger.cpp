#include <iostream>

using namespace std;

#include "logger.hpp"

/*

    Get instance

*/

Logger* Logger::getInstance() {
    if (!instance) instance = new Logger();
    return instance;
}

Logger* Logger::instance = nullptr;

/*

    Constructors and Destructors

*/

Logger::Logger() {
    cout << "Logger Constructor" << endl;
}

Logger::~Logger() {
    instance = nullptr;
    
    cout << "Logger Destructor" << endl;
}

/*

    Functions

*/

void Logger::log(const string &message) {
    cout << "Log: " << message << endl;
}

void Logger::error(const string &message) {
    cout << "\033[1;31mError: " << message << "\033[0m" << endl;
}