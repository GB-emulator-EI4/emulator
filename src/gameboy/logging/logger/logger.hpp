#pragma once

#include <string>
#include <vector>

using namespace std;

class Log;
#include "../log/log.hpp"

#define LOG_LOG 1
#define LOG_ERROR 2

class Logger {
    public:
        // Get instance
        static Logger* getInstance();

        // Destructor
        ~Logger();

        // Functions
        Log* getLogger(const string &domain);

        void log(const char &level, const string &domain, const string &message);

        void setConfig(const bool &enableLogging, const vector<char> &enabledLevels, const vector<string> &enabledDomains);

    private:
        // Instance
        static Logger* instance;

        // Constructors
        Logger();

        // Vars
        bool enableLogging = false;

        vector<char> enabledLevels;
        vector<string> enabledDomains;

    protected:

};