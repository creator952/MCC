#include <Arduino.h>
#include <mcc.h>

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial on boards like Leonardo, Pico, etc.

  // MCC-encoded example string
  String encoded = "# % $ & * ! @ 12 34";

  Serial.println("Encoded MCC string:");
  Serial.println(encoded);

  int8_t decoded[64];  // Output buffer
  size_t count = decodeMCC(encoded, decoded, sizeof(decoded));

  Serial.println("Decoded int8_t PCM data:");
  for (size_t i = 0; i < count; i++) {
    Serial.print(decoded[i], DEC);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  // Nothing here
}
