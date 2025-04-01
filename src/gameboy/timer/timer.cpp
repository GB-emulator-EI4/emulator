#include <iostream>
#include <stdint.h>
#include <string>

using namespace std;

#include "../utils/utils.hpp"
#include "../logging/logger/logger.hpp"

#include "timer.hpp"
#include "../../gameboy/gameboy.hpp"

// forward declaration
class Gameboy;

/*
    
    Constructors and Destructors

*/

Timer::Timer(Gameboy* gameboy) : gameboy(gameboy), dividerCounter(0), timerCounter(0), timerModulo(0), timerControl(0), timerClockSelect(0) {
    logger = Logger::getInstance()->getLogger("Timer");
    logger->log("Timer Constructor");
}

Timer::~Timer() {
    logger->log("Timer Destructor");
}

/*

    Methods

*/

void Timer::reset() {
    this->dividerCounter = 0;
    this->timerCounter = 0;
    this->timerModulo = 0;
    this->timerControl = 0;
    this->timerClockSelect = 0;
}

void Timer::step(uint8_t cycles) {
    this->updateTimer(cycles);
    this->checkAndTriggerInterrupt();
}

/*

    Getters et setters

*/

void Timer::setDividerRegister(uint8_t value) {
    this->dividerCounter = 0;
}

uint8_t Timer::getDividerRegister() const {
    return this->dividerCounter;
}

void Timer::setTimerCounter(uint8_t val) {
    this->timerCounter = val;
}

uint8_t Timer::getTimerCounter() const {
    return this->timerCounter;
}

void Timer::setTimerModulo(uint8_t val) {
    this->timerModulo = val;
}

uint8_t Timer::getTimerModulo() const {
    return this->timerModulo;
}

void Timer::setTimerControl(uint8_t val) {
    this->timerControl = val;
}

uint8_t Timer::getTimerControl() const {
    return this->timerControl;
}


/*

    Internnal meths

*/


void Timer::updateTimer(uint8_t cycles) {
    this->dividerCounter += cycles; // update div reg


    if(this->dividerCounter >= 256) {
        this->dividerCounter -= 256;
    }

    // timer enabled ? check on third LSB (TAC doc)
    if(!(this->timerControl & 0x04)) return;

    // clock select -> to determine TIMA freq incr -> 2 first LSB of TAC -> determines n° of T cycles after which to increment
    uint8_t frequency = 0;
    switch(this->timerControl & 0x03) {
        case 0x00: frequency = 1024; break;
        case 0x01: frequency = 16; break;
        case 0x02: frequency = 64; break;
        case 0x03: frequency = 256; break;
    }

    // Update timer
    this->timerCounter += cycles;

    if(this->timerCounter >= frequency) {
        this->timerCounter -= frequency;

        // Increment timer
        if (this->timerCounter == 0xFF) {
            this->timerCounter = this->timerModulo;
            this->gameboy->cpu->triggerInterrupt(Interrupt::Timer);
        } else {
            this->timerCounter++;
        }
    }
}

void Timer::checkAndTriggerInterrupt() {
    // Check if timer interrupt is enabled
    if(this->timerControl & 0x04) {
        // Trigger timer interrupt
        this->gameboy->cpu->triggerInterrupt(Interrupt::Timer);
    }
}