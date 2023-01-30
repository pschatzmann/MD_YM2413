#pragma once

#include "Stream.h"
#include "MD_IODriver.h"
#include "emu2413/emu2413.h"
#define MSX_CLK 3579545
#define SAMPLERATE 44100

/**
 * @brief YM2413 emulator which provdes the methos from the emulator and 
 * which implements an Arduino Stream so that the
 * generated sound can be read via readBytes().
 * 
 * It implements the MD_IODriver interface, so that it can be passed as argument
 * to the constructor of MD_YM2413 API class.
 */
class YM2413Emulator : public MD_IODriver, public Stream {
public:
  YM2413Emulator(uint32_t sampleRate = SAMPLERATE, int clock = MSX_CLK){
    // setup data pins
    sample_rate = sampleRate;
    we = WE;
    a0 = A0;
    for (int j=0;j<8;j++){
      D[j] = j;
    }
    // setup opll
    p_opll = OPLL_new(clock, sampleRate);
    OPLL_reset(p_opll);
  }

  ~YM2413Emulator(){
    OPLL_delete(p_opll);
  }

  /// Sets the mode for the output pin - does nothing!
  void setMode(uint16_t pin, int function) override{}

  /// @brief Updates the status of the indicated pin (=function)
  /// @param pin 
  /// @param status 
  void write(uint16_t pin, bool status)override{
    // latch output when we is moving from low to high
    bool latch = (pin==we && status && !values[pin]);
    // update pin value
    values[pin] = status;
    if (latch){
      uint8_t value = D[7]<<7 | D[6]<<6 | D[5]<<5 | D[4]<<4 | D[3]<<3 | D[2]<<2 | D[1]<<1 | D[0];
      if (values[A0]){
        // content
        writeReg(address, value);
      } else {
        // address
        address = value;
      }
    }
  }

  uint8_t channels() {
    return 2;
  }

  uint32_t bitsPerSample() {
    return sizeof(int32_t)*8;
  }

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

  void writeIO(uint32_t reg, uint8_t val){
      OPLL_writeIO(p_opll,  reg,  val);

  }

  void writeReg( uint32_t reg, uint8_t val){
      OPLL_writeReg(p_opll,  reg,  val);
  }

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
    OPLL_calcStereo(p_opll, out);
  }

  /// @brief  Provides a pointer to the OPLL object
  OPLL *opll() {
    return p_opll;
  }

  static void dumpToPatch(const uint8_t *dump, OPLL_PATCH *patch){
      OPLL_dumpToPatch(dump, patch);
  }

  static void patchToDump(const OPLL_PATCH *patch, uint8_t *dump){
      OPLL_patchToDump(patch, dump);
  }

  static void OPLL_getDefaultPatch(int32_t type, int32_t num, OPLL_PATCH *patch){
      OPLL_getDefaultPatch( type,  num, patch);
  }

  // Stream support
  int available() {
    return 1024;
  }

  size_t readBytes(uint8_t*buffer, size_t len){
    int frames = len / sizeof(int32_t) / 2;
    int32_t out[2];
    int32_t *data = (int32_t*)buffer;
    int pos=0;
    for (int j=0;j<frames;j++){
      getSamplesStereo(out);
      for (int ch=0;ch<2;ch++){
        data[pos++] = out[ch];
      }
    }
    return pos*sizeof(int32_t);
  }

  int availableForWrite(){
    return 0;
  }
  size_t write(uint8_t) {return 0; }
  int read() {return -1;};
  int peek() {return -1;}


protected:
  enum Functions { D0,D1,D2,D3,P4,D5,D7,WE,A0};
  friend class MD_YM2413;
  uint16_t D[8];
  uint8_t we; 
  uint8_t a0;
  uint8_t values[9];
  OPLL *p_opll;
  uint8_t address = 0;
  uint32_t sample_rate =0;

};