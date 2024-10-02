// str.h
#ifndef STR_H
#define STR_H

#include <stdbool.h>
#include <stddef.h>

// The minimum capacity of a string
#define STR_MIN_CAPACITY 16
#define STR_NPOS -1

// A dynamically resizable string
typedef struct {
  size_t length;    // The length of the string
  size_t capacity;  // The capacity of the string
  char data[];      // The string data as a flexible array member
} str;

// ========== Creation and destruction ==========

// Create a new empty string with the given capacity.
__attribute__((warn_unused_result)) str* str_new(size_t capacity);

// Create a new string from a C string.
__attribute__((warn_unused_result)) str* str_from(const char* cstr);

// Allocate and create a formatted string.
__attribute__((format(printf, 1, 2))) str* str_format(const char* format, ...);

// Free the memory used by a string.
void str_free(str* s);

// ========== Information ==========

// Get the length of the string.
inline size_t str_len(const str* s);

// Get the capacity of the string.
inline size_t str_capacity(const str* s);

// Check if the string is empty.
inline bool str_empty(const str* s);

// Ensure that the string has at least the given capacity.
inline bool str_ensure_capacity(str** s, size_t capacity);

// ============ Modification ============

// Append a C string to the end of the string.
bool str_append(str** s, const char* append);

// Append a formatted string to the end of the string.
bool str_append_fmt(str** s, const char* format, ...);

// Append a character to the end of the string.
bool str_append_char(str** s, char c);

// Prepend a C string to the beginning of the string.
bool str_prepend(str** s, const char* prepend);

// Insert a C string at the given index in the string.
bool str_insert(str** s, size_t index, const char* insert);

// Remove a substring from the string at the given index.
// The count parameter specifies the number of characters to remove.
bool str_remove(str** s, size_t index, size_t count);

// Remove all occurrences of a substring from the string.
// Returns the number of occurrences removed.
size_t str_remove_all(str** s, const char* substr);

// Clear the contents of the string.
inline void str_clear(str* s);

// Resize the string to the given length.
bool str_resize(str** s, size_t new_length);

// ============ Access ============

// Get the character at the given index in the string.
inline char str_at(const str* s, size_t index);

// Get a pointer to the internal data of the string.
inline char* str_data(str* s);

// Get a pointer to the internal data of the string (const version).
inline const char* str_cstr(const str* s);

// =========== Comparison and search ==================

// Compare two strings lexicographically.
inline int str_compare(const str* s1, const str* s2);

// Check if two strings are equal.
inline bool str_equals(const str* s1, const str* s2);

// Check if the string starts with the given prefix.
inline bool str_starts_with(const str* s, const char* prefix);

// Check if the string ends with the given suffix.
inline bool str_ends_with(const str* s, const char* suffix);

// Find the first occurrence of a substring in the string.
// Returns the index of the first character of the substring or STR_NPOS (-1) if not found.
int str_find(const str* s, const char* substr);

// Find the last occurrence of a substring in the string.
int str_rfind(const str* s, const char* substr);

// ============== Transformation ====================

// Convert the string to lowercase.
void str_to_lower(str* s);

// Convert the string to UPPERCASE.
void str_to_upper(str* s);

// Convert the string to snake_case.
void str_snake_case(str* s);

// Convert the string to camelCase.
void str_camel_case(str* s);

// Convert the string to PascalCase.
void str_pascal_case(str* s);

// Trim leading and trailing whitespace characters from the string.
void str_trim(str* s);

// Remove trailing whitespace characters from the string.
void str_rtrim(str* s);

// Remove leading whitespace characters from the string.
void str_ltrim(str* s);

// ============== Substrings and replacements ==============

// Get a substring of the string starting at the given index.
str* str_substr(const str* s, size_t start, size_t length);

// Replace the first occurrence of a substring in the string.
str* str_replace(const str* s, const char* old, const char* new);

// Replace all occurrences of a substring in the string.
str* str_replace_all(const str* s, const char* old, const char* new);

// ========== Splitting and joining ===========

// Split the string into substrings based on a delimiter.
str** str_split(const str* s, const char* delim, size_t* count);

// Join an array of strings into a single string using a delimiter.
str* str_join(const str** strings, size_t count, const char* delim);

// Reverse the string, returning a new string.
str* str_reverse(const str* s);

// Reverse the string in place.
void str_reverse_in_place(str* s);

#endif  // STR_H

#ifdef STR_IMPLEMENTATION

// str.c
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static inline size_t str_round_capacity(size_t capacity) {
  size_t result = STR_MIN_CAPACITY;
  while (result < capacity) {
    result *= 2;
  }
  return result;
}

str* str_new(size_t capacity) {
  capacity = str_round_capacity(MAX(capacity, 1));
  str* s = malloc(sizeof(str) + capacity);
  if (s) {
    s->length = 0;
    s->capacity = capacity;
    s->data[0] = '\0';
  }
  return s;
}

str* str_from(const char* cstr) {
  if (!cstr)
    return NULL;
  size_t len = strlen(cstr);
  str* s = str_new(len + 1);
  if (s) {
    memcpy(s->data, cstr, len + 1);
    s->length = len;
  }
  return s;
}

str* str_format(const char* format, ...) {
  va_list args;
  va_start(args, format);

  // Get the required size for the formatted string
  va_list args_copy;
  va_copy(args_copy, args);
  int size = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (size < 0) {
    va_end(args);
    return NULL;
  }

  str* str = str_new(size + 1);
  if (!str) {
    va_end(args);
    return NULL;
  }

  // Append the formatted string
  int written = vsnprintf(str->data, str->capacity, format, args);
  va_end(args);

  if (written < 0 || written >= str->capacity) {
    // Handle error or truncation
    str->length = str->capacity - 1;
    str->data[str->length] = '\0';
  } else {
    str->length = written;
  }

  return str;
}

void str_free(str* s) {
  if (s)
    free(s);
}

size_t str_len(const str* s) {
  return s ? s->length : 0;
}

size_t str_capacity(const str* s) {
  return s ? s->capacity : 0;
}

bool str_empty(const str* s) {
  return !s || s->length == 0;
}

bool str_ensure_capacity(str** s, size_t capacity) {
  if (!s || !*s)
    return false;

  if ((*s)->capacity >= capacity)
    return true;

  capacity = str_round_capacity(capacity);
  str* new_s = realloc(*s, sizeof(str) + capacity);
  if (!new_s)
    return false;

  new_s->capacity = capacity;
  *s = new_s;
  return true;
}

bool str_append(str** s, const char* append) {
  if (!s || !*s || !append)
    return false;
  size_t append_len = strlen(append);
  if (!str_ensure_capacity(s, (*s)->length + append_len + 1))
    return false;

  // Append the string and null-terminator
  memcpy((*s)->data + (*s)->length, append, append_len + 1);
  (*s)->length += append_len;
  return true;
}

bool str_append_fmt(str** s, const char* format, ...) {
  va_list args;
  va_start(args, format);

  // Get the required size for the formatted string
  va_list args_copy;
  va_copy(args_copy, args);
  int size = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);

  if (size < 0)
    return false;

  if (!str_ensure_capacity(s, (*s)->length + size + 1))
    return false;

  // Append the formatted string
  int written = vsnprintf((*s)->data + (*s)->length, size + 1, format, args);
  va_end(args);

  if (written < 0 || written >= size) {
    // Handle error or truncation
    (*s)->length += size;
    (*s)->data[(*s)->length] = '\0';
  } else {
    (*s)->length += written;
  }

  return true;
}

bool str_append_char(str** s, char c) {
  if (!s || !*s)
    return false;
  if (!str_ensure_capacity(s, (*s)->length + 2))
    return false;

  (*s)->data[(*s)->length++] = c;
  (*s)->data[(*s)->length] = '\0';
  return true;
}

bool str_prepend(str** s, const char* prepend) {
  if (!s || !*s || !prepend)
    return false;
  size_t prepend_len = strlen(prepend);
  if (!str_ensure_capacity(s, (*s)->length + prepend_len + 1))
    return false;

  memmove((*s)->data + prepend_len, (*s)->data, (*s)->length + 1);
  memcpy((*s)->data, prepend, prepend_len);
  (*s)->length += prepend_len;
  return true;
}

bool str_insert(str** s, size_t index, const char* insert) {
  if (!s || !*s || !insert || index > (*s)->length)
    return false;
  size_t insert_len = strlen(insert);
  if (!str_ensure_capacity(s, (*s)->length + insert_len + 1))
    return false;

  memmove((*s)->data + index + insert_len, (*s)->data + index, (*s)->length - index + 1);
  memcpy((*s)->data + index, insert, insert_len);
  (*s)->length += insert_len;
  return true;
}

bool str_remove(str** s, size_t index, size_t count) {
  if (!s || !*s || index >= (*s)->length)
    return false;
  count = MIN(count, (*s)->length - index);
  memmove((*s)->data + index, (*s)->data + index + count, (*s)->length - index - count + 1);
  (*s)->length -= count;
  return true;
}

void str_clear(str* s) {
  if (s) {
    s->length = 0;
    s->data[0] = '\0';
  }
}

size_t str_remove_all(str** s, const char* substr) {
  if (!s || !*s || !substr)
    return 0;

  size_t substr_len = strlen(substr);
  size_t count = 0;
  char* p = (*s)->data;

  while ((p = strstr(p, substr))) {
    memmove(p, p + substr_len, (*s)->length - (p - (*s)->data) - substr_len + 1);
    (*s)->length -= substr_len;
    ++count;
  }

  return count;
}

bool str_resize(str** s, size_t new_length) {
  if (!s || !*s)
    return false;
  if (!str_ensure_capacity(s, new_length + 1))
    return false;

  if (new_length > (*s)->length) {
    // Fill the new space with zeros
    memset((*s)->data + (*s)->length, 0, new_length - (*s)->length);
  }
  (*s)->length = new_length;
  (*s)->data[new_length] = '\0';
  return true;
}

char str_at(const str* s, size_t index) {
  return (s && index < s->length) ? s->data[index] : '\0';
}

char* str_data(str* s) {
  return s ? s->data : NULL;
}

const char* str_cstr(const str* s) {
  return s ? s->data : NULL;
}

int str_compare(const str* s1, const str* s2) {
  if (!s1 || !s2)
    return s1 == s2 ? 0 : (s1 ? 1 : -1);
  return strcmp(s1->data, s2->data);
}

bool str_equals(const str* s1, const str* s2) {
  return str_compare(s1, s2) == 0;
}

bool str_starts_with(const str* s, const char* prefix) {
  if (!s || !prefix)
    return false;
  size_t prefix_len = strlen(prefix);
  return s->length >= prefix_len && memcmp(s->data, prefix, prefix_len) == 0;
}

bool str_ends_with(const str* s, const char* suffix) {
  if (!s || !suffix)
    return false;
  size_t suffix_len = strlen(suffix);
  return s->length >= suffix_len &&
         memcmp(s->data + s->length - suffix_len, suffix, suffix_len) == 0;
}

int str_find(const str* s, const char* substr) {
  if (!s || !substr)
    return STR_NPOS;
  char* found = strstr(s->data, substr);
  return found ? (int)(found - s->data) : STR_NPOS;
}

int str_rfind(const str* s, const char* substr) {
  if (!s || !substr || !*substr)
    return STR_NPOS;

  size_t substr_len = strlen(substr);
  if (substr_len > s->length)
    return STR_NPOS;

  for (size_t i = s->length - substr_len + 1; i > 0; --i) {
    if (memcmp(s->data + i - 1, substr, substr_len) == 0) {
      return i - 1;
    }
  }
  return STR_NPOS;
}

void str_to_lower(str* s) {
  if (!s)
    return;
  for (size_t i = 0; i < s->length; ++i) {
    s->data[i] = tolower(s->data[i]);
  }
}

void str_to_upper(str* s) {
  if (!s)
    return;
  for (size_t i = 0; i < s->length; ++i) {
    s->data[i] = toupper(s->data[i]);
  }
}

void str_snake_case(str* s) {
  if (!s)
    return;
  for (size_t i = 0; i < s->length; ++i) {
    if (isupper(s->data[i])) {
      s->data[i] = tolower(s->data[i]);
      if (i > 0) {
        str_insert(&s, i, "_");
        ++i;
      }
    }
  }
}

void str_camel_case(str* s) {
  if (!s || s->length == 0) {
    return;
  }

  size_t read = 0, write = 0;
  bool capitalize_next = false;

  // First character should be lowercase
  s->data[write++] = (char)tolower(s->data[read++]);

  while (read < s->length) {
    char c = s->data[read++];

    if (c == ' ' || c == '_') {
      capitalize_next = true;
    } else if (capitalize_next) {
      s->data[write++] = (char)toupper(c);
      capitalize_next = false;
    } else {
      s->data[write++] = (char)tolower(c);
    }
  }

  s->data[write] = '\0';
  s->length = write;
}

void str_pascal_case(str* s) {
  if (!s || s->length == 0) {
    return;
  }

  size_t read = 0, write = 0;

  // Start with true to capitalize the first letter
  bool new_word = true;

  while (read < s->length) {
    char c = s->data[read++];

    if (c == ' ' || c == '_') {
      new_word = true;
    } else if (new_word) {
      s->data[write++] = (char)toupper(c);
      new_word = false;
    } else if (isupper(c) && read < s->length && islower(s->data[read])) {
      // If current char is uppercase and next char is lowercase,
      // it's the start of a new word in camelCase
      s->data[write++] = c;
      new_word = false;
    } else {
      s->data[write++] = (char)tolower(c);
    }
  }

  s->data[write] = '\0';
  s->length = write;
}

void str_trim(str* s) {
  if (!s || s->length == 0)
    return;

  size_t start = 0, end = s->length - 1;
  while (start < s->length && isspace(s->data[start]))
    ++start;
  while (end > start && isspace(s->data[end]))
    --end;

  s->length = end - start + 1;
  memmove(s->data, s->data + start, s->length);
  s->data[s->length] = '\0';
}

void str_rtrim(str* s) {
  if (!s || s->length == 0)
    return;

  size_t end = s->length - 1;
  while (end > 0 && isspace(s->data[end]))
    --end;

  s->length = end + 1;
  s->data[s->length] = '\0';
}

void str_ltrim(str* s) {
  if (!s || s->length == 0)
    return;

  size_t start = 0;
  while (start < s->length && isspace(s->data[start]))
    ++start;

  s->length -= start;
  memmove(s->data, s->data + start, s->length);
  s->data[s->length] = '\0';
}

str* str_substr(const str* s, size_t start, size_t length) {
  if (!s || start >= s->length)
    return NULL;

  length = MIN(length, s->length - start);
  str* result = str_new(length + 1);
  if (result) {
    memcpy(result->data, s->data + start, length);
    result->data[length] = '\0';
    result->length = length;
  }
  return result;
}

str* str_replace(const str* s, const char* old, const char* new) {
  if (!s || !old || !new)
    return NULL;

  size_t old_len = strlen(old);
  size_t new_len = strlen(new);
  size_t count = 0;
  const char* p = s->data;

  while ((p = strstr(p, old))) {
    ++count;
    p += old_len;
  }

  size_t result_len = s->length + count * (new_len - old_len);
  str* result = str_new(result_len + 1);
  if (!result)
    return NULL;

  char* dest = result->data;
  p = s->data;

  while (*p) {
    if (strncmp(p, old, old_len) == 0) {
      memcpy(dest, new, new_len);
      dest += new_len;
      p += old_len;
    } else {
      *dest++ = *p++;
    }
  }

  *dest = '\0';
  result->length = result_len;
  return result;
}

str* str_replace_all(const str* s, const char* old, const char* new) {
  if (!s || !old || !new)
    return NULL;

  size_t old_len = strlen(old);
  size_t new_len = strlen(new);
  size_t count = 0;
  const char* p = s->data;

  while ((p = strstr(p, old))) {
    ++count;
    p += old_len;
  }

  size_t result_len = s->length + count * (new_len - old_len);
  str* result = str_new(result_len + 1);
  if (!result)
    return NULL;

  char* dest = result->data;
  p = s->data;

  while (*p) {
    if (strncmp(p, old, old_len) == 0) {
      memcpy(dest, new, new_len);
      dest += new_len;
      p += old_len;
    } else {
      *dest++ = *p++;
    }
  }

  *dest = '\0';
  result->length = result_len;
  return result;
}

str** str_split(const str* s, const char* delim, size_t* count) {
  if (!s || !delim || !count)
    return NULL;

  size_t delim_len = strlen(delim);
  size_t max_splits = s->length / delim_len + 1;
  str** result = malloc(max_splits * sizeof(str*));
  if (!result)
    return NULL;

  *count = 0;
  const char* start = s->data;
  const char* end = s->data;

  while ((end = strstr(start, delim))) {
    size_t len = end - start;
    result[*count] = str_new(len + 1);
    if (!result[*count]) {
      for (size_t i = 0; i < *count; ++i)
        str_free(result[i]);
      free(result);
      return NULL;
    }
    memcpy(result[*count]->data, start, len);
    result[*count]->data[len] = '\0';
    result[*count]->length = len;
    ++(*count);
    start = end + delim_len;
  }

  size_t len = s->data + s->length - start;
  result[*count] = str_new(len + 1);
  if (!result[*count]) {
    for (size_t i = 0; i < *count; ++i)
      str_free(result[i]);
    free(result);
    return NULL;
  }
  memcpy(result[*count]->data, start, len);
  result[*count]->data[len] = '\0';
  result[*count]->length = len;
  ++(*count);

  return result;
}

str* str_join(const str** strings, size_t count, const char* delim) {
  if (!strings || count == 0 || !delim)
    return NULL;

  size_t total_len = 0;
  size_t delim_len = strlen(delim);

  for (size_t i = 0; i < count; ++i) {
    if (!strings[i])
      return NULL;
    total_len += strings[i]->length;
  }
  total_len += (count - 1) * delim_len;

  str* result = str_new(total_len + 1);
  if (!result)
    return NULL;

  char* dest = result->data;
  for (size_t i = 0; i < count; ++i) {
    if (i > 0) {
      memcpy(dest, delim, delim_len);
      dest += delim_len;
    }
    memcpy(dest, strings[i]->data, strings[i]->length);
    dest += strings[i]->length;
  }

  *dest = '\0';
  result->length = total_len;
  return result;
}

str* str_reverse(const str* s) {
  if (!s || s->length == 0)
    return NULL;

  str* result = malloc(sizeof(str) + s->length + 1);
  if (!result)
    return NULL;

  result->length = s->length;
  result->capacity = s->length + 1;

  char* src = (char*)s->data + s->length - 1;
  char* dest = result->data;
  size_t len = s->length;

  while (len--) {
    *dest++ = *src--;
  }

  *dest = '\0';
  return result;
}

// Reverse the string in place.
void str_reverse_in_place(str* s) {
  if (!s || s->length < 2)
    return;

  char* start = s->data;
  char* end = s->data + s->length - 1;

  while (start < end) {
    char temp = *start;
    *start++ = *end;
    *end-- = temp;
  }
}


#endif  // STR_IMPLEMENTATION
