tags: modify, memory

# string_format_time_string

Formats a string based on a time value using strftime format specifiers.

## Syntax

```c
String* string_format_time_string(String* result, const String* format, const struct tm* timeptr);
```

| Name | Type | Description |
| --- | --- | --- |
| result | String* | A string that stores the result of the format operation. If this is NULL, this function allocates a string for the return value. Otherwise it appends the formatted data to the end. |
| format | const |  A string that contains the text and time format specifiers to be written. |
| timeptr | const | The time value used to format the string. |

**Returns:** result if the argument was non-null. Otherwise a newly allocated string that contains the time format result. NULL on error.

## Example

### Function Allocates

```c
struct tm time_value;
time_value.tm_hour = 13;
time_value.tm_min = 30;
time_value.tm_sec = 2;

String format = string_create("%T");

String* result = string_format_time_string(NULL, &format, &time_value);

string_free(result);
string_free_resources(&format);

// Output:
// 13:30:02
```

### Caller Allocates

```c
struct tm time_value;
time_value.tm_hour = 13;
time_value.tm_min = 30;
time_value.tm_sec = 2;

String result = string_create("");
String format = string_create("%T");

string_format_time_string(result, &format, &time_value);

string_free_resources(&result);
string_free_resources(&format);

// Output:
// 13:30:02
```