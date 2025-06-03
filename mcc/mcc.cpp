#include "mcc.h"

// Helper: Split String by spaces into an array of tokens
static void splitStringBySpace(const String& str, String tokens[], size_t& count, size_t maxTokens) {
  count = 0;
  int start = 0;
  int end = str.indexOf(' ', start);
  while (end != -1 && count < maxTokens) {
    tokens[count++] = str.substring(start, end);
    start = end + 1;
    end = str.indexOf(' ', start);
  }
  if (count < maxTokens && start < str.length()) {
    tokens[count++] = str.substring(start);
  }
}

/**
 * @brief Encode raw signed 8-bit audio data into a space-separated MCC string.
 *        Format: each sample converted to int, separated by spaces.
 */
String mcc_encode(const int8_t* input, size_t length) {
  String result = "";
  for (size_t i = 0; i < length; i++) {
    result += String(input[i]);
    if (i != length - 1) result += " ";
  }
  return result;
}

/**
 * @brief Decode MCC string (space-separated tokens) into signed 8-bit audio samples.
 * @return Number of samples decoded.
 */
size_t decodeMCC(const String& mcc_string, int8_t* output, size_t max_output_len) {
  if (mcc_string.length() == 0 || max_output_len == 0) return 0;

  // Split MCC string by spaces into tokens
  const size_t maxTokens = max_output_len;
  String tokens[maxTokens];
  size_t tokenCount = 0;
  splitStringBySpace(mcc_string, tokens, tokenCount, maxTokens);

  size_t outputCount = 0;
  for (size_t i = 0; i < tokenCount; i++) {
    // Ignore empty tokens (defensive)
    if (tokens[i].length() == 0) continue;

    // Try to convert token to integer
    int val = tokens[i].toInt();
    if (val < -128 || val > 127) {
      // Out of int8 range, clamp or stop
      val = constrain(val, -128, 127);
    }
    output[outputCount++] = (int8_t)val;
    if (outputCount >= max_output_len) break;
  }
  return outputCount;
}

/**
 * @brief Validate MCC string is strictly space-separated tokens.
 *        No tabs, newlines, commas allowed.
 */
bool isValidMCCString(const String& mcc_string) {
  for (unsigned int i = 0; i < mcc_string.length(); i++) {
    char c = mcc_string.charAt(i);
    if (!(c == ' ' || (c >= '0' && c <= '9') || c == '-' || c == '^' || c == '#'
          || c == '(' || c == ')')) {
      return false; // invalid char found
    }
  }
  // Check no multiple spaces in a row
  for (unsigned int i = 0; i < mcc_string.length() - 1; i++) {
    if (mcc_string.charAt(i) == ' ' && mcc_string.charAt(i + 1) == ' ')
      return false; // consecutive spaces not allowed
  }
  return true;
}

/**
 * @brief Replace every 5 consecutive occurrences of pattern "#(# # # # #)"
 *        with a single caret '^' in the string.
 *        Pattern example: "#(1 2 3 4 5)"
 */
String replacePatternsWithCaret(const String& mcc_string) {
  String patternPrefix = "#(";
  const int patternLen = 13; // length of "#(# # # # #)" is always 13 chars
  
  String result = "";
  int i = 0;
  int len = mcc_string.length();

  while (i < len) {
    // Try to detect the pattern starting at i
    bool isPattern = false;
    if (i + patternLen <= len) {
      String sub = mcc_string.substring(i, i + patternLen);
      // Check if matches pattern: starts with "#(" and ends with ")"
      // and has five space separated numbers inside
      if (sub.startsWith(patternPrefix) && sub.endsWith(")")) {
        // crude validation of five numbers separated by spaces inside
        // example: "#(1 2 3 4 5)"
        String inner = sub.substring(2, patternLen - 1);
        int spaceCount = 0;
        for (unsigned int j = 0; j < inner.length(); j++) {
          if (inner.charAt(j) == ' ') spaceCount++;
        }
        if (spaceCount == 4) { // 5 numbers have 4 spaces
          isPattern = true;
        }
      }
    }

    if (isPattern) {
      // Count consecutive patterns
      int countPatterns = 1;
      int pos = i + patternLen;
      while (pos + patternLen <= len) {
        String nextSub = mcc_string.substring(pos, pos + patternLen);
        if (nextSub == mcc_string.substring(i, i + patternLen)) {
          countPatterns++;
          pos += patternLen;
        } else {
          break;
        }
      }

      // Replace each group of 5 patterns with a caret
      int fullGroups = countPatterns / 5;
      int remainder = countPatterns % 5;

      for (int g = 0; g < fullGroups; g++) {
        result += "^ ";
      }
      // Append remaining patterns as is
      for (int r = 0; r < remainder; r++) {
        result += mcc_string.substring(i, i + patternLen) + " ";
      }
      i += countPatterns * patternLen;
    } else {
      // Copy character normally
      result += mcc_string.charAt(i);
      i++;
    }
  }

  // Trim trailing spaces
  while (result.endsWith(" ")) {
    result.remove(result.length() - 1);
  }

  return result;
}
