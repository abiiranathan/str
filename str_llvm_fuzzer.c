#define STR_IMPLEMENTATION

#include <stdint.h>
#include "str.h"

// Function to fuzz
int process_input(const char* input) {
  str* s = str_from(input);
  str_trim(s);
  str_reverse_in_place(s);
  str_append(&s, "Hello");
  str_to_upper(s);
  str_free(s);

  // TODO: Add more functions to fuzz
  return 0;
}

// Fuzzer entry point
int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  // Convert fuzzed data to a string
  char* input = (char*)malloc(size + 1);
  if (!input) {
    return 0;
  }

  memcpy(input, data, size);
  input[size] = '\0';

  // Call the function to fuzz
  process_input(input);

  return 0;  // Return 0 indicates success
}

// compile with:
// clang -fsanitize=fuzzer str_llvm_fuzzer.c -o str_llvm_fuzzer
// ./str_llvm_fuzzer -max_total_time=15 -rss_limit_mb=3000