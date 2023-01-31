#pragma once
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "MD_YM2413.h"
#include "MD_IODriver.h"
#include "emu2413/emu2413.h"

#define MSX_CLK 3579545
#define SAMPLERATE 8000
//44100

#ifdef ARDUINO
#  include "Stream.h"
#else
// dummy class
class Stream{};
#endif

/**
 * @brief YM2413 emulator which provdes the methods from the emulator and 
 * which implements an Arduino Stream so that the
 * generated sound can be read via readBytes().
 * 
 * It implements the MD_IODriver interface, so that it can be passed as argument
 * to the constructor of MD_YM2413 API class.
 */
class YM2413Emulator : public MD_IODriver, public Stream {
public:
  YM2413Emulator(uint32_t sampleRate = SAMPLERATE, uint64_t clock = MSX_CLK){
    this->sample_rate = sampleRate;
    this->clock = clock;
    // setup data pins to make them available to MD_YM2413
    we = WE;
    a0 = A0;
    D = new pin_t[8];
    for (int j=0;j<8;j++){
      D[j] = j;
    }
    // setup opll
  }

  ~YM2413Emulator(){
    if (p_opll) OPLL_delete(p_opll);
    delete[] D;
  }

  void begin() {
    if (p_opll==nullptr){
      p_opll = OPLL_new(clock, sample_rate);
    //OPLL_reset(p_opll);
    }
  }

  void send(uint8_t addr, uint8_t data){
      printf("writeReg(%X, %X)\n", addr, data);
      writeReg(addr, data);
  }
  
  // number of output channels (1 = mono, 2=stereo) of audio result
  uint8_t channels() {
    return 2;
  }

  /// Bits per sample of audio result
  uint32_t bitsPerSample() {
    return sizeof(int16_t)*8;
  }

  /// Sample Rate audo audio output result
  uint32_t sampleRate() {
    return sample_rate;
  }

  void reset(){
      OPLL_reset(p_opll);
  }

  void resetPatch(uint8_t p){
      OPLL_resetPatch(p_opll, p);
  }

  /**
   * Set output wave sampling rate.
   * @param rate sampling rate. If clock / 72 (typically 49716 or 49715 at 3.58MHz) is set, the internal rate converter is
   * disabled.
   */
  void setSampleRate(uint32_t rate){
      sample_rate = rate;
      OPLL_setRate(p_opll,  rate);
  }
  /**
   * Set internal calcuration quality. Currently no effects, just for compatibility.
   * >= v1.0.0 always synthesizes internal output at clock/72 Hz.
   */
  void setQuality(uint8_t q){
      OPLL_setQuality(p_opll,  q);
  }

  /**
   * Set pan pot (extra function - not YM2413 chip feature)
   * @param ch 0..8:tone 9:bd 10:hh 11:sd 12:tom 13:cym 14,15:reserved
   * @param pan 0:mute 1:right 2:left 3:center
   * ```
   * pan: 76543210
   *            |+- bit 1: enable Left output
   *            +-- bit 0: enable Right output
   * ```
   */
  void setPan(uint32_t ch, uint8_t pan){
      OPLL_setPan(p_opll,  ch,  pan);
  }
  /**
   * Set fine-grained panning
   * @param ch 0..8:tone 9:bd 10:hh 11:sd 12:tom 13:cym 14,15:reserved
   * @param pan output strength of left/right channel.
   *            pan[0]: left, pan[1]: right. pan[0]=pan[1]=1.0f for center.
   */
  void setPanFine(uint32_t ch, float pan[2]){
      OPLL_setPanFine(p_opll,  ch,  pan);
  }
  /**
   * Set chip type. If vrc7 is selected, r#14 is ignored.
   * This method not change the current ROM patch set.
   * To change ROM patch set, use OPLL_resetPatch.
   * @param type 0:YM2413 1:VRC7
   */
  void setChipType(uint8_t type){
      OPLL_setChipType(p_opll,  type);
  }

  /**
   * Writes a register or value 
  */
  void writeIO(uint32_t reg, uint8_t val){
      OPLL_writeIO(p_opll,  reg,  val);

  }

  /**
   * Updates a register with the indicated value
  */
  void writeReg( uint32_t reg, uint8_t val){
      OPLL_writeReg(p_opll,  reg,  val);
  }

  /**
   * Defines the actual patch
  */
  void setPatch(const uint8_t *dump){
      OPLL_setPatch(p_opll, dump);
  }

  /**
   * Force to refresh.
   * External program should call this function after updating patch parameters.
   */
  void forceRefresh(){ 
      OPLL_forceRefresh(p_opll);
  }

  /**
   *  Set channel mask
   *  @param mask mask flag: OPLL_MASK_* can be used.
   *  - bit 0..8: mask for ch 1 to 9 (OPLL_MASK_CH(i))
   *  - bit 9: mask for Hi-Hat (OPLL_MASK_HH)
   *  - bit 10: mask for Top-Cym (OPLL_MASK_CYM)
   *  - bit 11: mask for Tom (OPLL_MASK_TOM)
   *  - bit 12: mask for Snare Drum (OPLL_MASK_SD)
   *  - bit 13: mask for Bass Drum (OPLL_MASK_BD)
   */
  uint32_t setMask(uint32_t mask){
      return OPLL_setMask(p_opll, mask);
  }

  /**
   * Toggler channel mask flag
   */
  uint32_t toggleMask(uint32_t mask){
      return OPLL_toggleMask(p_opll,  mask);
  }

  /**
   * @brief Calculates the next mono sample
   * @return int16_t 
   */
  int16_t getSampleMono() {
    return OPLL_calc(p_opll);
  }

  /// @brief Calculates the next stereo samples ()
  /// @param out 
  void getSamplesStereo(int32_t *out) {
    if (p_opll==nullptr){
      out[0]=0;
      out[1]=0;
      printf("p_opll is null!\n");
      return;
    }
    OPLL_calcStereo(p_opll, out);
  }

  /// @brief  Provides a pointer to the OPLL object
  OPLL *opll() {
    return p_opll;
  }

  /// @brief  The default volume might be too low, we allow to boost the volume by a defined factor
  /// @param multiplier 
  void setBoost(uint8_t multiplier){
    boost = multiplier;
  }

  static void dumpToPatch(const uint8_t *dump, OPLL_PATCH *patch){
      OPLL_dumpToPatch(dump, patch);
  }

  static void patchToDump(const OPLL_PATCH *patch, uint8_t *dump){
      OPLL_patchToDump(patch, dump);
  }

  static void getDefaultPatch(int32_t type, int32_t num, OPLL_PATCH *patch){
      OPLL_getDefaultPatch( type,  num, patch);
  }

  // Stream support
  int available() {
    return 1024;
  }

  /**
   * Returns the audio data as byte stream
  */
  size_t readBytes(uint8_t*buffer, size_t len){
    int frames = len / sizeof(int16_t) / 2;
    int32_t out[2];
    int16_t *data = (int16_t*)buffer;
    int pos=0;
    has_sound = false;
    for (int j=0;j<frames;j++){
      getSamplesStereo(out);
      for (int ch=0;ch<2;ch++){
        int32_t tmp = out[ch]*boost;
        assert(tmp>=-32768);
        assert(tmp<=32767);
        if (tmp!=0){
          has_sound = true;
        }
        data[pos++] = out[ch];
      }
    }
    return pos*sizeof(int16_t);
  }

  bool hasSound() {
    return has_sound;
  }

  /// Write is not supported!
  int availableForWrite(){
    return 0;
  }
  /// Write is not supported!
  size_t write(uint8_t) {return 0; }
  /// Read of single bytes is not supported!
  int read() {return -1;};
  /// Read of single bytes is not supported!
  int peek() {return -1;}


protected:
  friend class MD_YM2413;
  enum Functions { D0=0,D1,D2,D3,P4,D5,D7,WE,A0};
  const char *pinNames[9] = {"D0","D1","D2","D3","P4","D5","D7","WE","A0"};
  uint8_t values[9];
  OPLL *p_opll = nullptr;
  uint8_t address = 0;
  uint32_t sample_rate =0;
  uint64_t clock = 0;
  uint8_t boost = 7;
  bool has_sound = false;

};