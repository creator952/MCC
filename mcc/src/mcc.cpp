#include "mcc.h"

#include <stdlib.h>
#include <string.h>

#if defined(ARDUINO)
  #include <Arduino.h>
#elif defined(PICO)
  #include "pico/time.h"
  #include "hardware/pwm.h"
  #include "hardware/clocks.h"
  #include "pico/stdlib.h"
#else
  #error "Platform not supported: define ARDUINO or PICO"
#endif

// Internal playback state
static const unsigned char* _audioData = NULL;
static size_t _audioLength = 0;
static uint8_t _outputPin = 0;
static uint8_t _volume = 255;
static uint16_t _sampleRateHz = 8000;
static bool _loop = false;
static volatile bool _isPlaying = false;

#if defined(ARDUINO)
// Arduino timing variables
static unsigned long _lastSampleMillis = 0;
static unsigned int _samplePeriodMs = 0;
static size_t _currentSampleIndex = 0;

#elif defined(PICO)
// Pico timing variables
static absolute_time_t _lastSampleTime;
static uint64_t _samplePeriodUs = 0; // microseconds per sample
static size_t _currentSampleIndex = 0;

// PWM slice number for outputPin
static uint slice_num;
#endif

void mcc_init(void) {
#if defined(ARDUINO)
  // No special init needed, but set sample period
  _samplePeriodMs = 1000 / _sampleRateHz;
#elif defined(PICO)
  // Configure PWM for outputPin - delayed until decode call
#endif

  _isPlaying = false;
  _audioData = NULL;
  _audioLength = 0;
  _currentSampleIndex = 0;
}

void mcc_setSampleRate(uint16_t sampleRateHz) {
  if (sampleRateHz == 0) return; // ignore invalid

  _sampleRateHz = sampleRateHz;

#if defined(ARDUINO)
  _samplePeriodMs = 1000 / _sampleRateHz;
#elif defined(PICO)
  _samplePeriodUs = 1000000 / _sampleRateHz;
#endif
}

uint16_t mcc_getSampleRate(void) {
  return _sampleRateHz;
}

char* mcc_encode(const unsigned char* rawData, size_t length) {
  // For now, no compression, just duplicate buffer as char*
  char* encoded = (char*)malloc(length);
  if (!encoded) return NULL;
  memcpy(encoded, rawData, length);
  return encoded;
}

static void _setPwmValue(uint8_t val) {
  // Scale val 0-255 by _volume 0-255
  uint32_t scaled = (uint32_t)val * _volume / 255;

#if defined(ARDUINO)
  analogWrite(_outputPin, scaled);
#elif defined(PICO)
  // PWM on Pico uses 16-bit range
  uint16_t pwm_val = (scaled * 65535) / 255;
  pwm_set_gpio_level(_outputPin, pwm_val);
#endif
}

#if defined(ARDUINO)
void mcc_decode_tick() {
  if (!_isPlaying) return;

  unsigned long now = millis();
  if (now - _lastSampleMillis < _samplePeriodMs) {
    return; // wait until next sample period
  }
  _lastSampleMillis = now;

  if (_currentSampleIndex >= _audioLength) {
    if (_loop) {
      _currentSampleIndex = 0;
    } else {
      mcc_stop();
      return;
    }
  }

  uint8_t sample = _audioData[_currentSampleIndex++];
  _setPwmValue(sample);
}

#elif defined(PICO)
// Called frequently to update PWM output if playing
void mcc_decode_tick() {
  if (!_isPlaying) return;

  absolute_time_t now = get_absolute_time();
  if (absolute_time_diff_us(_lastSampleTime, now) < _samplePeriodUs) {
    return;
  }
  _lastSampleTime = now;

  if (_currentSampleIndex >= _audioLength) {
    if (_loop) {
      _currentSampleIndex = 0;
    } else {
      mcc_stop();
      return;
    }
  }

  uint8_t sample = _audioData[_currentSampleIndex++];
  _setPwmValue(sample);
}

#endif

int mcc_decode(const unsigned char* rawData, size_t length, uint8_t outputPin, uint8_t loop, uint8_t volume) {
  if (rawData == NULL || length == 0) return -1;

  mcc_stop();

  _audioData = rawData;
  _audioLength = length;
  _outputPin = outputPin;
  _loop = (loop != 0);
  _volume = volume ? volume : 255;
  _currentSampleIndex = 0;

#if defined(ARDUINO)
  pinMode(_outputPin, OUTPUT);
  analogWrite(_outputPin, 0);
#elif defined(PICO)
  gpio_set_function(_outputPin, GPIO_FUNC_PWM);
  slice_num = pwm_gpio_to_slice_num(_outputPin);

  pwm_config config = pwm_get_default_config();
  // Use max wrap for best resolution
  pwm_config_set_wrap(&config, 65535);
  pwm_init(slice_num, &config, true);
#endif

  _isPlaying = true;

  // Initial sample output
  _setPwmValue(_audioData[0]);

  return 0;
}

void mcc_stop(void) {
  if (!_isPlaying) return;

  _isPlaying = false;

#if defined(ARDUINO)
  analogWrite(_outputPin, 0);
#elif defined(PICO)
  pwm_set_gpio_level(_outputPin, 0);
  pwm_set_enabled(slice_num, false);
#endif

  _audioData = NULL;
  _audioLength = 0;
  _currentSampleIndex = 0;
}

int mcc_isPlaying(void) {
  return _isPlaying ? 1 : 0;
}

void mcc_setVolume(uint8_t volume) {
  if (volume == 0) volume = 1; // Avoid mute here, user can call stop
  _volume = volume;

  if (_isPlaying && _audioData != NULL) {
    // Update output immediately with new volume at current sample
    size_t idx = _currentSampleIndex > 0 ? _currentSampleIndex - 1 : 0;
    _setPwmValue(_audioData[idx]);
  }
}

uint8_t mcc_getVolume(void) {
  return _volume;
}
