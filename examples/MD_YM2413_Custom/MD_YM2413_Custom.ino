// MD_YM2413 Library example program.
//
// Example program for the MD_YM2413 library.
//
// Allows interactive CLI setting of 'canned' MIDI instrument parameters 
// for exploring what effect they can produce.
//
// MIDI instruments and drums are define in OPL2 format in the 
// midi_drums and midi_instruments header files, respectively.
//
// Library Dependencies
// MD_MusicTable library located at https://github.com/MajicDesigns/MD_MusicTable
// MD_cmdProcessor library located at https://github.com/MajicDesigns/MD_cmdProcessor
//

#include <MD_YM2413.h>
#include <MD_MusicTable.h>
#include <MD_cmdProcessor.h>
#include <YM2413Emulator.h>
#include "midi_instruments.h"
#include "midi_drums.h"
#include "AudioTools.h"
#include "AudioLibs/AudioKit.h"

// Hardware Definitions ---------------
YM2413Emulator emulator;

// Output 
AudioKitStream kit;
StreamCopy copier(kit, emulator); // copies sound into i2s (both from kit to filtered or filered to kit are supported)

// Miscellaneous
const uint8_t RCV_BUF_SIZE = 50;      // UI character buffer size
void(*hwReset) (void) = 0;            // declare reset function @ address 0

// Global Data ------------------------
MD_YM2413 S(emulator);
MD_MusicTable T;

char rcvBuf[RCV_BUF_SIZE];  // buffer for characters received from the console

uint16_t timePlay = 1500;    // note playing time in ms
uint16_t volume = MD_YM2413::VOL_MAX; // note playing volume

const uint8_t CHANNEL = 0;   // Channel being exercised

// Code -------------------------------
void handlerHelp(char* param); // function prototype only

char *getNum(uint16_t &n, char *psz)
{
  n = 0;

  while (*psz >= '0' && *psz <= '9')
  {
    n = (n * 10) + (*psz - '0');
    psz++;
  }
  
  if (*psz != '\0') psz--;
  
  return(psz);
}

void handlerZ(char* param) { hwReset(); }
      
void handlerP(char *param)
// Play note
{
  uint16_t midiNote;

  param = getNum(midiNote, param);

  if (T.findId(midiNote));
  {
    uint16_t f = (uint16_t)(T.getFrequency() + 0.5);  // round it up
    char buf[10];

    Serial.print(F("\n>Play "));
    Serial.print(midiNote);
    Serial.print(" (");
    Serial.print(T.getName(buf, sizeof(buf)));
    Serial.print(F(") @ "));
    Serial.print(f);
    Serial.print(F("Hz"));
    S.noteOn(CHANNEL, f, volume, timePlay);
  }
}

void handlerV(char* param)
// Channel Volume
{
  param = getNum(volume, param);
  if (volume > MD_YM2413::VOL_MAX)
    volume = MD_YM2413::VOL_MAX;
  Serial.print("\n>Volume ");
  Serial.print(volume);
}

void handlerT(char *param)
// time duration
{
  param = getNum(timePlay, param);
  Serial.print("\n>Time ");
  Serial.print(timePlay);
}

void handlerLI(char* param)
{
  uint16_t inst;

  param = getNum(inst, param);
  inst &= 127;  // ensure in range
  Serial.print("\n>Instrument ");
  Serial.print(inst);
  S.loadInstrumentOPL2(midiInstruments[inst], true);
}

void handlerLD(char* param)
{
  uint16_t drum;

  param = getNum(drum, param);
  // ensure in range
  if (drum < DRUM_NOTE_BASE) drum = DRUM_NOTE_BASE;
  if (drum >= DRUM_NOTE_BASE + NUM_MIDI_DRUMS) drum = DRUM_NOTE_BASE + NUM_MIDI_DRUMS - 1;
  Serial.print("\n>Instrument ");
  Serial.print(drum);
  S.loadInstrumentOPL2(midiDrums[drum], true);
}

const MD_cmdProcessor::cmdItem_t PROGMEM cmdTable[] =
{
  { "h",  handlerHelp, "", "Show this help" },
  { "?",  handlerHelp, "", "Show this help" },
  { "v",  handlerV,   "v", "Set channel volume to v [0..15]" },
  { "t",  handlerT,   "t", "Set play time duration to t ms" },
  { "li", handlerLI,  "n", "Load MIDI instrument n [0..127]"},
  { "ld", handlerLD,  "n", "Load MIDI drum n [27 .. 86]"},
  { "p",  handlerP,   "m", "Play MIDI Note m" },
  { "z",  handlerZ,    "", "Software reset" },
};

MD_cmdProcessor CP(Serial, cmdTable, ARRAY_SIZE(cmdTable));

void handlerHelp(char* param) { CP.help(); }

void setup(void)
{
  Serial.begin(57600);
  S.begin();
  S.setVolume(CHANNEL, volume);
  S.setInstrument(CHANNEL, MD_YM2413::I_CUSTOM);
  S.loadInstrumentOPL2(midiInstruments[0]);

  CP.begin();

  Serial.print(F("\n[MD_YM2413 Custom]"));
  Serial.print(F("\nEnsure serial monitor line ending is set to newline."));
  handlerHelp(nullptr);

  // configure output
  auto cfg = kit.defaultConfig();
  cfg.sample_rate = emulator.sampleRate();
  cfg.channels = emulator.channels();
  cfg.bits_per_sample = emulator.bitsPerSample();
  kit.begin(cfg);
}

void loop(void)
{
  S.run();      // run the sound machine every time through loop()
  CP.run();     // check the user input
  copier.copy(); // Audio output
}