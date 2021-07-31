tags: modify, memory

# string_format_cstr

Formats a string using printf format specifiers.

## Syntax

```c
String* string_format_cstr(String* result, const char* format, ...);
```

| Name | Type | Description |
| --- | --- | --- |
| result | String* | A string that stores the result of the format operation. If this is NULL, this function allocates a string for the return value. Otherwise it appends the formatted data to the end. |
| format | const | A c-string that contains the text and format specifiers to be written. |
| ... | ... | The format specifier values. |

**Returns:** result if the argument was non-null. Otherwise a newly allocated string that contains the format result. NULL on error.

## Example

### Function Allocates

```c
String* result = string_format_cstr(NULL, "%s %d", "Test Number:", 42);

puts(string_data(result)); // Test Number: 42

string_free(result);
```

### Caller Allocates

```c
String result = string_create("");

string_format_cstr(NULL, "%s %d", "Test Number:", 42);

puts(string_data(result)); // Test Number: 42

string_free_resources(&result);
```