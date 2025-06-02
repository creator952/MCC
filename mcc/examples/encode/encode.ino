#include <Arduino.h>
#include <mcc.h>

int8_t audioData[] = {
  0x00, 0x01, 0xFF, 0xF0, 0xFC, 0xEF, 0x0F, 0x12, 0x34
};

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial on some boards

  Serial.println("Original audio data:");
  for (int i = 0; i < sizeof(audioData); i++) {
    Serial.print(audioData[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  String encoded = encodeMCC(audioData, sizeof(audioData));
  Serial.println("Encoded MCC format:");
  Serial.println(encoded);//print out encoded
}

void loop() {
  // Nothing here
}
