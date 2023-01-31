// MD_YM2413 Library example program.
//
// Plays each percussion instrument in turn.
//

#include <MD_YM2413.h>
#include <YM2413Emulator.h>
#include "AudioTools.h"
#include "AudioLibs/AudioKit.h"

// Hardware Definitions ---------------
YM2413Emulator emulator;
// Output 
AudioKitStream kit;
StreamCopy copier(kit, emulator); // copies sound into i2s (both from kit to filtered or filered to kit are supported)

// Global Data ------------------------
MD_YM2413 S(emulator);

// Code -------------------------------
void setup(void)
{
  Serial.begin(115200);
  Serial.println(F("\n[MD_YM2413 Percussion Tester]"));

  S.begin();
  S.setPercussion(true);

  // configure output
  auto cfg = kit.defaultConfig();
  cfg.sample_rate = emulator.sampleRate();
  cfg.channels = emulator.channels();
  cfg.bits_per_sample = emulator.bitsPerSample();
  kit.begin(cfg);

}

void loop(void)
{
  // testing range for percussion channels
  const uint8_t START_CHANNEL = MD_YM2413::CH_HH;    // first percussion channel
  const uint8_t END_CHANNEL = MD_YM2413::CH_BD;     // last percussion channel

  // Timing constants
  const uint16_t PAUSE_TIME = 1000;  // pause between note in ms
  const uint16_t PLAY_TIME = 300;   // note playing time in ms

  // Note on/off FSM variables
  static enum { PAUSE, NOTE_ON, NOTE_OFF } state = PAUSE; // current state
  static uint32_t timeStart = 0;  // millis() timing marker
  static uint8_t chanId = START_CHANNEL;  // the next note to play
  
  S.run(); // run the sound machine every time through loop()

  // Manage the timing of notes on and off depending on 
  // where we are in the rotation/playing cycle
  switch (state)
  {
    case PAUSE: // pause between notes
      if (millis() - timeStart >= PAUSE_TIME)
        state = NOTE_ON;
      break;

    case NOTE_ON:  // play the next MIDI note
      Serial.print(F("\nChan "));
      Serial.print(chanId);
      S.noteOn(chanId, MD_YM2413::MIN_OCTAVE, 0, MD_YM2413::VOL_MAX, PLAY_TIME);

      // wraparound the note number if reached end midi notes
      chanId++;   // only do it once every 2 plays
      if (chanId > END_CHANNEL)
        chanId = START_CHANNEL;

      // next state
      state = NOTE_OFF;
      break;

    case NOTE_OFF:  // wait for note to complete
      if (S.isIdle(0))
      {
        timeStart = millis();
        state = PAUSE;
      }
      break;
  }
  copier.copy(); // Audio output

}