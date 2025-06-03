#ifndef MCC_H
#define MCC_H

#include <Arduino.h>

/**
 * @brief Encode raw audio data (signed int8_t array) into a space-separated MCC string.
 * @param input Pointer to signed 8-bit audio data.
 * @param length Number of elements in input.
 * @return String containing the encoded MCC data, space-separated.
 */
String mcc_encode(const int8_t* input, size_t length);

/**
 * @brief Decode an MCC-encoded String into raw signed 8-bit audio data.
 *        The MCC string should be space-separated tokens.
 * @param mcc_string MCC-encoded data as a String.
 * @param output Pointer to buffer where decoded data will be stored.
 * @param max_output_len Maximum size of output buffer.
 * @return Number of decoded elements written to output.
 */
size_t decodeMCC(const String& mcc_string, int8_t* output, size_t max_output_len);

/**
 * @brief Strictly checks that MCC string tokens are space-separated only.
 *        Optionally can clean or validate the string for this requirement.
 * @param mcc_string Input MCC string.
 * @return True if string is valid according to strict spacing, false otherwise.
 */
bool isValidMCCString(const String& mcc_string);

/**
 * @brief Replace every 5 occurrences of pattern "#(# # # # #)" in the MCC string with '^'.
 *        This helps compress repetitive patterns.
 * @param mcc_string Input MCC string (modified in place).
 * @return Modified MCC string with replacements.
 */
String replacePatternsWithCaret(const String& mcc_string);

#endif // MCC_H
