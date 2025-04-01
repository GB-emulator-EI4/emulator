#pragma once

#include <stdint.h>
#include <vector>

#include "../logging/log/log.hpp"

#include "../gameboy.hpp"

// Forward declaration
class Gameboy;


// Add your includes and namespace declarations here


class Timer {

    public:

        Timer(Gameboy& gameboy);
        ~Timer();
    
        void reset();
        void step(uint8_t cycles);
        void setDividerRegister(uint8_t value);
        uint8_t getDividerRegister() const;
        void setTimerCounter(uint8_t value);
        uint8_t getTimerCounter() const;
        void setTimerModulo(uint8_t value);
        uint8_t getTimerModulo() const;
        void setTimerControl(uint8_t value);
        uint8_t getTimerControl() const;


    private:
    
        Gameboy& gameboy;
        uint16_t dividerCounter;
        uint16_t timerCounter;
        uint8_t timerModulo;
        uint8_t timerControl;
        uint8_t timerClockSelect;

        Log* logger;

        void updateTimer(uint8_t cycles);
        void checkAndTriggerInterrupt();

};
