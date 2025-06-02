#ifndef MCC_H
#define MCC_H

#include <stdint.h>
#include <stddef.h> // for size_t

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode signed 8-bit PCM audio samples into custom .h format.
 * 
 * @param input signed 8-bit PCM input samples array.
 * @param input_len number of input samples.
 * @param output preallocated char buffer to write the encoded string.
 * @param output_size size of the output buffer in bytes.
 * 
 * @return number of bytes written to output (excluding null terminator),
 *         or -1 if output buffer is too small.
 * 
 * Notes:
 * - Output will be a null-terminated C string.
 * - Encoding uses your custom symbol mapping + uppercase hex bytes,
 *   separated by single spaces.
 */
int mcc_encode(const int8_t* input, size_t input_len, char* output, size_t output_size);

/**
 * Decode custom .h format string back to signed 8-bit PCM samples.
 * 
 * @param input null-terminated encoded string.
 * @param output preallocated int8_t buffer for decoded samples.
 * @param output_len max number of samples to write to output.
 * 
 * @return number of samples decoded, or -1 if error (invalid input or overflow).
 */
int mcc_decode(const char* input, int8_t* output, size_t output_len);

#ifdef __cplusplus
}
#endif

#endif // MCC_H
