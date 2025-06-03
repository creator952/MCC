#include "mcc.h"

const char MCC_SYMBOLS[MCC_SYMBOLS_COUNT + 1] =
  "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

String encodeMCC(const int8_t* input, size_t length) {
  return mcc_encode(input, length);
}

String mcc_encode(const int8_t* input, size_t length) {
  if (!input || length == 0) return "";

  String result;
  for (size_t i = 0; i < length; i++) {
    int16_t index = (int16_t)input[i] + 128; // Map from [-128, 127] to [0, 255]
    index = map(index, 0, 255, 0, MCC_SYMBOLS_COUNT - 1);

    // Clamp just in case
    if (index < 0) index = 0;
    if (index >= MCC_SYMBOLS_COUNT) index = MCC_SYMBOLS_COUNT - 1;

    result += MCC_SYMBOLS[index];
    if (i < length - 1) result += ' ';
  }

  return result;
}

size_t decodeMCC(const String& mcc_string, int8_t* output, size_t max_output_len) {
  return mcc_decode(mcc_string.c_str(), output, max_output_len);
}

size_t decodeMCC(const char* mcc_string, int8_t* output, size_t max_output_len) {
  return mcc_decode(mcc_string, output, max_output_len);
}

size_t mcc_decode(const char* mcc_string, int8_t* output, size_t max_output_len) {
  if (!mcc_string || !output || max_output_len == 0) return 0;

  size_t count = 0;
  const char* ptr = mcc_string;

  while (*ptr && count < max_output_len) {
    // Skip whitespace
    while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n' || *ptr == '\r') ptr++;
    if (*ptr == '\0') break;

    char symbol = *ptr++;
    if (!isValidMCCSymbol(symbol)) continue;

    int8_t value = decodeMCCSymbol(symbol);
    output[count++] = value;
  }

  return count;
}

char encodeMCCSymbol(int8_t value) {
  int16_t index = (int16_t)value + 128; // [-128, 127] -> [0, 255]
  index = map(index, 0, 255, 0, MCC_SYMBOLS_COUNT - 1);

  // Clamp for safety
  if (index < 0) index = 0;
  if (index >= MCC_SYMBOLS_COUNT) index = MCC_SYMBOLS_COUNT - 1;

  return MCC_SYMBOLS[index];
}

int8_t decodeMCCSymbol(char symbol) {
  for (int i = 0; i < MCC_SYMBOLS_COUNT; i++) {
    if (MCC_SYMBOLS[i] == symbol) {
      int mapped = map(i, 0, MCC_SYMBOLS_COUNT - 1, 0, 255);
      return (int8_t)(mapped - 128);
    }
  }
  return 0; // Invalid symbol returns silence
}

bool isValidMCCSymbol(char symbol) {
  return (symbol >= '!' && symbol <= '~'); // 33 to 126 ASCII range
}
