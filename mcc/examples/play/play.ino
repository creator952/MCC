#include <BluetoothAudio.h> //not wide working project may not work for yout setup reccomended; specific bluetooth speaker with Pico W
#include <mcc.h>
#include "raw_encoded.h" // MCC-encoded signed 8-bit mono audio

A2DPSource a2dp;

const uint32_t inputSampleRate = 8000;
const uint32_t outputSampleRate = 44100;

int16_t pcm[64 * 2]; // Stereo buffer
int8_t decodedRaw[32768]; // Adjust size to match your max decoded size

volatile bool paused = false;
volatile bool playwav = true;

size_t raw_len = 0;
float resampleRatio = (float)inputSampleRate / outputSampleRate;
double inputPhase = 0.0;

void avrcpCB(void *param, avrcp_operation_id_t op, int pressed) {
  if (pressed && op == AVRCP_OPERATION_ID_PLAY) paused = !paused;
  else if (pressed && op == AVRCP_OPERATION_ID_BACKWARD) playwav = !playwav;
}

void volumeCB(void*, int) {}
void connectCB(void*, bool) {}

void fillPCM() {
  if (paused || raw_len == 0) {
    memset(pcm, 0, sizeof(pcm));
    return;
  }

  for (int i = 0; i < 64; i++) {
    int idx = (int)inputPhase;
    float frac = inputPhase - idx;

    int nextIdx = idx + 1;
    if (nextIdx >= (int)raw_len) nextIdx = 0;

    int8_t s1 = decodedRaw[idx];
    int8_t s2 = decodedRaw[nextIdx];

    float sampleF = s1 * (1.0f - frac) + s2 * frac;
    int16_t sample = (int16_t)(sampleF * 256);

    pcm[i * 2] = sample;
    pcm[i * 2 + 1] = sample;

    inputPhase += resampleRatio;
    if (inputPhase >= raw_len) inputPhase -= raw_len;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Decode MCC audio string to signed 8-bit PCM
  raw_len = decodeMCC(audio_data_mcc, decodedRaw, sizeof(decodedRaw));

  Serial.print("Decoded bytes: ");
  Serial.println(raw_len);

  a2dp.onAVRCP(avrcpCB);
  a2dp.onVolume(volumeCB);
  a2dp.onConnect(connectCB);

  a2dp.begin();
  a2dp.clearPairing();
  a2dp.connect();
}

void loop() {
  if (a2dp.connected()) {
    while ((size_t)a2dp.availableForWrite() >= sizeof(pcm)) {
      fillPCM();
      a2dp.write((const uint8_t *)pcm, sizeof(pcm));
    }
  } else {
    static unsigned long lastTry = 0;
    unsigned long now = millis();
    if (now - lastTry > 5000) {
      a2dp.clearPairing();
      a2dp.connect();
      lastTry = now;
    }
  }
}
