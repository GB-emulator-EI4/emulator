#pragma once

#define LOGGING true

class Logger {
    public:
        // Get instance
        static Logger* getInstance();

        // Destructor
        ~Logger();

        // Functions
        void log(const string &message);
        void error(const string &message);

    private:
        // Instance
        static Logger* instance;

        // Constructors
        Logger();

    protected:

};