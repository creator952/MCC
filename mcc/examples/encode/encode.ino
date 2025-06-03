#include <Arduino.h>
#include <mcc.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Paste your raw hex bytes here, no casting needed
  uint8_t audioDataUnsigned[] = { 0x00, 0x01, 0xFF, 0xF0, 0xFC, 0xEF, 0x0F, 0x12, 0x34 };
  size_t len = sizeof(audioDataUnsigned);

  // Convert unsigned bytes to signed int8_t
  int8_t audioDataSigned[len];
  for (size_t i = 0; i < len; i++) {
    audioDataSigned[i] = (int8_t)audioDataUnsigned[i];
  }

  char encoded[256];  // output buffer (make large enough for your data)
  int result = mcc_encode(audioDataSigned, len, encoded, sizeof(encoded));

  if (result >= 0) {
    Serial.println("Encoded MCC string:");
    Serial.println(encoded);
  } else {
    Serial.println("Encoding failed: output buffer too small?");
  }
}

void loop() {
  // Nothing here
}
