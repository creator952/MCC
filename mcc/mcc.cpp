#include "mcc.h"

// Your custom byte to symbol mapping
// We'll store pairs in arrays for minimal code
static const uint8_t bytes[] = { 0x00, 0xFF, 0x01, 0xF0, 0xFC, 0xEF, 0x0F };
static const char symbols[] = { '#', '$', '%', '&', '*', '!', '@' };
static const size_t mapping_count = sizeof(bytes) / sizeof(bytes[0]);

static int find_symbol_for_byte(uint8_t byte) {
    for (size_t i = 0; i < mapping_count; i++) {
        if (bytes[i] == byte) return symbols[i];
    }
    return 0; // no symbol found
}

static int find_byte_for_symbol(char c) {
    for (size_t i = 0; i < mapping_count; i++) {
        if (symbols[i] == c) return bytes[i];
    }
    return -1; // not found
}

// Helper: Convert uint8_t to uppercase hex, 2 chars, output to buffer
// Assumes buffer has space for 2 chars (no null terminator here)
static void uint8_to_hex(uint8_t val, char* buffer) {
    const char hex_chars[] = "0123456789ABCDEF";
    buffer[0] = hex_chars[(val >> 4) & 0x0F];
    buffer[1] = hex_chars[val & 0x0F];
}

// Helper: Convert 2 hex chars to uint8_t, returns -1 if invalid hex
static int hex_to_uint8(const char* hex) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = hex[i];
        int digit = -1;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else return -1; // invalid char
        val = (val << 4) | digit;
    }
    return val;
}

int mcc_encode(const int8_t* input, size_t input_len, char* output, size_t output_size) {
    if (!input || !output || output_size == 0) return -1;
    size_t out_pos = 0;

    for (size_t i = 0; i < input_len; i++) {
        uint8_t byte = (uint8_t)input[i];

        // Find symbol if any
        int sym = find_symbol_for_byte(byte);
        char buf[2];

        if (sym) {
            // Symbol is 1 char
            if (out_pos + 1 + (i > 0 ? 1 : 0) >= output_size) return -1;
            if (i > 0) output[out_pos++] = ' ';
            output[out_pos++] = (char)sym;
        } else {
            // Hex output 2 chars + space if not first token
            if (out_pos + 2 + (i > 0 ? 1 : 0) >= output_size) return -1;
            if (i > 0) output[out_pos++] = ' ';
            uint8_to_hex(byte, buf);
            output[out_pos++] = buf[0];
            output[out_pos++] = buf[1];
        }
    }

    if (out_pos >= output_size) return -1; // no space for null terminator
    output[out_pos] = '\0';
    return (int)out_pos;
}

int mcc_decode(const char* input, int8_t* output, size_t output_len) {
    if (!input || !output) return -1;
    size_t out_pos = 0;

    size_t i = 0;
    while (input[i]) {
        // Skip spaces
        while (input[i] == ' ') i++;
        if (input[i] == '\0') break;

        // Check token length:
        // token can be one symbol char or two hex chars
        if (input[i + 1] == '\0' || input[i + 1] == ' ') {
            // One-char token
            char c = input[i];
            int val = find_byte_for_symbol(c);
            if (val >= 0) {
                if (out_pos >= output_len) return -1;
                output[out_pos++] = (int8_t)val;
                i++;
            } else {
                // Could be hex one-char? invalid in your spec, error
                return -1;
            }
        } else {
            // Two-char token
            char c1 = input[i];
            char c2 = input[i + 1];
            if ((input[i + 2] != '\0') && (input[i + 2] != ' ')) {
                // More than 2 chars token - error in your spec
                return -1;
            }

            int val = hex_to_uint8(&input[i]);
            if (val < 0) return -1;
            if (out_pos >= output_len) return -1;
            output[out_pos++] = (int8_t)val;
            i += 2;
        }

        // Skip trailing space after token if any
        if (input[i] == ' ') i++;
    }

    return (int)out_pos;
}
