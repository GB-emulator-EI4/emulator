#pragma once

#include <string>

using namespace std;

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
        inline void log(const string &message) { this->logger->log(LOG_LOG, domain, message); }
        inline void error(const string &message) { this->logger->log(LOG_ERROR, domain, message); }
        inline void warning(const string &message) { this->logger->log(LOG_WARNING, domain, message); }

        // Operator overloads for normal
        inline Log& operator << (const string &message) {
            this->log(message);
            return *this;
        }

        inline Log& operator >> (const string &message) {
            this->error(message);
            return *this;
        }
};