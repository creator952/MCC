#ifndef MCC_H
#define MCC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

//
// PLATFORM DETECTION
//
#if defined(ARDUINO)
  #include <Arduino.h>
#elif defined(PICO)
  #include "pico/stdlib.h"
  #include "hardware/pwm.h"
#else
  #warning "Platform not explicitly supported. Please define ARDUINO or PICO."
#endif

//
// MCC - Mono 8-bit PCM Audio Codec & Player Interface
//

// Initialize MCC audio system, call before playback or encoding
void mcc_init(void);

// Set the sample rate in Hz for encoding and playback.
// Common values: 8000, 16000, 22050, 44100, 48000
void mcc_setSampleRate(uint16_t sampleRateHz);

// Get the current sample rate set by mcc_setSampleRate()
uint16_t mcc_getSampleRate(void);

// Encode raw signed 8-bit PCM audio (stored as unsigned bytes, 0x00 to 0xFF)
// rawData: pointer to raw PCM samples
// length: number of samples
// Returns pointer to newly allocated encoded data buffer (must be freed by caller)
// (For now this can be a pass-through, extend with compression later)
char* mcc_encode(const unsigned char* rawData, size_t length);

// Decode/play raw signed 8-bit PCM audio stored as unsigned bytes (0x00-0xFF).
// rawData: pointer to raw PCM samples
// length: number of samples
// outputPin: Arduino pin number or Pico GPIO for PWM output
// loop: if non-zero, loops playback continuously
// volume: 0 (mute) to 255 (max volume)
// Returns 0 on success, non-zero on error
int mcc_decode(const unsigned char* rawData, size_t length, uint8_t outputPin, uint8_t loop, uint8_t volume);

// Stops playback if currently playing asynchronously
void mcc_stop(void);

// Query if playback is currently running (non-zero = playing)
int mcc_isPlaying(void);

// Set volume for playback (0-255)
void mcc_setVolume(uint8_t volume);

// Get current playback volume (0-255)
uint8_t mcc_getVolume(void);

#ifdef __cplusplus
}
#endif

#endif // MCC_H
