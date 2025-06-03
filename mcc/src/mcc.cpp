#include "mcc.h"

// Default sample rate (can be overridden with mcc_setSampleRate)
uint32_t mcc_sampleRate = 8000;

/**
 * Set the global sample rate
 */
void mcc_setSampleRate(uint32_t hz) {
    mcc_sampleRate = hz;
}

/**
 * Encode a single signed 8-bit PCM sample into an MCC character.
 */
char mcc_encode(int8_t pcm) {
    // Map -128..127 to 0..255
    uint8_t val = static_cast<uint8_t>(pcm + 128);

    // Now we map 0–255 to printable MCC characters (e.g., ASCII 35–126)
    // Total range: 92 characters (126 - 35 + 1) → scale accordingly
    uint8_t scaled = (val * 92) / 256;

    // Return printable char starting from '#'
    return static_cast<char>(35 + scaled);
}

/**
 * Decode an MCC character back into signed 8-bit PCM.
 */
int8_t mcc_decode(char c) {
    // Clamp character to range '#'(35) to '~'(126)
    if (c < 35 || c > 126) return 0;

    // Convert char to 0–91
    uint8_t scaled = static_cast<uint8_t>(c - 35);

    // Scale back to 0–255
    uint8_t val = (scaled * 256) / 92;

    // Convert to signed 8-bit PCM
    return static_cast<int8_t>(val - 128);
}

/**
 * Encode an array of signed 8-bit PCM audio to an MCC string.
 */
String mcc_encodeAudio(const int8_t* pcm, size_t len) {
    String encoded = "";

    for (size_t i = 0; i < len; i++) {
        encoded += mcc_encode(pcm[i]);
    }

    return encoded;
}

/**
 * Decode MCC string into PCM audio array.
 */
void mcc_decodeToPCM(const char* encoded, int8_t* out_pcm, size_t max_len) {
    for (size_t i = 0; i < max_len && encoded[i] != '\0'; i++) {
        out_pcm[i] = mcc_decode(encoded[i]);
    }
}
