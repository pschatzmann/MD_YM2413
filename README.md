# MD_YM2413

This is a ym2413 library which is based on the [MD_YM2413 project](https://github.com/MajicDesigns/MD_YM2413) from MajicDesigns. I have added the [YM2413 Emulator](https://github.com/digital-sound-antiques/emu2413) from digital-sound-antiques and modified the original library to support both: the output to Arduino pins and to the emulator. 

The YM2413, OPLL, is a cost-reduced FM synthesis sound chip manufactured by Yamaha Corporation and based on their YM3812 (OPL2).

![YM2413](https://pschatzmann.github.io/MD_YM2413/media/photo/YM2413_IC.png)

The simplifications mean that the YM2413 can only play one user-defined instrument at a time, with an additional 15 read-only hard-coded instrument profiles available. The IC can operate as 9 channels of instruments or 6 channels with melodic instruments and 5 with percussion instruments.

Its main historical application was the generation of music and sound effects in microprocessor systems. It was extensively used in early game consoles, arcade games, home computers and low-cost synthesizer keyboards.

This library implements functions that manage the sound generation interface to the YM2413 IC through a clean API encapsulating the basic functionality provided by the hardware.

## Documentation

The [actual documentation can be found here](https://pschatzmann.github.io/MD_YM2413/docs).

## Examples

To test the functionality of the Emulator, the examples have been extended to output the audio from the Emulator using the [AudioTools](https://github.com/pschatzmann/arduino-audio-tools) project:  The [AudioKit](https://github.com/pschatzmann/arduino-audiokit) is used as output device, but you can replace this with any supported output stream class (e.g. I2SStream, AnalogAudioStream...)

Because of the limited functionality of the Emulator, however most examples do not work, nor do not sound good. So I recommend to stick with the original functionality and use it togheter with a proper YM2413 chip.

## RAM

The emulator allocates a lot of RAM (145.4 KB) dynamically, so you might need to use e.g. an ESP32 with PSRAM.


## Installation in Arduino

You can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone pschatzmann/MD_YM2413.git
```

I recommend to use git because you can easily update to the latest version just by executing the ```git pull``` command in the project folder.


