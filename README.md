**Custom .h Encoding Rules — Conversion from Raw int8 Mono Audio Data**

---

1. **Input**: Raw int8 mono audio data (one byte per sample).

2. **Byte-to-Symbol Mapping**: Replace specific byte values with single-character symbols as follows:

   * `0x00` → `#`
   * `0xFF` → `$`
   * `0x01` → `%`
   * `0xF0` → `&`
   * `0xFC` → `*`
   * `0xEF` → `!`
   * `0x0F` → `@`

3. **Unmapped Bytes**: All other byte values are represented as uppercase two-digit hexadecimal strings without the `0x` prefix (e.g., byte `0x12` → `"12"`).

4. **Separator**: Each encoded byte or symbol is separated by exactly one space (`" "`). No commas, tabs, or multiple spaces are used.

5. **No Concatenation for Hex Bytes**: Two-digit hex strings are never concatenated; always separated by a space.

6. **No Prefixes**: The `0x` prefix is removed from all hexadecimal byte representations.

7. **No Newlines or Tabs**: The entire data can be kept in a single line or broken into lines only by spaces. Tabs or other whitespace characters are not used.

8. **Output Format**: The resulting string is stored in a `.h` file as a plain C string or array for direct inclusion.

---

**End of rules**
