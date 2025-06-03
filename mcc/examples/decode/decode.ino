#include <Arduino.h>
#include <mcc.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Example MCC encoded string (from encode output)
  const char* encoded = "# $ % & * ! @ 00 01 FF F0 FC EF 0F 12 34";

  Serial.println("Encoded MCC string:");
  Serial.println(encoded);

  int8_t decoded[64];  // decoded signed PCM buffer
  int count = mcc_decode(encoded, decoded, sizeof(decoded));

  if (count < 0) {
    Serial.println("Decoding failed!");
    return;
  }

  Serial.println("Decoded signed 8-bit PCM data (hex):");
  for (int i = 0; i < count; i++) {
    // Print hex with leading zeros and uppercase
    if ((uint8_t)decoded[i] < 0x10) Serial.print("0");
    Serial.print((uint8_t)decoded[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  // Nothing here
}
