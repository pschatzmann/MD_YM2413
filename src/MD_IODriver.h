/**
 * @file MD_IODriver.h
 * @author Phil Schatzmann
 * @brief Output to pins or to Emulator. 
 * @version 0.1
 * @date 2023-01-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <stdint.h>
#include <stdio.h>

#define DATA_BITS 8             ///< Number of bits in the byte (for loops)

// On some microcontollers 8 bits might not be good enough so we have a
// central place to change it here 
using pin_t = uint8_t;

/**
 * @brief Abstract output API
 * 
 */
class MD_IODriver {
public:
  MD_IODriver() = default;
  MD_IODriver(const pin_t* D, pin_t we, pin_t a0): D((pin_t*)D), we(we), a0(a0) {};
  virtual void begin() {};
  virtual void send(uint8_t addr, uint8_t data) = 0;
  // Pin numbers
  pin_t *D = nullptr;
  pin_t we; 
  pin_t a0;
};

#ifdef ARDUINO
#include "Arduino.h"
/**
 * @brief Original functionality using the output to the pins
 * of a microcontroller with digitalWrite
 */
class IOPins : public MD_IODriver {
public:
  IOPins() = default;
  IOPins(const pin_t* D, pin_t we, pin_t a0) : MD_IODriver(D, we, a0) {}

  void begin() {
    // Set all pins to outputs and initialize
    for (int8_t i = 0; i < DATA_BITS; i++)
      pinMode(D[i], OUTPUT);
    pinMode(we, OUTPUT);
    pinMode(a0, OUTPUT);
    digitalWrite(we, HIGH);
  }

  void setPins(const pin_t* D, pin_t we, pin_t a0){
    this->D = (pin_t*)D;
    this->we = we;
    this->a0 = a0;
  }

  virtual void send(uint8_t addr, uint8_t data)
  {
    // From the datasheet
    //  /WE A0
    //   1  x  = Write inhibited
    //   0  0  = Write register address
    //   0  1  = Write register content 

    if (_lastAddress != addr)
    {
      // write register address
      digitalWrite(a0, LOW);
      for (uint8_t i = 0; i < DATA_BITS; i++)
        digitalWrite(D[i], (addr & (1 << i)) ? HIGH : LOW);

      // Toggle !WE LOW then HIGH to latch it in the IC
      // wait for 12 master clock cycles (@3.6Mhz ~ 4us)
      digitalWrite(we, LOW);
      delayMicroseconds(4);
      digitalWrite(we, HIGH);

      _lastAddress = addr;    // remember for next time
    }

    digitalWrite(a0, HIGH);
    for (uint8_t i = 0; i < DATA_BITS; i++)
      digitalWrite(D[i], (data & (1 << i)) ? HIGH : LOW);

    // Toggle !WE LOW then HIGH to latch it in the IC
    // wait for 84 master clock cycles (@3.6Mhz ~ 25us)
    digitalWrite(we, LOW);
    delayMicroseconds(25);
    digitalWrite(we, HIGH);
  }

protected:
    uint8_t _lastAddress;     ///< used by send() to remember the last address and not repeat send if same


};

#endif

