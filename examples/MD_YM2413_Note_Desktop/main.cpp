/**
 * Build the sketch on your desktop with cmake: 
 * - mkdir build
 * - cd build
 * - cmake ..
 * - make
*/
#include <MD_YM2413.h>
#include <YM2413Emulator.h>
#include <iostream>

const uint16_t PLAY_TIME = 0;   // note playing time in ms
const uint8_t CHAN_COUNT = 2; //

YM2413Emulator emulator; // output to emulator
MD_YM2413 S(emulator);

void setup(void){
  S.begin();
  S.setVolume(MD_YM2413::VOL_MAX);
  for (uint8_t i = 0; i < CHAN_COUNT; i++)
    S.noteOn(i, 200, MD_YM2413::VOL_MAX, PLAY_TIME);
}

int main () { 
  int16_t buffer[2];
  setup();
  for(int j=0;j<10000;j++) {
    emulator.readBytes((uint8_t*)buffer, sizeof(int32_t)*2);
    std::cout << buffer[0] << " " << buffer[1] << std::endl;
  }
}	

