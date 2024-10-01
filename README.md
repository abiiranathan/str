# str

A single header-only C library for manipulating strings in C.

## Features

- `str_new` - Create a new string
- `str_from` - Create a new string from a C string
- `str_free` - Free a string
- `str_len` - Get the length of a string
- `str_capacity` - Get the capacity of a string
- `str_empty` - Check if a string is empty or NULL
- `str_ensure_capacity` - Ensure a string has enough capacity
- `str_append` - Append a char * to a string
- `str_append_char` - Append a char to a string
- `str_prepend` - Prepend a char * to a string
- `str_insert` - Insert a C string at a given index
- `str_remove` - Remove a substring from a string starting at a given index
- `str_clear` - Empty a string(keep the capacity)
- `str_resize` - Resize a string to a given length
- `str_at` - Get the character at a given index
- `str_data` - Get the underlying C string
- `str_cstr` - Get the underlying C string(const version)
- `str_compare` - Compare two strings
- `str_equals` - Check if two strings are equal
- `str_starts_with` - Check if a string starts with a given prefix
- `str_ends_with` - Check if a string ends with a given suffix
- `str_find` - Find the first occurrence of a substring in a string
- `str_rfind` - Find the last occurrence of a substring in a string
- `str_to_lower` - Convert a string to lowercase
- `str_to_upper` - Convert a string to uppercase
- `str_snake_case` - Convert a string to snake_case
- `str_camel_case` - Convert a string to camelCase
- `str_pascal_case` - Convert a string to PascalCase
- `str_trim` - Trim leading and trailing whitespace from a string
- `str_rtrim` - Trim trailing whitespace from a string
- `str_ltrim` - Trim leading whitespace from a string
- `str_substr` - Get a substring of a string
- `str_replace` - Replace the first occurrence of a substring in a string
- `str_replace_all` - Replace all occurrences of a substring in a string
- `str_split` - Split a string into an array of substrings
- `str_join` - Join an array of strings into a single string
- `str_reverse` - Reverse a string
- `str_reverse_in_place` - Reverse a string in place
  


## Usage

```c
#define STR_IMPLEMENTATION
#include "str.h"

int main() {
    str_t *s = str_new(16);
    str_append(s, "Hello, ");
    str_append(s, "world!");
    printf("%s\n", str_data(s));
    str_free(s);
    return 0;
}

```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

