#include <Arduino.h>
#include <mcc.h>

// Example encoded MCC string (matches custom encoding rules)
const char encodedMCC[] = "# % $ & * ! @ 12 34";

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection on some boards

  Serial.println("Encoded MCC string:");
  Serial.println(encodedMCC);

  int8_t decoded[64];  // Buffer for decoded samples
  int count = mcc_decode(encodedMCC, decoded, sizeof(decoded));
  
  if (count < 0) {
    Serial.println("Error decoding MCC string!");
    return;
  }

  Serial.print("Decoded signed 8-bit PCM samples (");
  Serial.print(count);
  Serial.println(" bytes):");

  for (int i = 0; i < count; i++) {
    Serial.print(decoded[i], DEC);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  // Nothing to do here
}
