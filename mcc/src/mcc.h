#ifndef MCC_H
#define MCC_H

#include <Arduino.h>

// --- User-defined audio sample rate ---
extern uint32_t mcc_sampleRate;

/**
 * @brief Set the sample rate (Hz) for MCC audio playback.
 * 
 * @param hz The sample rate in Hz (e.g., 8000, 16000, 44100).
 */
void mcc_setSampleRate(uint32_t hz);

/**
 * @brief Encode a signed 8-bit PCM sample into a single MCC character.
 * 
 * @param pcm A signed 8-bit audio sample.
 * @return char MCC-encoded character.
 */
char mcc_encode(int8_t pcm);

/**
 * @brief Decode a single MCC character back to a signed 8-bit PCM sample.
 * 
 * @param c MCC character.
 * @return int8_t Decoded signed 8-bit PCM sample.
 */
int8_t mcc_decode(char c);

/**
 * @brief Encode an array of signed 8-bit PCM audio to MCC string.
 * 
 * @param pcm Pointer to signed 8-bit PCM array.
 * @param len Number of samples.
 * @return String Encoded MCC string.
 */
String mcc_encodeAudio(const int8_t* pcm, size_t len);

/**
 * @brief Decode an MCC string to signed 8-bit PCM samples.
 * 
 * @param encoded Pointer to MCC string (null-terminated).
 * @param out_pcm Output buffer to hold decoded PCM samples.
 * @param max_len Maximum number of samples to decode.
 */
void mcc_decodeToPCM(const char* encoded, int8_t* out_pcm, size_t max_len);

#endif // MCC_H
