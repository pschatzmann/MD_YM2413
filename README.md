# MD_YM2413

This is a ym2413 library which is based on the [MD_YM2413 project](https://github.com/MajicDesigns/MD_YM2413) from MajicDesigns. I have added the [YM2413 Emulator](https://github.com/digital-sound-antiques/emu2413) from digital-sound-antiques and modified the original library to support both: the output to Arduino pins and to the emulator. 

The YM2413, OPLL, is a cost-reduced FM synthesis sound chip manufactured by Yamaha Corporation and based on their YM3812 (OPL2).

The simplifications mean that the YM2413 can only play one user-defined instrument at a time, with an additional 15 read-only hard-coded instrument profiles available. The IC can operate as 9 channels of instruments or 6 channels with melodic instruments and 5 with percussion instruments.

Its main historical application was the generation of music and sound effects in microprocessor systems. It was extensively used in early game consoles, arcade games, home computers and low-cost synthesizer keyboards.

This library implements functions that manage the sound generation interface to the YM2413 IC through a clean API encapsulating the basic functionality provided by the hardware.

## Examples

The examples have been extended to output the audio from the Emulator using the AudioTools project.  A AudioKit is used, but you can replace this with any supported output stream class (e.g. I2SStream, AnalogAudioStream...)

## RAM

The emulator uses a lot of RAM, so you need to have e.g. an ESP32 with PSRAM.
