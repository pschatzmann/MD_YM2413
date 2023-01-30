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

/**
 * @brief Abstract output API
 * 
 */
class MD_IODriver {
public:
  virtual void setMode(uint16_t pin, int function);
  virtual void write(uint16_t pin, bool status);
};

/**
 * @brief Original functionality using the output to the pins
 * of a microcontroller with digitalWrite
 */
class IOPins : public MD_IODriver {
public:
  IOPins() = default;
  IOPins(const uint16_t* D, uint8_t we, uint8_t a0) : D(D), we(we), a0(a0) {}

  void setMode(uint16_t pin, int function)override{
    pinMode(pin, function);
  }
  void write(uint16_t pin, bool status)override{
    digitalWrite(pin, status);
  }

  const uint16_t *D;
  uint8_t we; 
  uint8_t a0;

};

