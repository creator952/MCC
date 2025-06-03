#ifndef MCC_H
#define MCC_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file mcc.h
 * @brief MCC (Mapped Character Compression) audio encoder/decoder for signed 8-bit PCM audio.
 * 
 * MCC compresses signed 8-bit PCM audio into a space-separated string of symbols from
 * a defined printable ASCII set. The output can be safely embedded into source code,
 * JSON, serial communication, or stored in text files.
 * 
 * Each character maps to a specific audio value. This library is suitable for
 * applications like embedded audio storage, compressed string assets, and playback systems.
 */

// -----------------------------------------------------------------------------
// Symbol Set
// -----------------------------------------------------------------------------

/**
 * @brief Number of unique MCC symbols available.
 * Each symbol maps to one signed int8_t value.
 */
#define MCC_SYMBOLS_COUNT 94  ///< Supports ASCII characters from 33 ('!') to 126 ('~')

/**
 * @brief MCC symbol lookup table. These are the printable ASCII characters
 * from '!' (33) to '~' (126), totaling 94 characters. They are used to
 * encode 8-bit signed PCM samples (-128 to 127) into printable, text-friendly format.
 */
extern const char MCC_SYMBOLS[MCC_SYMBOLS_COUNT + 1];  ///< Null-terminated string of valid MCC symbols

// -----------------------------------------------------------------------------
// Encoding Interface
// -----------------------------------------------------------------------------

/**
 * @brief Encode signed 8-bit PCM audio data into an MCC string.
 * 
 * @param input Pointer to an array of signed 8-bit PCM audio samples.
 * @param length Number of samples in the input array.
 * @return A space-separated String of MCC symbols representing the encoded audio.
 */
String encodeMCC(const int8_t* input, size_t length);

/**
 * @brief Internal encoder. Accepts same input but returns the string more directly.
 * Can be used for tighter C++ control.
 */
String mcc_encode(const int8_t* input, size_t length);

// -----------------------------------------------------------------------------
// Decoding Interface
// -----------------------------------------------------------------------------

/**
 * @brief Decode an MCC string back into signed 8-bit PCM audio data.
 * 
 * @param mcc_string Input MCC string (space-separated symbols).
 * @param output Destination buffer to store decoded int8_t PCM values.
 * @param max_output_len Maximum number of elements output[] can hold.
 * @return The number of decoded samples written into the output buffer.
 */
size_t decodeMCC(const String& mcc_string, int8_t* output, size_t max_output_len);

/**
 * @brief Decode an MCC string (C-style const char*) into signed 8-bit PCM.
 * 
 * @param mcc_string Input MCC string (space-separated symbols, null-terminated).
 * @param output Destination buffer to store decoded int8_t PCM values.
 * @param max_output_len Maximum number of elements output[] can hold.
 * @return The number of decoded samples written into the output buffer.
 */
size_t decodeMCC(const char* mcc_string, int8_t* output, size_t max_output_len);

/**
 * @brief Internal decoder (raw version).
 * Can be called from both Arduino or C/C++ code if needed.
 */
size_t mcc_decode(const char* mcc_string, int8_t* output, size_t max_output_len);

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------

/**
 * @brief Validates if a character is part of the MCC symbol set.
 * 
 * @param symbol Character to validate.
 * @return true if the symbol is a valid MCC symbol.
 */
bool isValidMCCSymbol(char symbol);

/**
 * @brief Looks up the MCC symbol corresponding to a given int8_t value.
 * 
 * @param value Signed 8-bit PCM audio sample.
 * @return Corresponding MCC symbol character.
 */
char encodeMCCSymbol(int8_t value);

/**
 * @brief Reverse lookup: Converts a MCC symbol to its corresponding int8_t value.
 * 
 * @param symbol MCC symbol character.
 * @return Signed 8-bit PCM value associated with the symbol.
 *         Returns 0 if symbol is invalid.
 */
int8_t decodeMCCSymbol(char symbol);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // MCC_H
