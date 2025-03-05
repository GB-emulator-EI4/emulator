#pragma once

#include <string>

using namespace std;

class Logger;
#include "../logger/logger.hpp"

class Log {
    private:
        // Domain the log is for
        string domain;

        // Reference to the Logger class
        Logger* logger;

    public:
        // Constructor
        Log(const string &domain);

        // Destructor
        ~Log();

        // Functions
        void log(const string &message);
        void error(const string &message);
};